/* Copyright (C) 2007-2009 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * This exception applies only to the code released under the name GNU
 * AstCTIServer.  If you copy code from other releases into a copy of GNU
 * AstCTIServer, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTIServer, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include "QsLog.h"
#include "coretcpserver.h"
#include "ctiserverapplication.h"

/*!
    Our CoreTcpServer derive from QTcpServer
*/
CoreTcpServer::CoreTcpServer(AstCtiConfiguration *config, QObject *parent)
	: QTcpServer(parent)
{
    this->isClosing = false;
	this->amiStatus = AmiConnectionStatusDisconnected;
    this->config = config;

	//Initialize list of CTI commands
	this->commandsList.insert("QUIT",       CmdQuit);
	this->commandsList.insert("CMPR",       CmdCompression);
	this->commandsList.insert("NOOP",       CmdNoOp);
	this->commandsList.insert("USER",       CmdUser);
	this->commandsList.insert("PASS",       CmdPass);
	this->commandsList.insert("CHPW",       CmdChangePassword);
	this->commandsList.insert("KEEP",       CmdKeep);
	this->commandsList.insert("OSTYPE",     CmdOsType);
	this->commandsList.insert("SERVICES",   CmdServices);
	this->commandsList.insert("QUEUES",     CmdQueues);
	this->commandsList.insert("PAUSE",      CmdPause);
	this->commandsList.insert("ORIG",       CmdOrig);
	this->commandsList.insert("MAC",        CmdMac);

	connect(this, SIGNAL(newConnection()),
			this, SLOT(newConnection()));

	qRegisterMetaType<AstCtiConfiguration*>("AstCtiConfiguration*");
	qRegisterMetaType<AstCtiCall*>("AstCtiCall*");
	qRegisterMetaType<AmiCommand*>("AMICommand*");

	this->amiClient = new AmiClient();
	this->amiClient->setParameters(this->config);
	connect(this->amiClient, SIGNAL(amiConnectionStatusChange(const AmiConnectionStatus)),
			this, SLOT(amiConnectionStatusChange(const AmiConnectionStatus)));
	connect(this->amiClient, SIGNAL(asteriskEvent(const AmiEvent, AstCtiCall*)),
			this, SLOT(receiveAsteriskEvent(const AmiEvent, AstCtiCall*)));
	connect(this->amiClient, SIGNAL(asteriskResponse(AmiCommand*)),
			this, SLOT(processAsteriskResponse(AmiCommand*)));
	connect(this, SIGNAL(newAmiCommand(AmiCommand*)),
			this->amiClient, SLOT(sendCommandToAsterisk(AmiCommand*)));
	connect(this, SIGNAL(newAmiConfiguration(AstCtiConfiguration*)),
			this->amiClient, SLOT(setParameters(AstCtiConfiguration*)));

	QThread *amiThread = new QThread(this);
	this->amiClient->moveToThread(amiThread);
	connect(amiThread, SIGNAL(started()),
			this->amiClient, SLOT(run()));
	connect(amiThread, SIGNAL(finished()),
			this->amiClient, SLOT(deleteLater()));
	amiThread->start();
}

CoreTcpServer::~CoreTcpServer()
{
	this->amiClient->stop();
	// amiClient will be deleted automatically because the thread's
	// finished() signal is connected to amiClient's deleteLater() slot
	QThread *amiThread = this->amiClient->thread();
	amiThread->quit();
	amiThread->wait();
	delete amiThread;

	//Closing AMI connection will disconnect all CTI clients

	delete this->config;
}

AstCtiConfiguration *CoreTcpServer::getConfig()
{
	return this->config;
}

