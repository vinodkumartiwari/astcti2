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
 * AstCtiServer.  If you copy code from other releases into a copy of GNU
 * AstCtiServer, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCtiServer, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include "QsLog.h"
#include "coretcpserver.h"
#include "ctiserverapplication.h"

/*!
    Our CoreTcpServer derive from QTcpServer
 */
CoreTcpServer::CoreTcpServer(AstCtiConfiguration* const config, QObject* parent)
	: QTcpServer(parent)
{
	QLOG_TRACE() << "Creating CoreTcpServer";

	this->isClosing = false;
	this->amiStatus = AmiConnectionStatusDisconnected;
    this->config = config;

	connect(this, SIGNAL(newConnection()),
			this, SLOT(connectionInitiated()));

	qRegisterMetaType<AstCtiConfiguration*>("AstCtiConfiguration*");
	qRegisterMetaType<AstCtiChannel*>("AstCtiChannel*");
	qRegisterMetaType<AmiCommand*>("AMICommand*");

	this->amiClient = new AmiClient();
	this->amiClient->setParameters(this->config);
	connect(this->amiClient, SIGNAL(amiConnectionStatusChange(AmiConnectionStatus)),
			this, SLOT(handleAmiConnectionStatusChange(AmiConnectionStatus)));
	connect(this->amiClient, SIGNAL(amiChannelEvent(AmiEvent, AstCtiChannel*)),
			this, SLOT(handleAmiChannelEvent(AmiEvent, AstCtiChannel*)));
	connect(this->amiClient, SIGNAL(amiStatusEvent(AmiEvent, QString, QString, int)),
			this, SLOT(handleAmiStatusEvent(AmiEvent, QString, QString, int)));
	connect(this->amiClient, SIGNAL(amiCommandFailed(AmiAction, QString)),
			this, SLOT(handleAmiCommandFailed(AmiAction, QString)));
	connect(this, SIGNAL(newAmiCommand(AmiCommand*)),
			this->amiClient, SLOT(sendCommandToAsterisk(AmiCommand*)));
	connect(this, SIGNAL(newAmiConfiguration(AstCtiConfiguration*)),
			this->amiClient, SLOT(setParameters(AstCtiConfiguration*)));

	QThread* amiThread = new QThread(this);
	this->amiClient->moveToThread(amiThread);
	connect(amiThread, SIGNAL(started()),
			this->amiClient, SLOT(run()));
	connect(amiThread, SIGNAL(finished()),
			this->amiClient, SLOT(deleteLater()));
	amiThread->start();
}

CoreTcpServer::~CoreTcpServer()
{
	QLOG_TRACE() << "Destroying CoreTcpServer";

	this->amiClient->stop();
	// amiClient will be deleted automatically because the thread's
	// finished() signal is connected to amiClient's deleteLater() slot
	QThread* amiThread = this->amiClient->thread();
	amiThread->quit();
	amiThread->wait();
	delete amiThread;

	//Closing AMI connection will disconnect all CTI clients

	delete this->config;
}

