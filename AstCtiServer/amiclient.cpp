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
#include "amiclient.h"
#include "ctiserverapplication.h"

//Q_DECLARE_METATYPE(QAbstractSocket::SocketState)
//Q_DECLARE_METATYPE(QAbstractSocket::SocketError)

AmiClient::AmiClient() : QObject()
{
	qRegisterMetaType<AmiAction>("AmiAction");
	qRegisterMetaType<AmiEvent>("AmiEvent");
	qRegisterMetaType<AmiConnectionStatus>("AmiConnectionStatus");
	qRegisterMetaType<AmiClientStatus>("AmiClientStatus");

	QLOG_TRACE() << "Creating AmiClient";

	this->currentActionId = 0;
    this->localSocket = 0;
	this->isRunning = false;
	this->amiClientStatus = AmiStatusLoggedOff;
}

AmiClient::~AmiClient()
{
	QLOG_TRACE() << "Destroying AmiClient";

	this->isRunning = false;
	if (this->localSocket->state() == QAbstractSocket::ConnectedState) {
		this->performLogoff();
	}
	qDeleteAll(this->activeCalls);
	qDeleteAll(this->pendingAmiCommands);
	delete this->localSocket;
}

void AmiClient::buildSocket()
{
	// Builds socket
    this->localSocket = new QTcpSocket();

	//qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState" );
	//qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError" );
	connect(this->localSocket, SIGNAL(disconnected()),
			this, SLOT(socketDisconnected()));
	connect(this->localSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
			this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
	connect(this->localSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(socketError(QAbstractSocket::SocketError)));
}

void AmiClient::run()
{
	this->isRunning = true;

	// We must allocate heap objects here, because if we do it in a constructor,
	// allocation would be performed on the main thread,
	// meaning that the newly created objects are then owned by the main thread
	// and not the new thread managed by QThread
	if (this->localSocket == 0) {
		// Thread could theoretically be started more than once,
		// so we allocate objects only once
		this->buildSocket();
	}

	while (this->isRunning) {
		this->localSocket->connectToHost(this->amiHost, this->amiPort);

		if (!this->localSocket->waitForConnected(this->amiConnectTimeout) ) {
			this->amiClientStatus = AmiStatusLoggedOff;

			QLOG_WARN() << "Unable to connect to"
						<< QString("%1:%2.").arg(this->amiHost).arg(this->amiPort)
						<< "AmiClient will retry in" << this->amiConnectRetryAfter << "seconds.";
			this->delay(this->amiConnectRetryAfter);
		} else {
			while (this->isRunning) {
				//http://qt.git orious.org/qt/miniak/blobs/6a994abef70012e2c0aa3d70253ef4b9985b2f20/
				//src/corelib/kernel/qeventdispatcher_win.cpp
				while (this->localSocket->waitForReadyRead(this->amiReadTimeout)) {
                    QByteArray dataFromSocket = localSocket->readAll();
                    QString receivedData = QString(dataFromSocket);
                    if (receivedData.trimmed().length() > 0) {
						if (this->amiClientStatus != AmiStatusLoggedOff) {
							this->dataBuffer.append(receivedData);
							if (this->dataBuffer.contains("\r\n\r\n"))
								this->parseDataReceivedFromAsterisk();
						} else {
							if (receivedData.contains("Asterisk Call Manager", Qt::CaseInsensitive))
								this->performLogin();
						}
                    }
                }
                if (this->localSocket->state() != QAbstractSocket::ConnectedState) {
					this->delay(this->amiConnectRetryAfter);
					break;
                }
            }
        }
    }
}

void AmiClient::stop()
{
	// Even if this function is called from another thread,
	// mutex should not be necessary as it is bool variable, and its value
	// should be changed atomically, and even if the write itself is not atomic
	// and the variable can have an inconsistent value, it can be either
	// false or true, and it will eventually be read as false
	// (since the only value that is written there is false)
	this->isRunning = false;
}

void AmiClient::setParameters(AstCtiConfiguration *config)
{
	if (this->isRunning) {
		if (this->amiHost != config->amiHost || this->amiPort != config->amiPort ||
			this->amiUser != config->amiUser || this->amiSecret != config->amiSecret) {

			this->performLogoff();
			this->localSocket->disconnectFromHost();
			qDeleteAll(this->pendingAmiCommands);
			this->pendingAmiCommands.clear();
		}
	}

	this->amiHost = config->amiHost;
	this->amiPort = config->amiPort;
	this->amiUser = config->amiUser;
	this->amiSecret = config->amiSecret;
	this->amiConnectTimeout = config->amiConnectTimeout;
	this->amiReadTimeout = config->amiReadTimeout;
	this->amiConnectRetryAfter = config->amiConnectRetryAfter;
}

void AmiClient::socketStateChanged(QAbstractSocket::SocketState socketState)
{
	QLOG_TRACE() << "AmiClient socket state changed:" << this->socketStateToString(socketState);
}

void AmiClient::socketError(QAbstractSocket::SocketError error)
{    
	if (error != QAbstractSocket::SocketTimeoutError) {
		QLOG_ERROR() << "AmiClient Socket Error:" << error << this->localSocket->errorString();
//		this->amiClientStatus = AmiStatusLoggedOff;
//		emit this->amiConnectionStatusChange(AmiConnectionStatusDisconnected);
	}
}

void AmiClient::socketDisconnected()
{
	this->amiClientStatus = AmiStatusLoggedOff;
    emit this->amiConnectionStatusChange(AmiConnectionStatusDisconnected);
	QLOG_WARN() << "AmiClient Socket Disconnected";
}

void AmiClient::performLogin()
{
	this->amiClientStatus = AmiStatusLoggingIn;

	AmiCommand *cmd = new AmiCommand(AmiActionLogin);
	cmd->parameters = new QHash<QString, QString>;
	cmd->parameters->insert("Username", this->amiUser);
	cmd->parameters->insert("Secret", this->amiSecret);
	this->sendCommandToAsterisk(cmd);
}

void AmiClient::performLogoff()
{
	AmiCommand *cmd = new AmiCommand(AmiActionLogoff);
	this->sendCommandToAsterisk(cmd);
}

void AmiClient::parseDataReceivedFromAsterisk()
{
	//Did we receive a complete set of responses
	bool partialData = !this->dataBuffer.endsWith("\r\n\r\n");

	// here we can begin to evaluate AMI responses and events
	QStringList amiMessages = this->dataBuffer.split("\r\n\r\n");
	if (partialData) {
		//We return incomplete portion of the data to the buffer
		this->dataBuffer = amiMessages.last();
		amiMessages.removeLast();
	} else {
		this->dataBuffer.clear();
	}

	// we need to check if the buffer contains more than
	// one messsage and parse them all in the right order
	foreach (const QString &m, amiMessages) {
		if (m.length() > 0) {
			QHash<QString, QString>* message = this->hashFromMessage(m);
			// We should have at least one Event or response key in the hash!
			if (message->contains("Event"))
				this->evaluateEvent(message);
			else if (message->contains("Response"))
				this->evaluateResponse(message);
			else
				QLOG_WARN() << "Received unrecognized message from Asterisk" << message;

			delete message;
		}
	}
}

QHash<QString, QString>* AmiClient::hashFromMessage(QString data)
{
    QHash<QString, QString>* hash = new QHash<QString, QString>;
    QStringList lines = data.split('\n');
	foreach (const QString &line, lines) {
        if (line.contains(':')) {
			QStringList keyValue = line.split(':');
            if (keyValue.length() > 1) {
                QString key     = keyValue.at(0);
                QString value   = "";
				for (int i = 1; i < keyValue.length(); i++) {
                    value.append(QString(keyValue.at(i)).trimmed());
					if (i < keyValue.length() - 1)
						value.append(":");
                }
				hash->insert(key.trimmed(), value.trimmed());
            }
        }
    }
    return hash;
}

void AmiClient::evaluateEvent(QHash<QString, QString> *event)
{
	QLOG_DEBUG() << "Received Asterisk event" << this->eventToString(event);

	QString eventName = event->value("Event");

	if (eventName == "FullyBooted") {
		// Asterisk is fully booted now, so we can interact with it freely
		// We don't consider to be logged in until we receive this event
		if (this->amiClientStatus == AmiStatusLoggingIn) {
			// In rare cases, this event can be received twice,
			// so we change status only once
			this->amiClientStatus = AmiStatusLoggedIn;
			emit this->amiConnectionStatusChange(AmiConnectionStatusConnected);
		}
	} else if (eventName == "Shutdown") {
		QLOG_WARN() << "Asterisk shutting down. Connection lost.";

		qDeleteAll(this->activeCalls);
		this->activeCalls.clear();
		qDeleteAll(this->pendingAmiCommands);
		this->pendingAmiCommands.clear();
		this->localSocket->close();
		this->amiClientStatus = AmiStatusLoggedOff;

		emit this->asteriskEvent(AmiEventShutdown, 0);
	} else if (eventName == "Newchannel") {
		// Build a new asterisk call object and add it to hashtable
		QString uniqueId = event->value("Uniqueid");
		AstCtiCall *newCall = new AstCtiCall(uniqueId);
        newCall->setChannel(event->value("Channel"));
        newCall->setCalleridNum(event->value("CallerIDNum"));
        newCall->setCalleridName(event->value("CallerIDName"));
        newCall->setState(event->value("ChannelStateDesc"));
		newCall->setExten(event->value("Exten"));
		newCall->setAccountCode(event->value("AccountCode"));

		this->activeCalls.insert(uniqueId, newCall);

		QLOG_DEBUG() << "Newchannel" << event->value("Channel")
					 << "ID" << uniqueId
					 << "CallerID" << event->value("CallerIDNum")
					 << "in state:" << event->value("ChannelStateDesc");

        // TODO: Add destionation and call
		emit this->asteriskEvent(AmiEventNewchannel, newCall);
	} else if (eventName == "Hangup") {
		QString uniqueId = event->value("Uniqueid");
		if (this->activeCalls.contains(uniqueId)) {
			AstCtiCall *newCall = this->activeCalls.value(uniqueId);
            if (newCall != 0) {
				//Receiving slot is responsible for deleting the call object
				emit this->asteriskEvent(AmiEventHangup, newCall);
            }
			// Delete the call from the hashtable
			this->activeCalls.remove(uniqueId);
        }
		QLOG_DEBUG() << "Hangup: " << event->value("Channel")
					 << "ID" << uniqueId
					 << "CallerID" << event->value("CallerIDNum");
	} else if (eventName == "Newexten") {
        QString context = event->value("Context");
		QString uniqueId = event->value("Uniqueid");
		if (this->activeCalls.contains(uniqueId)) {
			AstCtiCall *newCall = this->activeCalls.value(uniqueId);
			if (newCall != 0 && newCall->getContext() != context) {
				newCall->setContext(context);
				QLOG_INFO() << "Call context for" << uniqueId << "is now" << context;
				// TODO: Add destionation and call
				emit this->asteriskEvent(AmiEventNewexten, newCall);
            }
        }
	} else if (eventName == "VarSet") {
		QString uniqueId = event->value("Uniqueid");
		if (this->activeCalls.contains(uniqueId) ) {
			AstCtiCall *newCall = this->activeCalls.value(uniqueId);
            if (newCall != 0) {
				QString variable = event->value("Variable");
				QString value = event->value("Value");

				//Variable will be set only if it exists in call object
				newCall->setVariable(variable, value);

				QLOG_DEBUG() << "Reading variable" << variable
							 << "with value" << value
							 << "for UniqueID:" << uniqueId;

				emit this->asteriskEvent(AmiEventVarSet, newCall);
            }
        }
    } else if (eventName == "Join") {
		QLOG_INFO() << "Channel" << event->value("Channel")
					<< "is joining queue" << event->value("Queue");
		// TODO: Add destination and call
		emit this->asteriskEvent(AmiEventJoin, 0);
    } else if (eventName == "Bridge") {
        // we come from a queue
        /*
        Event: Bridge
        Privilege: call,all
        Bridgestate: Link
        Bridgetype: core
        Channel1: SIP/200-0974a250 <- source channel
        Channel2: SIP/201-097525f8 <- dest channel
        Uniqueid1: bruno-1240133625.2 <- source uid
        Uniqueid2: bruno-1240133628.3 <- dest uid
        CallerID1: 200
        CallerID2: 200
        */

		QString uniqueId1 = event->value("Uniqueid1");
		QString uniqueId2 = event->value("Uniqueid2");

		QLOG_DEBUG() << "Processing Bridge event for channels" << event->value("Channel1")
					 << "( CID" << event->value("Callerid1") << "UID" << uniqueId1 << ") and"
					 << event->value("Channel2")
					 << "( CID" << event->value("Callerid2") << "UID" << uniqueId2 << ")";

		AstCtiCall *newCall = 0;
		if (this->activeCalls.contains(uniqueId1))
			newCall = this->activeCalls.value(uniqueId1);
		else if (this->activeCalls.contains(uniqueId2))
			newCall = this->activeCalls.value(uniqueId2);

		if (newCall != 0) {
			// Let's say where the call is directed. The destination
			// will let us know the client to signal.
			newCall->setDestUniqueId(event->value("Uniqueid2"));
			newCall->setDestChannel(event->value("Channel2"));

			emit this->asteriskEvent(AmiEventBridge, newCall);
		}
    }
}

void AmiClient::evaluateResponse(QHash<QString, QString> *response)
{
    int actionId = response->value("ActionID").toInt();
	if (actionId == 0) {
		QLOG_WARN() << "ActionId is set to 0 in AMIClient::evaluateResponse()";
	}

	if (this->pendingAmiCommands.contains(actionId)) {
		AmiCommand *cmd = this->pendingAmiCommands.value(actionId);
        if (cmd != 0) {
			QString responseString  = response->value("Response");
			QString responseMessage = response->value("Message");

			QLOG_DEBUG() << "Received response" << responseString
						 << "for action:" << AmiCommand::getActionName(cmd->action)
						 << "and channel:" << cmd->exten
						 << ". ActionId:"<< actionId
						 << ". Message:"<< responseMessage;;

			cmd->responseString = responseString;
            cmd->responseMessage = responseMessage;

			//Remove the command from list of pending commands
			this->pendingAmiCommands.remove(actionId);

			switch (cmd->action) {
			case AmiActionLogoff:
				QLOG_INFO() << "Logged off from AMI Server";
				this->amiClientStatus = AmiStatusLoggedOff;
				delete cmd;
				break;
			case AmiActionLogin:
				if (responseString == "Success") {
					QLOG_INFO() << "Authenticated by AMI Server";
					// We don't consider ourselves logged in
					// until we receive FullyBooted event
				} else {
					QLOG_ERROR() << "AMI Server authentication failure. Message:"
								 << responseMessage;

					this->localSocket->close();
					this->amiClientStatus = AmiStatusLoggedOff;
				}
				delete cmd;
				break;
			default:
				//Signal that the response was received.
				//Receiving slot is responsible for deleting the command object
				emit this->asteriskResponse(cmd);
			}
        }
    }
}

bool AmiClient::sendDataToAsterisk(const QString& data)
{
	QLOG_DEBUG() << "Sending data to Asterisk:" << data.simplified();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // We output simple ascii strings (no utf8)    
	out.writeRawData(data.toLatin1(), data.length());

	if (this->localSocket->write(block) > -1 && this->localSocket->flush()) {
		return true;
	} else {
		QLOG_ERROR() << "Sending data to Asterisk failed:"
					 << this->localSocket->errorString();
		this->localSocket->close();
		return false;
	}
}

void AmiClient::sendCommandToAsterisk(AmiCommand *command)
{
	this->currentActionId++;

	QString data = "";
	//Define action
	data.append(QString("Action:%1\r\n").arg(AmiCommand::getActionName(command->action)));
	//Add action ID
	data.append(QString("ActionId:%1\r\n").arg(this->currentActionId));
	//Add parameters, if any
	if (command->parameters != 0) {
		QHashIterator<QString, QString> i(*(command->parameters));
		while (i.hasNext()) {
			i.next();
			data.append(QString("%1:%2\r\n").arg(i.key()).arg(i.value()));
		}
	}
	//Add variables, if any
	if (command->variables != 0) {
		QHashIterator<QString, QString> i(*(command->variables));
		while (i.hasNext()) {
			i.next();
			data.append(QString("Variable:%1=%2\r\n").arg(i.key()).arg(i.value()));
		}
	}
	//End command
	data.append("\r\n");

	if (this->sendDataToAsterisk(data)) {
		//Command successfully sent, we add it to the list of pending commands
		this->pendingAmiCommands.insert(this->currentActionId, command);
	} else {
		//Command not sent, signal that the command has failed and decrease actionId
		//Receiving slot is responsible for deleting the command object
		command->responseString = "Error";
		command->responseMessage = "Command not sent";
		emit this->asteriskResponse(command);
		this->currentActionId--;
	}
}

QString AmiClient::getEventName(const AmiEvent event) {
	switch (event) {
	case AmiEventFullyBooted:
		return "FullyBooted";
	case AmiEventShutdown:
		return "Shutdown";
	case AmiEventNewchannel:
		return "Newchannel";
	case AmiEventNewexten:
		return "Newexten";
	case AmiEventNewstate:
		return "Newstate";
	case AmiEventHangup:
		return "Hangup";
	case AmiEventVarSet:
		return "VarSet";
	case AmiEventJoin:
		return "Join";
	case AmiEventBridge:
		return "Bridge";
	case AmiEventExtensionStatus:
		return "ExtensionStatus";
	case AmiEventPeerStatus:
		return "PeerStatus";
	default:
		return "";
	}
}

QString AmiClient::socketStateToString(QAbstractSocket::SocketState socketState)
{
	switch(socketState) {
	case QAbstractSocket::UnconnectedState:
		return "UnconnectedState";
		break;
	case QAbstractSocket::HostLookupState:
		return "HostLookupState";
		break;
	case QAbstractSocket::ConnectingState:
		return "ConnectingState";
		break;
	case QAbstractSocket::ConnectedState:
		return "ConnectedState";
		break;
	case QAbstractSocket::BoundState:
		return "BoundState";
		break;
	case QAbstractSocket::ClosingState:
		return "ClosingState";
		break;
	case QAbstractSocket::ListeningState:
		return "ListeningState";
		break;
	default:
		return "Unknown";
		break;
	}
}

QString AmiClient::eventToString(QHash<QString, QString> *event)
{
	if (event->isEmpty())
		return "";

	QString result = event->value("Event");
	const int listSize = event->keys().size();
	for (int i = 0; i < listSize; i++) {
		QString argName = event->keys().at(i);
		if (argName != "Event")
			result.append(QString(" (%1=%2)").arg(argName).arg(event->value(argName)));
	}

	return result;
}

void AmiClient::delay(const int secs)
{
	QTime endTime = QTime::currentTime().addSecs(secs);
	while (QTime::currentTime() < endTime && this->isRunning)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