void CoreTcpServer::setConfig(AstCtiConfiguration *newConfig)
{
	this->stopListening();

	AstCtiConfiguration *tmpConfig = this->config;

	bool amiParamsChanged = newConfig->amiHost != tmpConfig->amiHost ||
							newConfig->amiPort != tmpConfig->amiPort ||
							newConfig->amiUser != tmpConfig->amiUser ||
							newConfig->amiSecret != tmpConfig->amiSecret ||
							newConfig->amiReadTimeout != tmpConfig->amiReadTimeout ||
							newConfig->amiConnectTimeout != tmpConfig->amiConnectTimeout ||
							newConfig->amiConnectRetryAfter != tmpConfig->amiConnectRetryAfter;
	if (amiParamsChanged)
		emit this->newAmiConfiguration(newConfig);

	bool amiConnectionDropped = newConfig->amiHost != tmpConfig->amiHost ||
								newConfig->amiPort != tmpConfig->amiPort ||
								newConfig->amiUser != tmpConfig->amiUser ||
								newConfig->amiSecret != tmpConfig->amiSecret;

	if (!amiConnectionDropped) {
		//If AMI client disconnects, CTI server will disconnect all clients,
		//so we don't need to do any further checking, but if not,
		//we must check whether server parameters were changed

		bool ctiParamsChanged = newConfig->ctiServerAddress != tmpConfig->ctiServerAddress ||
								newConfig->ctiServerPort != tmpConfig->ctiServerPort ||
								newConfig->ctiCompressionLevel != tmpConfig->ctiCompressionLevel;
		if (ctiParamsChanged) {
			//CTI server parameters are changed, all clients should be disconnected
			this->disconnectClients();
		} else {
			//Check if operators or seats are changed, some clients may have to be disconnected
			foreach (ClientManager *cm, this->clients) {
				bool disconnected = false;
				AstCtiOperator *activeOp = cm->activeOperator;
				if (activeOp != 0) {
					QString username = activeOp->getUsername();
					AstCtiOperator *op = tmpConfig->operators.value(username);
					if (op == 0) {
						//Operator does not exist in new config
						//Disconnection will delete client and remove it from clients hash
						cm->socket->disconnectFromHost();
						disconnected = true;
					} else {
						if (activeOp->getPassword() != op->getPassword() ||
							activeOp->getSeatId() != op->getSeatId()) {
							//Critical operator parameters have changed
							cm->socket->disconnectFromHost();
							disconnected = true;
						} else {
							//Operator is unchanged in new config
							cm->activeOperator = op;
						}
					}
				}

				if (!disconnected) {
					AstCtiSeat *activeSeat = cm->activeSeat;
					if (activeSeat != 0) {
						int id = activeSeat->getId();
						AstCtiSeat *seat = tmpConfig->seats.value(id);
						if (seat == 0) {
							//Seat does not exist in new config
							//Disconnection will delete client and remove it from clients hash
							cm->socket->disconnectFromHost();
						} else {
							if (activeSeat->getMac() != seat->getMac() ||
								activeSeat->getExtensions() != seat->getExtensions()) {
								//Critical seat parameters have changed
								cm->socket->disconnectFromHost();
							} else {
								//Seat is unchanged in new config
								cm->activeSeat = seat;
							}
						}
					}
				}
			}
		}
	}

	this->config = newConfig;
	delete tmpConfig;

	if (!this->startListening())
		this->stopServer();
}

bool CoreTcpServer::startListening()
{
	if (!this->isListening()) {
		QHostAddress bindAddress;
		if (!bindAddress.setAddress(this->config->ctiServerAddress))
			bindAddress = QHostAddress::Any;

		const bool success = this->listen(bindAddress, this->config->ctiServerPort);
		if (success)
			QLOG_INFO() << "CoreTcpServer listening on socket"
						<< this->config->ctiServerAddress << ":" << this->config->ctiServerPort;
		else
			QLOG_ERROR() << "CoreTcpServer failed listening on socket"
						 << this->config->ctiServerAddress << ":" << this->config->ctiServerPort
						 << "Error:" << this->errorString();
		return success;
	} else {
		QLOG_WARN() << "CoreTcpServer is already listening on socket"
					<< this->config->ctiServerAddress << ":" << this->config->ctiServerPort;
		return true;
	}
}