void CoreTcpServer::setConfig(AstCtiConfiguration* const newConfig)
{
	this->stopListening();

	AstCtiConfiguration* tmpConfig = this->config;

	bool amiParamsChanged = newConfig->amiHost != tmpConfig->amiHost ||
							newConfig->amiPort != tmpConfig->amiPort ||
							newConfig->amiUser != tmpConfig->amiUser ||
							newConfig->amiSecret != tmpConfig->amiSecret ||
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
			foreach (ClientManager* cm, this->clients) {
				bool disconnected = false;
				AstCtiOperator* const activeOp = cm->getActiveOperator();
				if (activeOp != 0) {
					QString username = activeOp->getUsername();
					AstCtiOperator* op = newConfig->operators.value(username);
					if (op == 0) {
						//Operator does not exist in new config
						//Disconnection will delete client and remove it from clients hash
						disconnected = true;
					} else {
						if (activeOp->getPassword() != op->getPassword() ||
							activeOp->getSeatId() != op->getSeatId() ||
							!activeOp->compareServices(op->getServices())) {
							//Critical operator parameters have changed
							disconnected = true;
						} else {
							//Critical operator parameters are unchanged in new config
							cm->setActiveOperator(op);
						}
					}
				}

				if (!disconnected) {
					const AstCtiSeat* const activeSeat = cm->getActiveSeat();
					if (activeSeat != 0) {
						int id = activeSeat->getId();
						AstCtiSeat* seat = newConfig->seats.value(id);
						if (seat == 0) {
							//Seat does not exist in new config
							//Disconnection will delete client and remove it from clients hash
							disconnected = true;
						} else {
							if (activeSeat->getMac() != seat->getMac() ||
								!activeSeat->compareExtensions(seat->getExtensions())) {
								//Critical seat parameters have changed
								disconnected = true;
							} else {
								//Seat is unchanged in new config
								cm->setActiveSeat(seat);
							}
						}
					}
				}

				if (disconnected) {
					// Send disconnect message and close connection
					QString xmlString;
					QXmlStreamWriter writer(&xmlString);

					writer.writeStartDocument();
					writer.writeTextElement(QStringLiteral("Disconnect"), QStringLiteral(""));
					writer.writeEndDocument();

					this->sendDataToClient(cm->getSocket(), xmlString);

					cm->getSocket()->disconnectFromHost();
				} else {
					//Send new operator configuration to client
					this->sendDataToClient(cm->getSocket(),
										   cm->getActiveOperator()->toXml(cm->getActiveSeat()));
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
						<< QString("%1:%2").arg(this->config->ctiServerAddress)
										   .arg(this->config->ctiServerPort);
		else
			QLOG_ERROR() << "CoreTcpServer failed listening on socket"
						 << QString("%1:%2").arg(this->config->ctiServerAddress)
											.arg(this->config->ctiServerPort)
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
					<< QString("%1:%2").arg(this->config->ctiServerAddress)
									   .arg(this->config->ctiServerPort);
	}
}

/*!
	Called when a new connection is avaiable.
 */
void CoreTcpServer::connectionInitiated()
{
	QTcpSocket* newSocket = this->nextPendingConnection();

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

		ClientManager* cm = new ClientManager(newSocket);
		cm->setLocalIdentifier(QString("%1:%2").arg(remoteAddr.toString()).arg(remotePort));
		cm->setIdleTimeout(config->ctiReadTimeout);

		this->addClient(cm);

		// Let's say Welcome to our client!
		this->sendDataToClient(newSocket, QString(RSP_OK" WELCOME %1")
										  .arg(this->config->ctiCompressionLevel));

		// Welcome message is always sent uncompressed, further communication
		// will be compressed if so defined in config
		cm->setCompressionLevel(this->config->ctiCompressionLevel);
	}
}