void CoreTcpServer::stopListening()
{
	if (this->isListening()) {
		this->close();
		QLOG_INFO() << "CoreTcpServer stopped listening on socket"
					<< this->config->ctiServerAddress << ":" << this->config->ctiServerPort;
	}
}

/*!
	Called when a new connection is avaiable.
*/
void CoreTcpServer::newConnection()
{
	QTcpSocket *newSocket = this->nextPendingConnection();

	if (newSocket != 0)  {
		QHostAddress remoteAddr = newSocket->peerAddress();
		quint16 remotePort = newSocket->peerPort();

		QLOG_INFO() << "Incoming connection from" << remoteAddr.toString() << ":" << remotePort;

		connect(newSocket, SIGNAL(readyRead()),
				this, SLOT(socketDataReceived()));
		connect(newSocket, SIGNAL(disconnected()),
				this, SLOT(socketDisconnected()));
		connect(newSocket, SIGNAL(error(QAbstractSocket::SocketError)),
				this, SLOT(socketError(QAbstractSocket::SocketError)));

		ClientManager *cm = new ClientManager();
		cm->socket = newSocket;
		cm->localIdentifier = QString("%1:%2")
							  .arg(remoteAddr.toString()).arg(remotePort);

		this->addClient(newSocket, cm);

		// Let's say Welcome to our client!
		this->sendDataToClient(newSocket, "100 OK Welcome to AstCTIServer");
	}
}

void CoreTcpServer::amiConnectionStatusChange(const AmiConnectionStatus status)
{
	this->amiStatus = status;
    switch(status) {
    case AmiConnectionStatusConnected:
		/* CODE TESTING START */
		//this->stopServer(true);
		//return;
		/* CODE TESTING END */

		if (!this->startListening())
			this->stopServer();
		break;
    case AmiConnectionStatusDisconnected:        
		this->stopListening();
		this->disconnectClients();
		break;
    }
}

void CoreTcpServer::socketDataReceived()
{
	//Retrieve sender QTcpSocket
	QTcpSocket *socket = qobject_cast<QTcpSocket*>(QObject::sender());

	ClientManager *cm = this->clients.value(socket);

	QDataStream in(socket);
	//in.setVersion(QDataStream::Qt_4_5);

	while (!in.atEnd()) {
		if (cm->blockSize == 0) {
			//Read data block size from frame descriptor (if enough data available)
			if (socket->bytesAvailable() >= (int)sizeof(quint16))
				in >> cm->blockSize;
		}

		//Wait for entire data block to be received
		//If not, readyRead will be emitted again when more data is available
		if (cm->blockSize > 0 && socket->bytesAvailable() >= cm->blockSize) {
			QByteArray receivedData;
			QString receivedString;
			if (cm->compressionLevel > 0) {
				//Compressed data will be UTF8 encoded
				in >> receivedData;
				QByteArray uncompressedData = qUncompress(receivedData);
				if (uncompressedData.isEmpty()) {
					QLOG_ERROR() << "Unable to uncompress data received from cliebt. "
									"Trying to use uncompressed data.";
					receivedString = QString::fromUtf8(receivedData);
				} else {
					receivedString = QString::fromUtf8(uncompressedData);
				}
			} else {
				//Uncompressed data will be sent as-is
				in >> receivedString;
			}

			//Reset block size
			cm->blockSize = 0;

			this->processClientData(socket, receivedString);
		}
	}
}

void CoreTcpServer::socketDisconnected() {
	//Retrieve sender QTcpSocket
	QTcpSocket *socket = qobject_cast<QTcpSocket*>(QObject::sender());

	ClientManager *cm = this->clients.value(socket);

	if (cm->isAuthenticated && this->amiStatus == AmiConnectionStatusConnected) {
		// We should do a logoff right now
		this->ctiClientLogoff(cm);
	}
	this->removeUser(cm->ctiUsername);

	this->removeClient(socket);
	delete cm;
	socket->deleteLater();
}

void CoreTcpServer::socketError(QAbstractSocket::SocketError error)
{
	//Retrieve sender QTcpSocket
	QTcpSocket *socket = qobject_cast<QTcpSocket*>(QObject::sender());

	ClientManager *cm = this->clients.value(socket);

	QString iden = cm == 0 ? "" : cm->localIdentifier;
	QLOG_ERROR() << "Client Socket Error:" << error << socket->errorString()
				 << "for client" << iden;
}

void CoreTcpServer::processClientData(QTcpSocket *socket, const QString &data)
{
	ClientManager *cm = this->clients.value(socket);

	QLOG_DEBUG() << (QString("Client %1 << %2")
					 .arg(cm->localIdentifier, -32)
					 .arg(data.simplified())).toUtf8().constData();

	QString parm;
	AstCTICommand cmd = this->parseCommand(data);

	if (!commandsList.contains(cmd.command)) {
		QLOG_ERROR() << "Received unrecognized command:" << cmd.command;
		return;
	}

	switch(commandsList[cmd.command]) {
	case CmdQuit:
		this->sendDataToClient(socket, "900 BYE");
		socket->flush();
		socket->disconnectFromHost();
		break;
	case CmdNoOp:
		this->sendDataToClient(socket, "100 OK");
		break;
	case CmdUser:
		if (cmd.parameters.count() < 1) {
			this->sendDataToClient(socket, "101 KO No username given");
			break;
		} else {
			cm->ctiUsername = cmd.parameters.at(0);

			// Avoid duplicate logins
			AstCtiOperator *ctiOperator = config->operators.value(cm->ctiUsername);
			if (ctiOperator != 0) {
				if (!this->containsUser(cm->ctiUsername)) {
					this->sendDataToClient(socket, "100 OK Send password now");
				} else {
					cm->ctiUsername = "";
					this->sendDataToClient(socket, "101 KO User is still logged in");
				}
			} else {
				cm->ctiUsername = "";
				this->sendDataToClient(socket, "101 KO User doesn't exist");
			}
		}
		break;
	case CmdPass:
		if (cm->ctiUsername.isEmpty())  {
			this->sendDataToClient(socket, "101 KO Send user first");
		} else {
			if (cmd.parameters.count() < 1) {
				this->sendDataToClient(socket, "101 KO No password given");
				break;
			}

			{
				AstCtiOperator *ctiOperator = config->operators.value(cm->ctiUsername);
				if (ctiOperator == 0) {
					this->sendDataToClient(socket, "101 KO Operator not found");
					cm->ctiUsername = "";
					cm->retries--;
				} else {
					if (!ctiOperator->checkPassword(cmd.parameters.at(0))) {
						this->sendDataToClient(socket, "101 KO Wrong password");
						cm->ctiUsername = "";
						cm->retries--;
					} else {
						cm->state = ctiOperator->getBeginInPause() ? StatePaused : StateLoggedIn;
						this->addUser(cm->ctiUsername);
						cm->activeOperator = ctiOperator;
						cm->isAuthenticated = true;
						if (ctiOperator->isCallCenter()) {
							this->sendDataToClient(socket, "102 OK");
						} else {
							AstCtiSeat *seat = config->seats.value(ctiOperator->getSeatId());
							if (seat == 0) {
								this->sendDataToClient(socket, "101 KO Extension(s) not found");
								cm->ctiUsername = "";
								cm->retries--;
							} else {
								cm->activeSeat = seat;
								cm->localIdentifier.append(QString(" (%1)")
														   .arg(seat->getExtensions().join(" ")));
								// We can do a successful cti login only after the seat is known
								this->ctiClientLogin(cm);
								this->sendDataToClient(
										socket, QString("102 OK %1")
												.arg(seat->getExtensions().join(" ")));
							}
						}
					}
				}
			}

			if (cm->retries == 0)
				socket->close();
		}
		break;
	case CmdChangePassword:
		if (!cm->isAuthenticated) {
			this->sendDataToClient(socket, "101 KO Not authenticated");
			break;
		} else {
			if (cmd.parameters.count() < 2) {
				this->sendDataToClient(socket, "101 KO Invalid parameters");
				break;
			}

			if (cm->activeOperator != 0) {
				QString oldPassword = cmd.parameters.at(0);
				QString newPassword = cmd.parameters.at(1);

				if (cm->activeOperator->checkPassword(oldPassword) &&
					cm->activeOperator->changePassword(newPassword)) {
					this->sendDataToClient(socket, "100 OK Password changed successfully");
				} else {
					this->sendDataToClient(socket, "101 KO Password not changed. Error occurred");
				}
			} else {
				this->sendDataToClient(socket, "101 KO Not authenticated");
			}

		}
	case CmdMac:
		if (!cm->isAuthenticated) {
			this->sendDataToClient(socket, "101 KO Not authenticated");
			break;
		}
		if (cm->activeSeat != 0) {
			this->sendDataToClient(socket, "101 KO Seat already set");
			break;
		}

		if (cmd.parameters.count() < 1) {
			this->sendDataToClient(socket, "101 KO No MAC given");
			break;
		} else {
			QString mac = cmd.parameters.at(0).toLower();
			AstCtiSeat *seat = config->getSeatByMac(mac);
			if (seat != 0) {
				cm->activeSeat = seat;
				cm->localIdentifier.append(QString(" (%1)").arg(seat->getExtensions().join(" ")));
				// We can do a successful cti login only after the seat is known
				this->ctiClientLogin(cm);
				this->sendDataToClient(socket, QString("100 OK %1")
											   .arg(seat->getExtensions().join(" ")));
			} else {
				this->sendDataToClient(socket, "101 KO MAC address unknown");
			}
		}
		break;
	case CmdKeep:
		this->sendDataToClient(socket, QString("104 OK %1").arg(config->ctiReadTimeout));
		break;
	case CmdCompression:
		this->sendDataToClient(socket, QString("103 %1").arg(this->config->ctiCompressionLevel));
		cm->compressionLevel = this->config->ctiCompressionLevel;
		break;
	case CmdOsType:
		if (!cm->isAuthenticated) {
			cm->clientOperatingSystem = "";
			this->sendDataToClient(socket, "101 KO Not authenticated");
			break;
		}
		if (cmd.parameters.count() < 1) {
			cm->clientOperatingSystem = "";
			this->sendDataToClient(socket, "101 KO Operating system not set");
			break;
		} else {
			cm->clientOperatingSystem = cmd.parameters.at(0).toUpper();
			this->sendDataToClient(socket, "100 OK Operating system set");
		}
		break;
	case CmdOrig:
		if (!cm->isAuthenticated) {
			this->sendDataToClient(socket, "101 KO Not authenticated");
			break;
		}
		foreach (parm, cmd.parameters) {
			//TODO
//			emit this->notifyServer(parm);
		}
		this->sendDataToClient(socket, "100 OK");
		break;
	case CmdServices:
		if (!cm->isAuthenticated) {
			this->sendDataToClient(socket, "101 KO Not authenticated");
			break;
		}

		foreach (AstCtiService *service, cm->activeOperator->getServices()->keys()) {
			this->sendDataToClient(socket, QString("200 %1 %2")
										   .arg(service->getName())
										   .arg(service->getContextType() == ServiceTypeInbound ?
													"INBOUND" : "OUTBOUND"));
		}
		this->sendDataToClient(socket, "201 eof");

		break;
	case CmdQueues:
		if (!cm->isAuthenticated) {
			this->sendDataToClient(socket, "101 KO Not authenticated");
			break;
		}

		foreach (AstCtiService *service, cm->activeOperator->getServices()->keys()) {
			if (service->getServiceIsQueue())
				this->sendDataToClient(socket, QString("202 %1")
											   .arg(service->getQueueName()));
		}
		this->sendDataToClient(socket, "203 eof");

		break;
	case CmdPause:
		if (!cm->isAuthenticated) {
			this->sendDataToClient(socket, "101 KO Not authenticated");
			break;
		}
		if (cmd.parameters.count() < 1) {
			this->sendDataToClient(socket, "101 KO No action requested");
			break;
		} else {
			this->sendDataToClient(socket, "301 Pause Pending");
			if (cm->state == StatePaused) {
				cm->state = StatePauseOutRequested;
				this->ctiClientPauseOut(cm);
			} else {
				cm->state = StatePauseInRequested;
				this->ctiClientPauseIn(cm);
			}
		}
		break;
	case CmdNotDefined:
		this->sendDataToClient(socket, "101 KO Invalid Command");
		break;
	} // end switch
}