void CoreTcpServer::socketDataReceived()
{
	//Retrieve sender QTcpSocket
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(QObject::sender());

	ClientManager* cm = this->clients.value(socket);

	cm->resetIdleTimer();

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
			if (cm->getCompressionLevel() > 0) {
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
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(QObject::sender());

	ClientManager* cm = this->clients.value(socket);

	if (cm->getIsAuthenticated() && this->amiStatus == AmiConnectionStatusConnected) {
		// We should do a logoff right now
		this->agentLogout(cm);
	}
	this->removeUser(cm->getCtiUsername());

	this->removeClient(cm);
	delete cm;
	socket->deleteLater();
}

void CoreTcpServer::socketError(QAbstractSocket::SocketError error)
{
	//Retrieve sender QTcpSocket
	QTcpSocket* socket = qobject_cast<QTcpSocket*>(QObject::sender());

	ClientManager* cm = this->clients.value(socket);

	const QString iden = cm == 0 ? QStringLiteral("") : cm->getLocalIdentifier();
	QLOG_ERROR() << "Client Socket Error:" << error << socket->errorString()
				 << "for client" << iden;
}

void CoreTcpServer::processClientData(QTcpSocket* const socket, const QString& data)
{
	ClientManager* cm = this->clients.value(socket);

	QLOG_DEBUG() << (QString("Client %1 << %2")
					 .arg(cm->getLocalIdentifier(), -32)
					 .arg(data.simplified())).toUtf8().constData();

	AstCtiCommand cmd = this->parseCommand(data);

	switch(cmd.command) {
	case CmdNotDefined:
		// Unrecognized command was received
		this->sendDataToClient(socket, QString(RSP_ERR" "ERR_UNKNOWN_CMD));
		return;
	case CmdLogin:
		if (cmd.parameters.count() != 4) {
			this->sendDataToClient(socket, QString(RSP_ERR" "ERR_WRONG_PARAM));
			break;
		} else {
			bool success = true;

			cm->setCtiUsername(cmd.parameters.at(0));

			AstCtiOperator* ctiOperator = config->operators.value(cm->getCtiUsername());
			if (ctiOperator != 0) {
				// Avoid duplicate logins
				if (this->containsUser(cm->getCtiUsername())) {
					success = false;
					cm->setCtiUsername(QStringLiteral(""));
					this->sendDataToClient(socket, QString(RSP_ERR" "ERR_USER_LOGGED_IN));
				}
			} else {
				success = false;
				cm->setCtiUsername(QStringLiteral(""));
				this->sendDataToClient(socket, QString(RSP_ERR" "ERR_WRONG_CREDS));
			}

			if (success) {
				if (!ctiOperator->checkPassword(cmd.parameters.at(1))) {
					success = false;
					cm->setCtiUsername(QStringLiteral(""));
					this->sendDataToClient(socket, QString(RSP_ERR" "ERR_WRONG_CREDS));
				}
			}

			if (success) {
				const QString osTypeString = cmd.parameters.at(2);
				if (AstCtiAction::parseOsType(osTypeString) == ActionOsAll) {
					success = false;
					cm->setCtiUsername(QStringLiteral(""));
					this->sendDataToClient(socket, QString(RSP_ERR" "ERR_UNKNOWN_OS));
				}
				cm->setClientOperatingSystem(osTypeString);
			}

			AstCtiSeat* seat = 0;
			if (success) {
				if (ctiOperator->isCallCenter()) {
					// Call-center operators must send their MAC addres to identify their seat
					QString mac = cmd.parameters.at(3).toLower();
					seat = config->getSeatByMac(mac);
					if (seat == 0) {
						success = false;
						cm->setCtiUsername(QStringLiteral(""));
						this->sendDataToClient(socket, QString(RSP_ERR" "ERR_WRONG_MAC));
					}
				} else {
					// Non-call-center operators already have a seat defined
					seat = config->seats.value(ctiOperator->getSeatId());
				}

				if (success) {
					const int listSize = this->clients.size();
					for (int i = 0; i < listSize; i++) {
						if (this->clients.values().at(i)->getActiveSeat() == seat) {
							success = false;
							cm->setCtiUsername(QStringLiteral(""));
							this->sendDataToClient(socket, QString(RSP_ERR" "ERR_SEAT_TAKEN));
						}
					}
				}
			}

			if (success) {
				cm->setIsAuthenticated(true);
				cm->setActiveOperator(ctiOperator);
				cm->setActiveSeat(seat);
				cm->setLocalIdentifier(cm->getLocalIdentifier() +
									   QString(" (%1)").arg(seat->getExtensionNumbers().join(" ")));
				this->addUser(cm->getCtiUsername());

				this->sendDataToClient(socket, QString(RSP_OK" %1").arg(config->ctiReadTimeout));
				// Login is successful, so we send operator data to client
				// After receiving this, client will display it's main window
				this->sendDataToClient(socket, cm->getActiveOperator()->toXml(cm->getActiveSeat()));
			} else {
				socket->close();
			}
		}
		break;
	case CmdChangePassword:
		if (!cm->getIsAuthenticated()) {
			this->sendDataToClient(socket, QString(RSP_ERR" "ERR_NO_AUTH));
		} else {
			if (cmd.parameters.count() < 2) {
				this->sendDataToClient(socket, QString(RSP_ERR" "ERR_WRONG_PARAM));
			} else {
				const QString oldPassword = cmd.parameters.at(0);
				const QString newPassword = cmd.parameters.at(1);

				if (cm->getActiveOperator()->checkPassword(oldPassword) &&
					cm->getActiveOperator()->changePassword(newPassword)) {
					this->sendDataToClient(socket, QStringLiteral(RSP_OK));
				} else {
					this->sendDataToClient(socket, QString(RSP_ERR" "ERR_PASS_CHG_FAIL));
				}
			}
		}
		break;
	case CmdKeepAlive:
		this->sendDataToClient(socket, QString(RSP_OK" %1").arg(config->ctiReadTimeout));
		break;
	case CmdOriginate:
		if (!cm->getIsAuthenticated()) {
			this->sendDataToClient(socket, QString(RSP_ERR" "ERR_NO_AUTH));
		} else {
			if (cmd.parameters.count() < 2) {
				this->sendDataToClient(socket, QString(RSP_ERR" "ERR_WRONG_PARAM));
			} else {
				const QString channelName = cmd.parameters.at(0);
				if (!cm->getActiveSeat()->hasExtension(channelName)) {
					this->sendDataToClient(socket, QString(RSP_ERR" "ERR_UNKNOWN_CHAN));
				} else {
					//TODO
					this->sendDataToClient(socket, QStringLiteral(RSP_OK));
				}
			}
		}
		break;
	case CmdConference:
		if (!cm->getIsAuthenticated()) {
			this->sendDataToClient(socket, QString(RSP_ERR" "ERR_NO_AUTH));
		} else {
			if (cmd.parameters.count() < 2) {
				this->sendDataToClient(socket, QString(RSP_ERR" "ERR_WRONG_PARAM));
			} else {
				const QString channelName = cmd.parameters.at(0);
				if (!cm->getActiveSeat()->hasExtension(channelName)) {
					this->sendDataToClient(socket, QString(RSP_ERR" "ERR_UNKNOWN_CHAN));
				} else {
					//TODO
					this->sendDataToClient(socket, QStringLiteral(RSP_OK));
				}
			}
		}
		break;
	case CmdStart:
		if (!cm->getIsAuthenticated()) {
			this->sendDataToClient(socket, QString(RSP_ERR" "ERR_NO_AUTH));
		} else {
			if (cmd.parameters.count() < 2) {
				this->sendDataToClient(socket, QString(RSP_ERR" "ERR_WRONG_PARAM));
			} else {
				const QString channelName = cmd.parameters.at(0);
				if (!cm->getActiveSeat()->hasExtension(channelName)) {
					this->sendDataToClient(socket, QString(RSP_ERR" "ERR_UNKNOWN_CHAN));
					return;
				}
				const QString queue = cmd.parameters.at(1);
				if (!cm->getActiveSeat()->hasQueue(channelName, queue)) {
					this->sendDataToClient(socket, QString(RSP_ERR" "ERR_UNKNOWN_QUEUE));
					return;
				}
				this->sendDataToClient(socket, QStringLiteral(RSP_OK));
				this->agentLogin(cm, channelName, queue);
			}
		}
		break;
	case CmdPause:
		if (!cm->getIsAuthenticated()) {
			this->sendDataToClient(socket, QString(RSP_ERR" "ERR_NO_AUTH));
		} else {
			if (cmd.parameters.count() < 2) {
				this->sendDataToClient(socket, QString(RSP_ERR" "ERR_WRONG_PARAM));
			} else {
				const QString channelName = cmd.parameters.at(0);
				if (!cm->getActiveSeat()->hasExtension(channelName)) {
					this->sendDataToClient(socket, QString(RSP_ERR" "ERR_UNKNOWN_CHAN));
					return;
				}
				const QString queue = cmd.parameters.at(1);
				if (!cm->getActiveSeat()->hasQueue(channelName, queue)) {
					this->sendDataToClient(socket, QString(RSP_ERR" "ERR_UNKNOWN_QUEUE));
					return;
				}
				this->sendDataToClient(socket, QStringLiteral(RSP_OK));
				this->agentPause(cm, channelName, queue);
			}
		}
		break;
	case CmdQuit:
		this->sendDataToClient(socket, QStringLiteral(RSP_OK));
		socket->flush();
		socket->disconnectFromHost();
		break;
	} // end switch
}

/*!
	Sends string data over the specified QTcpSocket
 */
void CoreTcpServer::sendDataToClient(QTcpSocket* const socket, const QString& data)
{
	if (socket == 0 || socket->state() != QAbstractSocket::ConnectedState)
		return;

	ClientManager* cm = this->clients.value(socket);
	QLOG_DEBUG() << (QString("Client %1 >> %2")
					 .arg(cm->getLocalIdentifier(), -32)
					 .arg(data.simplified())).toUtf8().constData();

	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	//out.setVersion(QDataStream::Qt_4_5);

	out << (quint16)0;
	if (cm->getCompressionLevel() > 0)
		out << qCompress(data.toUtf8(), cm->getCompressionLevel());
	else
		out << data;

	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));

	socket->write(block);

	cm->resetIdleTimer();
}