/*!
	Sends string data over the specified QTcpSocket
*/
void CoreTcpServer::sendDataToClient(QTcpSocket *socket, const QString &data)
{
	if (socket == 0 || socket->state() != QAbstractSocket::ConnectedState)
		return;

	ClientManager *cm = this->clients.value(socket);
	QLOG_DEBUG() << (QString("Client %1 >> %2")
					 .arg(cm->localIdentifier, -32)
					 .arg(data.simplified())).toUtf8().constData();

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	//out.setVersion(QDataStream::Qt_4_5);

	out << (quint16)0;
	if (cm->compressionLevel > 0)
		out << qCompress(data.toUtf8(), cm->compressionLevel);
	else
		out << data;

	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));

	socket->write(block);
}

bool CoreTcpServer::containsUser(const QString &username)
{
	return this->users.contains(username);
}

void CoreTcpServer::addUser(const QString &username)
{
	if (!containsUser(username))
		this->users.append(username);
}

void CoreTcpServer::removeUser(const QString &username)
{
	if (containsUser(username))
		this->users.removeAll(username);
}

void CoreTcpServer::addClient(QTcpSocket *socket, ClientManager *cm)
{
	QLOG_INFO() << "Adding client" << cm->localIdentifier;

	this->clients.insert(socket, cm);

	QLOG_INFO() << "Number of clients:" << this->clients.count();
}

void CoreTcpServer::removeClient(QTcpSocket *socket)
{
	if (this->clients.contains(socket)) {
		ClientManager *cm = this->clients.value(socket);
		QLOG_INFO() << "Removing client" << cm->localIdentifier;

		this->clients.remove(socket);

		int clientCount = clients.count();
		QLOG_INFO() << "Number of clients:" << clientCount;

		if (clientCount == 0)
			this->clients.squeeze();
	} else {
		QLOG_WARN() << "Cannot find client for requested socket.";
	}
}

ClientManager *CoreTcpServer::getClientByExten(const QString &exten)
{
	foreach (ClientManager *cm, this->clients) {
		if (cm->getExtensions().contains(exten))
			return cm;
	}

	return 0;
}

void CoreTcpServer::disconnectClients()
{
	foreach (ClientManager *cm, this->clients) {
		//Disconnection will delete client and remove it from clients hash
		cm->socket->disconnectFromHost();
	}
}

void CoreTcpServer::stopServer()
{
	// TODO : There is currently no way to call this method
	// TODO : A mechanism to close the application must be implemented, maybe using QtService

	this->isClosing = true;

	QLOG_INFO() << "Received STOP signal";

	emit this->serverIsClosing();

	this->stopListening();
    
	CtiServerApplication::instance()->quit();
}