bool CoreTcpServer::containsUser(const QString& username)
{
	return this->users.contains(username);
}

void CoreTcpServer::addUser(const QString& username)
{
	if (!this->containsUser(username))
		this->users.append(username);
}

void CoreTcpServer::removeUser(const QString& username)
{
	if (this->containsUser(username))
		this->users.removeAll(username);
}

void CoreTcpServer::addClient(ClientManager* const cm)
{
	QLOG_INFO() << "Adding client" << cm->getLocalIdentifier();

	this->clients.insert(cm->getSocket(), cm);

	QLOG_INFO() << "Number of clients:" << this->clients.count();
}

void CoreTcpServer::removeClient(ClientManager* const cm)
{
	if (cm != 0 && this->clients.contains(cm->getSocket())) {
		QLOG_INFO() << "Removing client" << cm->getLocalIdentifier();

		this->clients.remove(cm->getSocket());

		int clientCount = clients.count();
		QLOG_INFO() << "Number of clients:" << clientCount;

		if (clientCount == 0)
			this->clients.squeeze();
	} else {
		QLOG_WARN() << "Cannot find requested client to remove.";
	}
}

ClientManager* CoreTcpServer::getClientByChannelName(const QString& channelName)
{
	if (channelName.isEmpty())
		return 0;

	foreach (ClientManager* cm, this->clients)
		if (cm->getActiveSeat() != 0 && cm->getActiveSeat()->hasExtension(channelName))
			return cm;

	return 0;
}