void CoreTcpServer::receiveAsteriskEvent(const AmiEvent &eventId, AstCtiCall *call)
{
	QString logMsg = "";
	if (call != 0)
		logMsg = "for call " + call->getUniqueId();

	switch (eventId) {
	case AmiEventNewchannel:
		{
			QString context = call->getContext();
			if (!context.isEmpty()) {
				AstCtiService *service = this->config->getServiceByName(context);
				if (service != 0) {
					// Here we add reference to service actions.
					// Before the call is passed to the client, the right action
					// will be selected using client's operating system.
					call->setActions(service->getActions());

					// Add relevant variables to call
					foreach (QString varName, *(service->getVariables()))
						call->addVariable(varName, "");
				}
			}
		}
		break;
	case AmiEventBridge:
		{
			if (call != 0) {
				QString callChannel = call->getParsedDestChannel();

				if (callChannel.length() > 0) {
					ClientManager* cm = this->getClientByExten(callChannel);
					if (cm != 0) {
						QString clientOperatingSystem = cm->clientOperatingSystem;
						if (!clientOperatingSystem.isEmpty()) {
							// Here we add information about CTI application to start
							call->setOperatingSystem(clientOperatingSystem);
							QString xmlData = call->toXml();
							this->sendDataToClient(cm->socket, xmlData);
						}
					} else {
						QLOG_WARN() << ">> receiveCtiEvent << Client with exten" << callChannel
									<< "not found in client list";
					}
				} else {
					QLOG_WARN() << ">> receiveCtiEvent << Call Channel is empty";
				}
			}
		}
		break;
	case AmiEventHangup:
		// Call object is owned by another thread so it is unsafe to delete it here.
		// Instead, we invoke QObject's deleteLater() method which will
		// delete the object once the control returns to the event loop.
		call->deleteLater();
		break;
	}

	QLOG_INFO() << "Received CTI Event" << this->amiClient->getEventName(eventId) << logMsg;
}

void CoreTcpServer::processAsteriskResponse(AmiCommand *cmd)
{
	ClientManager* cm = this->getClientByExten(cmd->exten);

	if (cm != 0) {
		QString responseString = cmd->responseString;
        QString responseMessage = cmd->responseMessage;
		if (cmd->action == AmiActionQueuePause) {
            bool result = (responseString.toLower() == "success");
			if (cm->state == StatePauseInRequested) {
				this->pauseInResult(cm, result, responseMessage);
			} else if (cm->state == StatePauseOutRequested) {
				this->pauseOutResult(cm, result, responseMessage);
			}
        } else {
			QLOG_WARN() << ">> processAsteriskResponse << Client with exten" << cmd->exten
						<< "not found in client list";
			this->ctiResponse(cm, this->amiClient->getActionName(cmd->action),
							  responseString, responseMessage);
        }
    }

	// Cmd object is owned by another thread so it is unsafe to delete it here.
	// Instead, we invoke QObject's deleteLater() method which will
	// delete the object once the control returns to the event loop.
	cmd->deleteLater();
}

void CoreTcpServer::ctiClientLogin(ClientManager *cm)
{
	AstCtiSeat *seat = cm->activeSeat;
	AstCtiOperator *op = cm->activeOperator;
	if (seat != 0 && op != 0) {
		QHash<AstCtiService*, int> *services = op->getServices();

		QHashIterator<AstCtiService*, int> i(*services);
		while (i.hasNext()) {
			i.next();
			AstCtiService *service = i.key();
			// Check if we got an existent service and it is a queue
			if (service != 0 && service->getServiceIsQueue()) {
				QString beginInPause = (op->getBeginInPause() ? "true" : "false");
				QString penalty = QString::number(i.value());
				// Get operator interfaces from it's seat
				QStringList interfaces = seat->getExtensions();

				const int listSize = interfaces.size();
				for (int i = 0; i < listSize; i++) {
					AmiCommand *cmd = new AmiCommand;
					cmd->action = AmiActionQueueAdd;
					cmd->exten = interfaces.at(i);
					cmd->parameters = new QHash<QString, QString>;
					cmd->parameters->insert("Queue", service->getQueueName());
					cmd->parameters->insert("Interface", interfaces.at(i));
					cmd->parameters->insert("Penalty", penalty);
					cmd->parameters->insert("Paused", beginInPause);
					emit this->newAmiCommand(cmd);
				}
			}
		}
	}
}

void CoreTcpServer::ctiClientLogoff(ClientManager *cm)
{
	AstCtiSeat *seat = cm->activeSeat;
	AstCtiOperator *op = cm->activeOperator;
	if (seat != 0 && op != 0) {
		QHash<AstCtiService*, int> *services = op->getServices();

		QHashIterator<AstCtiService*, int> i(*services);
		while (i.hasNext()) {
			i.next();
			AstCtiService *service = i.key();
			// Check if we got an existent service and it is a queue
			if (service != 0 && service->getServiceIsQueue()) {
				// Get operator interfaces from it's seat
				QStringList interfaces = seat->getExtensions();

				const int listSize = interfaces.size();
				for (int i = 0; i < listSize; i++) {
					AmiCommand *cmd = new AmiCommand;
					cmd->action = AmiActionQueueRemove;
					cmd->exten = interfaces.at(i);
					cmd->parameters = new QHash<QString, QString>;
					cmd->parameters->insert("Queue", service->getQueueName());
					cmd->parameters->insert("Interface", interfaces.at(i));
					emit this->newAmiCommand(cmd);
				}
			}
		}
	}
}

void CoreTcpServer::ctiClientPauseIn(ClientManager *cm)
{
	AstCtiSeat *seat = cm->activeSeat;
	AstCtiOperator *op = cm->activeOperator;
	if (seat != 0 && op != 0) {
		// Get operator interfaces from it's seat
		QStringList interfaces = seat->getExtensions();

		const int listSize = interfaces.size();
		for (int i = 0; i < listSize; i++) {
			AmiCommand *cmd = new AmiCommand;
			cmd->action = AmiActionQueuePause;
			cmd->exten = interfaces.at(i);
			cmd->parameters = new QHash<QString, QString>;
			cmd->parameters->insert("Interface", interfaces.at(i));
			cmd->parameters->insert("Paused", "true");
			emit this->newAmiCommand(cmd);
		}
	}
}

void CoreTcpServer::ctiClientPauseOut(ClientManager *cm)
{
	AstCtiSeat *seat = cm->activeSeat;
	AstCtiOperator *op = cm->activeOperator;
	if (seat != 0 && op != 0) {
		// Get operator interfaces from it's seat
		QStringList interfaces = seat->getExtensions();

		const int listSize = interfaces.size();
		for (int i = 0; i < listSize; i++) {
			AmiCommand *cmd = new AmiCommand;
			cmd->action = AmiActionQueuePause;
			cmd->exten = interfaces.at(i);
			cmd->parameters = new QHash<QString, QString>;
			cmd->parameters->insert("Interface", interfaces.at(i));
			cmd->parameters->insert("Paused", "false");
			emit this->newAmiCommand(cmd);
		}
	}
}

void CoreTcpServer::pauseInResult(ClientManager *cm, bool result, const QString &reason)
{
	if (result) {
		cm->state = StatePaused;
		this->sendDataToClient(cm->socket, "300 OK");
	} else {
		cm->state = StateLoggedIn;
		this->sendDataToClient(cm->socket, QString("302 KO %1").arg(reason));
	}
}

void CoreTcpServer::pauseOutResult(ClientManager *cm, bool result, const QString &reason)
{
	if (result) {
		cm->state = StateLoggedIn;
		this->sendDataToClient(cm->socket, "300 OK");
	} else {
		cm->state = StatePaused;
		this->sendDataToClient(cm->socket, QString("302 KO %1").arg(reason));
	}
}

void CoreTcpServer::ctiResponse(ClientManager *cm, const QString &actionName,
								const QString &responseString, const QString &responseMessage)
{
	//TODO
	QString strMessage = QString("500 %1,%2,%3")
								 .arg(actionName)
								 .arg(responseString)
								 .arg(responseMessage);
	this->sendDataToClient(cm->socket, strMessage);
}

/*!
	Parse a string command and returns an AstCTICommand struct with
	command and parameters as QStringList;
*/
AstCTICommand CoreTcpServer::parseCommand(const QString &command)
{
	QStringList data = command.split(" ");
	AstCTICommand newCommand;
	newCommand.command = QString(data.at(0)).toUpper();
	data.removeFirst();
	newCommand.parameters = data;
	return newCommand;
}