void CoreTcpServer::disconnectClients()
{
	foreach (ClientManager* cm, this->clients) {
		//Disconnection will delete client and remove it from clients hash
		cm->getSocket()->disconnectFromHost();
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

void CoreTcpServer::handleAmiConnectionStatusChange(AmiConnectionStatus status)
{
	this->amiStatus = status;
	switch(status) {
	case AmiConnectionStatusConnected:
		/* CODE TESTING START */
		//this->stopServer();
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

void CoreTcpServer::handleAmiChannelEvent(AmiEvent eventId, AstCtiChannel* channel)
{
	QLOG_INFO() << "Received AMI channel event" << AmiClient::getEventName(eventId)
				<< "for channel" << channel->getChannelId();

	const QString exten = channel->getChannelId();
	foreach (ClientManager* cm, this->clients) {
		if (cm->getActiveSeat() != 0 && cm->getActiveSeat()->hasExtension(exten)) {
			switch (eventId) {
			case AmiEventNewchannel:
			{
				const QString context = channel->getContext();
				AstCtiService* service = this->config->getServiceByName(context);
				if (service != 0) {
					// Here we add reference to service actions.
					// Before the call is passed to the client, the right action
					// will be selected using client's operating system.
					channel->setActions(service->getActions());

					// Add relevant variables to call
					foreach (QString varName, service->getVariables())
						channel->addVariable(varName, "");
				}
			}
				break;
			case AmiEventBridge:
				// Here we add information about client operating system
				// so that only relevant actions are sent to CTI client
				channel->setOperatingSystem(cm->getClientOperatingSystem());
				break;
			}
			this->sendDataToClient(cm->getSocket(),
								   channel->toXml(AmiClient::getEventName(eventId)));
		} else if (cm->getActiveOperator() != 0 && cm->getActiveOperator()->getCanMonitor()) {
			// Operator monitors all system calls
			this->sendDataToClient(cm->getSocket(),
								   channel->toXml(AmiClient::getEventName(eventId)));
		}
	}

	// Channel object is owned by another thread so it is unsafe to delete it here.
	// Instead, we invoke QObject's deleteLater() method which will
	// delete the object once the control returns to the event loop.
	if (eventId == AmiEventHangup)
		channel->deleteLater();
}

void CoreTcpServer::handleAmiStatusEvent(AmiEvent eventId, QString channelName,
										 QString data, int status)
{
	AstCtiExtensionStatus extensionStatus;
	AstCtiAgentStatus agentStatus;

	if (eventId == AmiEventAgentStatus)
		agentStatus = (AstCtiAgentStatus)status;
	else
		extensionStatus = (AstCtiExtensionStatus)status;

	QLOG_INFO() << "Received AMI status event" << AmiClient::getEventName(eventId)
				<< "for object" << channelName << "with status"
				<< (eventId == AmiEventAgentStatus ?
					AmiClient::agentStatusToString(agentStatus) :
					AmiClient::extensionStatusToString(extensionStatus));

	ClientManager* cm = 0;
	cm = this->getClientByChannelName(channelName);
	if (cm == 0) {
		QLOG_WARN() << "Process AMI Status event: Client with channel" << channelName
					<< "not found in client list";
		return;
	}

	QString xmlString;
	QXmlStreamWriter writer(&xmlString);

	switch (eventId) {
	case AmiEventExtensionStatus:
		cm->getActiveSeat()->setExtensionStatus(channelName, extensionStatus);

		// Create XML status object
		writer.writeStartDocument();
		writer.writeStartElement(QStringLiteral("ExtensionStatus"));
		writer.writeAttribute(QStringLiteral("ChannelName"), channelName);
		writer.writeCharacters(QString::number((int)extensionStatus));
		writer.writeEndElement(); // ExtensionStatus
		writer.writeEndDocument();

		this->sendDataToClient(cm->getSocket(), xmlString);
		break;
	case AmiEventAgentStatus:
		if (agentStatus != AgentStatusLoginFailed && agentStatus != AgentStatusPauseFailed)
			cm->getActiveSeat()->setAgentStatus(channelName, data, agentStatus);

		// Create XML status object
		writer.writeStartDocument();
		writer.writeStartElement(QStringLiteral("AgentStatus"));
		writer.writeAttribute(QStringLiteral("ChannelName"), channelName);
		writer.writeAttribute(QStringLiteral("Queue"), data);
		writer.writeCharacters(QString::number((int)agentStatus));
		writer.writeEndElement(); // AgentStatus
		writer.writeEndDocument();

		this->sendDataToClient(cm->getSocket(), xmlString);
		break;
	}
}

void CoreTcpServer::handleAmiCommandFailed(AmiAction action, QString channelName)
{
	// TODO
}

void CoreTcpServer::agentLogin(ClientManager * const cm, const QString &channelName,
							   const QString &queue)
{
	AstCtiSeat* seat = cm->getActiveSeat();
	AstCtiOperator* op = cm->getActiveOperator();
	if (seat != 0 && op != 0) {
		QString beginInPause =
				op->getBeginInPause() ? QStringLiteral("true") : QStringLiteral("false");
		QString penalty = QString::number(this->config->getPenaltyByQueueName(queue));

		AmiCommand* cmd = new AmiCommand(AmiActionQueueAdd);
		cmd->channelName = channelName;
		cmd->addParameter(QStringLiteral("Queue"), queue);
		cmd->addParameter(QStringLiteral("Interface"), channelName);
		cmd->addParameter(QStringLiteral("Penalty"), penalty);
		cmd->addParameter(QStringLiteral("Paused"), beginInPause);
		this->sendAmiCommand(cmd);
	}
}

void CoreTcpServer::agentLogout(ClientManager* const cm)
{
	// We always log agents out of all queues
	AstCtiSeat* seat = cm->getActiveSeat();
	AstCtiOperator* op = cm->getActiveOperator();
	if (seat != 0 && op != 0) {
		AstCtiServiceRevHash services = op->getServices();

		for (AstCtiServiceRevHash::const_iterator i = services.constBegin();
			 i != services.constEnd();
			 i++) {
			AstCtiService* service = i.key();
			// Check if we got an existent service and it is a queue
			if (service != 0 && service->isQueue()) {
				// Get operator interfaces from it's seat
				AstCtiExtensionList interfaces = seat->getExtensions();

				const int listSize = interfaces.size();
				for (int i = 0; i < listSize; i++) {
					const QString channelName = interfaces.at(i)->channelName;
					const QString queue = service->getQueueName();
					const AstCtiAgentStatus agentStatus = seat->getAgentStatus(channelName, queue);
					if (agentStatus != AgentStatusLoggedOut) {
						AmiCommand* cmd = new AmiCommand(AmiActionQueueRemove);
						cmd->channelName = channelName;
						cmd->addParameter(QStringLiteral("Queue"), queue);
						cmd->addParameter(QStringLiteral("Interface"), channelName);
						this->sendAmiCommand(cmd);
					}
				}
			}
		}
	}
}

void CoreTcpServer::agentPause(ClientManager* const cm, const QString& channelName,
							   const QString& queue)
{
	AstCtiSeat* seat = cm->getActiveSeat();
	AstCtiOperator* op = cm->getActiveOperator();
	if (seat != 0 && op != 0) {
		const AstCtiAgentStatus agentStatus = seat->getAgentStatus(channelName, queue);
		const QString pause = agentStatus == AgentStatusPaused ?
								  QStringLiteral("false") :
								  QStringLiteral("true");
		AmiCommand* cmd = new AmiCommand(AmiActionQueuePause);
		cmd->channelName = channelName;
		cmd->addParameter(QStringLiteral("Queue"), queue);
		cmd->addParameter(QStringLiteral("Interface"), channelName);
		cmd->addParameter(QStringLiteral("Paused"), pause);
		this->sendAmiCommand(cmd);
	}
}

void CoreTcpServer::sendAmiCommand(AmiCommand* const cmd)
{
	// Move command object to the AmiClient's thread because we don't need to handle it any more
	// It will be deleted by AMiClient once it is procesed
	cmd->moveToThread(this->amiClient->thread());
	emit this->newAmiCommand(cmd);
}

/*!
	Parse a string command and returns an AstCtiCommand struct with
	command and parameters as QStringList;
 */
AstCtiCommand CoreTcpServer::parseCommand(const QString& command)
{
	QStringList data = command.split(" ");
	AstCtiCommand newCommand;
	newCommand.command = this->parseCommandType(data.at(0).toUpper());
	if (newCommand.command == CmdNotDefined) {
		QLOG_ERROR() << "Received unrecognized command:" << newCommand.command;
	} else {
		data.removeFirst();
		newCommand.parameters = data;
	}
	return newCommand;
}

AstCtiCommands CoreTcpServer::parseCommandType(const QString &commandName)
{
	if (commandName == QStringLiteral("LOGIN"))
		return CmdLogin;
	else if (commandName == QStringLiteral("NOOP"))
		return CmdKeepAlive;
	else if (commandName == QStringLiteral("CHPW"))
		return CmdChangePassword;
	else if (commandName == QStringLiteral("ORIG"))
		return CmdOriginate;
	else if (commandName == QStringLiteral("CONF"))
		return CmdConference;
	else if (commandName == QStringLiteral("START"))
		return CmdStart;
	else if (commandName == QStringLiteral("PAUSE"))
		return CmdPause;
	else if (commandName == QStringLiteral("QUIT"))
		return CmdQuit;
	else
		return CmdNotDefined;
}
