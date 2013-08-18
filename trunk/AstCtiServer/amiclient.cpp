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
	this->reconnectWarningIssued = false;
	this->amiClientStatus = AmiStatusLoggedOff;
}

AmiClient::~AmiClient()
{
	QLOG_TRACE() << "Destroying AmiClient";

	this->isRunning = false;
	if (this->localSocket->state() == QAbstractSocket::ConnectedState) {
		this->performLogoff();
	}
	qDeleteAll(this->freeChannels);
	qDeleteAll(this->bridgedChannels);
	qDeleteAll(this->pendingAmiCommands);
	delete this->localSocket;
}

void AmiClient::buildSocket()
{
    this->localSocket = new QTcpSocket();

	connect(this->localSocket, SIGNAL(readyRead()),
			this, SLOT(receiveData()));
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

			if (!this->reconnectWarningIssued) {
				QLOG_WARN() << "Unable to connect to"
							<< QString("%1:%2.").arg(this->amiHost).arg(this->amiPort)
							<< "AmiClient will keep trying in" << this->amiConnectRetryAfter
							<< "second intervals.";
				this->reconnectWarningIssued = true;
			}
			this->delay(this->amiConnectRetryAfter);
		} else {
			QLOG_INFO() << "Connected to"
						<< QString("%1:%2.").arg(this->amiHost).arg(this->amiPort);
			this->reconnectWarningIssued = false;
			while (this->isRunning) {
                if (this->localSocket->state() != QAbstractSocket::ConnectedState) {
					this->delay(this->amiConnectRetryAfter);
					break;
				} else {
					QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
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

void AmiClient::setParameters(AstCtiConfiguration* config)
{
	if (this->isRunning) {
		if (this->amiHost != config->amiHost || this->amiPort != config->amiPort ||
			this->amiUser != config->amiUser || this->amiSecret != config->amiSecret) {

			this->performLogoff();
			this->localSocket->disconnectFromHost();
		}
	}

	this->amiHost = config->amiHost;
	this->amiPort = config->amiPort;
	this->amiUser = config->amiUser;
	this->amiSecret = config->amiSecret;
	this->amiConnectTimeout = config->amiConnectTimeout;
	this->amiConnectRetryAfter = config->amiConnectRetryAfter;
}

void AmiClient::receiveData()
{
	QByteArray dataFromSocket = localSocket->readAll();
	QString receivedData = QString(dataFromSocket);
	if (receivedData.trimmed().length() > 0) {
		if (this->amiClientStatus != AmiStatusLoggedOff) {
			this->dataBuffer.append(receivedData);
			if (this->dataBuffer.contains(QStringLiteral("\r\n\r\n")))
				this->parseDataReceivedFromAsterisk();
		} else {
			if (receivedData.contains(QStringLiteral("Asterisk Call Manager"), Qt::CaseInsensitive))
				this->performLogin();
		}
	}
}

void AmiClient::socketDisconnected()
{
	qDeleteAll(this->freeChannels);
	this->freeChannels.clear();
	qDeleteAll(this->bridgedChannels);
	this->bridgedChannels.clear();
	qDeleteAll(this->pendingAmiCommands);
	this->pendingAmiCommands.clear();

	this->amiClientStatus = AmiStatusLoggedOff;
	emit this->amiConnectionStatusChange(AmiConnectionStatusDisconnected);
	QLOG_WARN() << "AmiClient Socket Disconnected";
}

void AmiClient::socketStateChanged(QAbstractSocket::SocketState socketState)
{
//	QLOG_TRACE() << "AmiClient socket state changed:" << this->socketStateToString(socketState);
}

void AmiClient::socketError(QAbstractSocket::SocketError error)
{    
	if (error != QAbstractSocket::SocketTimeoutError) {
		QLOG_ERROR() << "AmiClient Socket Error:" << error << this->localSocket->errorString();
//		this->amiClientStatus = AmiStatusLoggedOff;
//		emit this->amiConnectionStatusChange(AmiConnectionStatusDisconnected);
	}
}

void AmiClient::performLogin()
{
	this->amiClientStatus = AmiStatusLoggingIn;

	AmiCommand* cmd = new AmiCommand(AmiActionLogin);
	cmd->addParameter(QStringLiteral("Username"), this->amiUser);
	cmd->addParameter(QStringLiteral("Secret"), this->amiSecret);
	this->sendCommandToAsterisk(cmd);
}

void AmiClient::performLogoff()
{
	AmiCommand* cmd = new AmiCommand(AmiActionLogoff);
	this->sendCommandToAsterisk(cmd);
}

void AmiClient::parseDataReceivedFromAsterisk()
{
	//Did we receive a complete set of responses
	bool partialData = !this->dataBuffer.endsWith(QStringLiteral("\r\n\r\n"));

	// here we can begin to evaluate AMI responses and events
	QStringList amiMessages = this->dataBuffer.split(QStringLiteral("\r\n\r\n"));
	if (partialData) {
		//We return incomplete portion of the data to the buffer
		this->dataBuffer = amiMessages.last();
		amiMessages.removeLast();
	} else {
		this->dataBuffer.clear();
	}

	// we need to check if the buffer contains more than
	// one messsage and parse them all in the right order
	foreach (const QString& m, amiMessages) {
		if (m.length() > 0) {
			QStringHash message = this->hashFromMessage(m);
			// We should have at least one Event or response key in the hash!
			if (message.contains(QStringLiteral("Event")))
				this->evaluateEvent(message);
			else if (message.contains(QStringLiteral("Response")))
				this->evaluateResponse(message);
			else
				QLOG_WARN() << "Received unrecognized message from Asterisk" << message;
		}
	}
}

QStringHash AmiClient::hashFromMessage(const QString &data)
{
	QStringHash hash;
    QStringList lines = data.split('\n');
	foreach (const QString& line, lines) {
        if (line.contains(':')) {
			QStringList keyValue = line.split(':');
            if (keyValue.length() > 1) {
                QString key     = keyValue.at(0);
                QString value   = "";
				for (int i = 1; i < keyValue.length(); i++) {
					value.append(keyValue.at(i).trimmed());
					if (i < keyValue.length() - 1)
						value.append(":");
                }
				hash.insert(key.trimmed(), value.trimmed());
            }
        }
    }
    return hash;
}

AstCtiChannel* AmiClient::addChannelToBridge(const int bridgeId, const QString& uniqueId,
											 const QString& channelName)
{
	AstCtiChannel* channel = this->freeChannels.take(uniqueId);
	if (channel != 0) {
		if (bridgeId == 0)
			channel->setBridgeId(AstCtiChannel::getNextBridgeId());
		else
			channel->setBridgeId(bridgeId);

		this->bridgedChannels.insert(uniqueId, channel);
		emit this->amiChannelEvent(AmiEventBridge, channel);
		return channel;
	}

	QLOG_WARN() << "Channel" << uniqueId << channelName
				<< "is entering a bridge, but it is not found in the list of free channels.";
	return 0;
}

void AmiClient::removeChannelFromBridge(const QString& uniqueId, const QString& channelName)
{
	AstCtiChannel* channel = this->bridgedChannels.take(uniqueId);
	if (channel != 0) {
		this->freeChannels.insert(uniqueId, channel);
		emit this->amiChannelEvent(AmiEventUnlink, channel);
		return;
	}

	QLOG_WARN() << "Channel" << uniqueId << channelName
				<< "is being unlinked, but it is not found in the list of bridged channels.";
}

bool AmiClient::isLocalChannel(const QString& channelName)
{
	return channelName.contains(QStringLiteral("Local/"));
}

void AmiClient::evaluateEvent(const QStringHash& event)
{
	QLOG_DEBUG() << "Received Asterisk event" << this->eventToString(event);

	const QString eventName = event.value(QStringLiteral("Event"));

	if (eventName == QStringLiteral("FullyBooted")) {
		// Asterisk is fully booted now, so we can interact with it freely
		// We don't consider to be logged in until we receive this event
		if (this->amiClientStatus == AmiStatusLoggingIn) {
			// In rare cases, this event can be received twice,
			// so we change status only once
			this->amiClientStatus = AmiStatusLoggedIn;
			emit this->amiConnectionStatusChange(AmiConnectionStatusConnected);
		}
	} else if (eventName == QStringLiteral("Shutdown")) {
		QLOG_WARN() << "Asterisk shutting down. Connection lost.";

		this->localSocket->close();
		this->amiClientStatus = AmiStatusLoggedOff;

		//emit this->asteriskEvent(AmiEventShutdown, 0, "");
	} else if (eventName == QStringLiteral("Newchannel")) {
		const QString channelName = event.value(QStringLiteral("Channel"));
		// We ignore local channels, they just bridge "real" channels
		// Local channels always consist of two halves (two virtual channels)
		// On Bridge event, we will associate each half with its "real" channel counterpart
		if (!this->isLocalChannel(channelName)) {
			// Build a new asterisk channel object and add it ot list of free channels
			const QString uniqueId = event.value(QStringLiteral("Uniqueid"));
			const QString exten = event.value(QStringLiteral("Exten"));
			AstCtiChannel* newChannel = new AstCtiChannel(uniqueId);
			newChannel->setChannel(channelName);
			newChannel->setCalleridNum(event.value(QStringLiteral("CallerIDNum")));
			newChannel->setCalleridName(event.value(QStringLiteral("CallerIDName")));
			newChannel->setContext(event.value(QStringLiteral("Context")));
			newChannel->setDialedLineNum(exten);
			newChannel->setState((AstCtiChannelState)event.value(QStringLiteral("ChannelState"))
								 .toInt());
			newChannel->setAccountCode(event.value(QStringLiteral("AccountCode")));

			this->freeChannels.insert(uniqueId, newChannel);

			QLOG_DEBUG() << "Newchannel" << channelName
						 << "ID" << uniqueId
						 << "CID" << event.value("CallerIDNum")
						 << "in state:" << event.value("ChannelStateDesc");

			// TODO
			emit this->amiChannelEvent(AmiEventNewchannel, newChannel);
		}
	} else if (eventName == QStringLiteral("Newstate")) {
		const QString channelName = event.value(QStringLiteral("Channel"));
		if (!this->isLocalChannel(channelName)) {
			const QString uniqueId = event.value(QStringLiteral("Uniqueid"));
			AstCtiChannel* channel = this->freeChannels.value(uniqueId);
			if (channel == 0)
				channel = this->bridgedChannels.value(uniqueId);

			if (channel != 0) {
				const AstCtiChannelState state =
						(AstCtiChannelState)event.value(QStringLiteral("ChannelState")).toInt();
				channel->setState(state);
				channel->setConnectedLineNum(event.value(QStringLiteral("ConnectedLineNum")));
				emit this->amiChannelEvent(AmiEventNewstate, channel);

				QLOG_DEBUG() << "New state" << event.value("ChannelStateDesc")
							 << "for channel" << uniqueId << channelName
							 << "CallerID" << event.value("CallerIDNum");
			} else {
				QLOG_WARN() << "Received Newstate event for channel"
							<< uniqueId << channelName
							<< "CallerID" << event.value("CallerIDNum")
							<< "but it does not exist in channel list.";
			}
		}
	} else if (eventName == QStringLiteral("NewCallerid")) {
		const QString channelName = event.value(QStringLiteral("Channel"));
		if (!this->isLocalChannel(channelName)) {
			const QString uniqueId = event.value(QStringLiteral("Uniqueid"));
			AstCtiChannel* channel = this->freeChannels.value(uniqueId);
			if (channel == 0)
				channel = this->bridgedChannels.value(uniqueId);

			if (channel != 0) {
				const QString cidNum = event.value(QStringLiteral("CallerIDNum"));
				const QString cidName = event.value(QStringLiteral("CallerIDName"));
				channel->setCalleridNum(cidNum);
				channel->setCalleridName(cidName);
				emit this->amiChannelEvent(AmiEventNewCallerid, channel);

				QLOG_DEBUG() << "New caller ID" << cidNum << cidName
							 << "for channel" << uniqueId << channelName;
			} else {
				QLOG_WARN() << "Received NewCallerid event for channel"
							<< uniqueId << channelName
							<< "CallerID" << event.value("CallerIDNum")
							<< "but it does not exist in channel list.";
			}
		}
	} else if (eventName == QStringLiteral("NewAccountCode")) {
		const QString channelName = event.value(QStringLiteral("Channel"));
		if (!this->isLocalChannel(channelName)) {
			const QString uniqueId = event.value(QStringLiteral("Uniqueid"));
			AstCtiChannel* channel = this->freeChannels.value(uniqueId);
			if (channel == 0)
				channel = this->bridgedChannels.value(uniqueId);

			if (channel != 0) {
				const QString accountCode = event.value(QStringLiteral("AccountCode"));
				channel->setAccountCode(accountCode);
				emit this->amiChannelEvent(AmiEventNewAccountCode, channel);

				QLOG_DEBUG() << "New account code" << accountCode
							 << "for channel" << uniqueId << channelName;
			} else {
				QLOG_WARN() << "Received NewAccountCode event for channel"
							<< uniqueId << channelName
							<< "CallerID" << event.value("CallerIDNum")
							<< "but it does not exist in channel list.";
			}
		}
	} else if (eventName == QStringLiteral("MusicOnHold")) {
		const QString channelName = event.value(QStringLiteral("Channel"));
		if (!this->isLocalChannel(channelName)) {
			const QString uniqueId = event.value(QStringLiteral("Uniqueid"));
			AstCtiChannel* channel = this->freeChannels.value(uniqueId);
			if (channel == 0)
				channel = this->bridgedChannels.value(uniqueId);

			if (channel != 0) {
				const QString state = event.value(QStringLiteral("State"));
				channel->setMusicOnHoldState(state);
				emit this->amiChannelEvent(AmiEventMusicOnHold, channel);

				QLOG_DEBUG() << "Music on hold" << state
							 << "for channel" << uniqueId << channelName;
			} else {
				QLOG_WARN() << "Received MusicOnHold event for channel"
							<< uniqueId << channelName
							<< "but it does not exist in channel list.";
			}
		}
	} else if (eventName == QStringLiteral("Unlink")) {
		// On Unlink, we remove the channel from bridged channels, but we dont destroy it,
		// because Asterisk may attempt to bridge the channels again
		// Channels are only destroyed on Hangup
		const QString uniqueId1 = event.value(QStringLiteral("Uniqueid1"));
		const QString uniqueId2 = event.value(QStringLiteral("Uniqueid2"));
		const QString channelName1 = event.value(QStringLiteral("Channel1"));
		const QString channelName2 = event.value(QStringLiteral("Channel2"));
		const bool isLocal1 = this->isLocalChannel(channelName1);
		const bool isLocal2 = this->isLocalChannel(channelName2);

		if (isLocal1 && isLocal2) {
			// Both channels are local, nothing to do.
			// This should not happen, but we check for it anyway
		} else if (isLocal2) {
			// Calling channel is being unlinked from it's local channel half
			AstCtiChannel* channel = this->freeChannels.value(uniqueId2);
			if (channel != 0)
				// Channel is not bridged yet, we just remove the association
				channel->setAssociatedLocalChannel(QStringLiteral(""));
			else
				// Channel is bridged, we unbridge it
				this->removeChannelFromBridge(uniqueId2, channelName2);
		} else if (isLocal1) {
			// Called channel is being unlinked from it's local channel half
			AstCtiChannel* channel = this->freeChannels.value(uniqueId1);
			if (channel != 0)
				// Channel is not bridged yet, we just remove the association
				channel->setAssociatedLocalChannel(QStringLiteral(""));
			else
				// Channel is bridged, we unbridge it
				this->removeChannelFromBridge(uniqueId1, channelName1);
		} else {
			this->removeChannelFromBridge(uniqueId1, channelName1);
			this->removeChannelFromBridge(uniqueId2, channelName2);
		}
	} else if (eventName == QStringLiteral("Hangup")) {
		const QString channelName = event.value(QStringLiteral("Channel"));
		if (!this->isLocalChannel(channelName)) {
			const QString uniqueId = event.value(QStringLiteral("Uniqueid"));
			const QString hangupCause = event.value(QStringLiteral("Cause"));
			AstCtiChannel* channel = this->freeChannels.value(uniqueId);
			if (channel != 0) {
				channel->setHangupCause(hangupCause);
				//Receiving slot is responsible for deleting the call object
				emit this->amiChannelEvent(AmiEventHangup, channel);
				// Delete the channel from the hashtable
				this->freeChannels.remove(uniqueId);

				QLOG_DEBUG() << "Hangup:" << "ID" << uniqueId << channelName
							 << "CallerID" << event.value("CallerIDNum")
							 << "with cause:" << hangupCause;
			} else {
				QLOG_WARN() << "Received Hangup event for channel"
							<< uniqueId << channelName
							<< "CallerID" << event.value("CallerIDNum")
							<< "with cause:" << hangupCause
							<< "but it does not exist in channel list.";
			}
		}
	} else if (eventName == QStringLiteral("Newexten")) {
		const QString channelName = event.value(QStringLiteral("Channel"));
		if (!this->isLocalChannel(channelName)) {
			const QString context = event.value(QStringLiteral("Context"));
			const QString uniqueId = event.value(QStringLiteral("Uniqueid"));
			AstCtiChannel* channel = this->freeChannels.value(uniqueId);
			if (channel = 0)
				channel = this->bridgedChannels.value(uniqueId);

			if (channel != 0 && channel->getContext() != context) {
				channel->setContext(context);
				QLOG_INFO() << "Call context for" << uniqueId << channelName << "is now" << context;
				emit this->amiChannelEvent(AmiEventNewexten, channel);
			}
		}
	} else if (eventName == QStringLiteral("VarSet")) {
		const QString channelName = event.value(QStringLiteral("Channel"));
		if (!this->isLocalChannel(channelName)) {
			const QString uniqueId = event.value(QStringLiteral("Uniqueid"));
			AstCtiChannel* channel = this->bridgedChannels.value(uniqueId);
			if (channel == 0)
				channel = this->freeChannels.value(uniqueId);

			if (channel != 0) {
				const QString variable = event.value(QStringLiteral("Variable"));
				const QString value = event.value(QStringLiteral("Value"));

				//Variable will be set only if it exists in channel object
				if (channel->setVariable(variable, value)) {
					QLOG_DEBUG() << "Reading variable" << variable
								 << "with value" << value
								 << "for channel" << uniqueId << channelName;

					emit this->amiChannelEvent(AmiEventVarSet, channel);
				}
			}
		}
	} else if (eventName == QStringLiteral("Join")) {
		const QString uniqueId = event.value(QStringLiteral("Uniqueid"));
		const QString queue = event.value(QStringLiteral("Queue"));
		AstCtiChannel* channel = this->freeChannels.value(uniqueId);
		if (channel != 0) {
			QLOG_INFO() << "Channel" << uniqueId << event.value(QStringLiteral("Channel"))
						<< "is joining queue" << queue;
			channel->setQueue(queue);
			emit this->amiChannelEvent(AmiEventJoin, channel);
		} else {
			QLOG_WARN() << "Channel" << uniqueId << event.value(QStringLiteral("Channel"))
						<< "is joining queue" << queue
						<< "but it is not found in the list of free channels.";
		}
	} else if (eventName == "Bridge") {
		const QString uniqueId1 = event.value(QStringLiteral("Uniqueid1"));
		const QString uniqueId2 = event.value(QStringLiteral("Uniqueid2"));
		const QString channelName1 = event.value(QStringLiteral("Channel1"));
		const QString channelName2 = event.value(QStringLiteral("Channel2"));

		QLOG_DEBUG() << "Processing Bridge event for channels" << channelName1
					 << "( CID" << event.value("Callerid1") << "UID" << uniqueId1 << ") and"
					 << channelName2
					 << "( CID" << event.value("Callerid2") << "UID" << uniqueId2 << ")";

		const bool isLocal1 = this->isLocalChannel(channelName1);
		const bool isLocal2 = this->isLocalChannel(channelName2);

		if (isLocal1 && isLocal2) {
			// Both channels are local, nothing to do.
			// This should not happen, but we check for it anyway
		} else if (isLocal2) {
			// Calling channel is being bridged with first half of the local channel
			// We associate the local channel with the calling channel,
			// but don't indicate any bridging yet
			AstCtiChannel* channel = this->freeChannels.value(uniqueId1);
			if (channel != 0) {
				channel->setAssociatedLocalChannel(channelName2);
				QLOG_DEBUG() << "Associating local channel" << uniqueId2 << channelName2
							 << "with calling channel" << uniqueId1 << channelName1;
			} else {
				// We check if the channel is already bridged
				// Asterisk will sometimes temporarily break the bridge
				// In this case, we don't have to do anything
				channel = this->bridgedChannels.value(uniqueId1);
				if (channel == 0) {
					QLOG_WARN() << "Channel" << uniqueId1 << channelName1
								<< "is being bridged with local channel"
								<< uniqueId2 << channelName2
								<< "but it is not found in the list of free channels.";
				}
			}
		} else if (isLocal1) {
			// Called channel is being bridged with second half of the local channel
			// We associate the local channel with the called channel,
			// and indicate that calling channel and called channel are bridged
			AstCtiChannel* channel2 = this->freeChannels.value(uniqueId2);
			if (channel2 != 0) {
				channel2->setAssociatedLocalChannel(channelName1);
				QLOG_DEBUG() << "Associating local channel" << uniqueId1 << channelName1
							 << "with called channel" << uniqueId2 << channelName2;

				AstCtiChannel* channel1 = 0;
				const int listSize = this->freeChannels.size();
				for (int i = 0; i < listSize; i++) {
					AstCtiChannel* channel = this->freeChannels.values().at(i);
					if (channel->hasMatchingLocalChannel(channelName1)) {
						channel1 = channel;
						break;
					}
				}
				if (channel1 != 0) {
					channel1 = this->addChannelToBridge(0, uniqueId1, channelName1);
					channel2 = this->addChannelToBridge(channel1->getBridgeId(),
														uniqueId2, channelName2);
				} else {
					QLOG_WARN() << "Channel" << uniqueId2 << channelName2
								<< "is being bridged with another channel via local channel"
								<< uniqueId1 << channelName1 << "but matching channel"
								<< "is not found in the list of free channels.";

					// We delete the channel because we don't have a matching channel to bridge
					this->freeChannels.remove(uniqueId2);
					delete channel2;
				}
			} else {
				// We check if the channel is already bridged
				// Asterisk will sometimes temporarily break the bridge
				// In this case, we don't have to do anything
				channel2 = this->bridgedChannels.value(uniqueId2);
				if (channel2 == 0) {
					QLOG_WARN() << "Channel" << uniqueId2 << channelName2
								<< "is being bridged with local channel"
								<< uniqueId1 << channelName1
								<< "but it is not found in the list of free channels.";
				}
			}
		} else {
			AstCtiChannel* channel1 = this->bridgedChannels.value(uniqueId1);
			AstCtiChannel* channel2 = this->bridgedChannels.value(uniqueId2);

			if (channel1 != 0 && channel2 != 0) {
				// Both channels already exist in bridged channels
				// Asterisk will sometimes temporarily break and then re-establish the bridge
				// (e.g. when interpreting DTMF, so for each DTMF we will get
				// one Brige event with Bridgestate=Unlink and one with Bridgestate=Link)
				// This is internal stuff which shouldn't even be exposed to AMI,
				// so we don't need to do anything in this case
			} else if (channel1 != 0) {
				// First channel already exists in bridged channels
				channel2 =
						this->addChannelToBridge(channel1->getBridgeId(), uniqueId2, channelName2);
			} else if (channel2 != 0) {
				// Second channel already exists in bridged channels
				channel1 =
						this->addChannelToBridge(channel2->getBridgeId(), uniqueId1, channelName1);
			} else {
				// Neither channel already exists in bridged channels,
				// we need to create a new bridge
				channel1 = this->addChannelToBridge(0, uniqueId1, channelName1);
				channel2 = this->addChannelToBridge(channel1->getBridgeId(),
													uniqueId2, channelName2);
				if (channel1 != 0 && channel2 == 0) {
					// Only the first channel was found, so we don't have a complete bridge
					// This should not happen, we must unbridge the first channel
					QLOG_WARN() << "Channel" << uniqueId2 << channelName2
								<< "is being bridged with channel"
								<< uniqueId1 << channelName1
								<< "but it is not found in the list of free channels.";
					this->removeChannelFromBridge(uniqueId1, channelName1);
				} else if (channel1 == 0 && channel2 != 0) {
					// Only the second channel was found, so we don't have a complete bridge
					// This should not happen, we must unbridge the second channel
					QLOG_WARN() << "Channel" << uniqueId1 << channelName1
								<< "is being bridged with channel"
								<< uniqueId2 << channelName2
								<< "but it is not found in the list of free channels.";
					this->removeChannelFromBridge(uniqueId2, channelName2);
				}
			}
		}
	} else if (eventName == QStringLiteral("ExtensionStatus")) {
		const QString extension = event.value(QStringLiteral("Hint"));
		const QString status = event.value(QStringLiteral("Status"));
		emit this->amiStatusEvent(AmiEventExtensionStatus, extension, status);

		QLOG_DEBUG() << "Received new status"
					 << this->extensionStatusToString((AstCtiExtensionStatus)status.toInt())
					 << "for extension" << extension;
	} else if (eventName == QStringLiteral("PeerStatus")) {
		// We shouldn't need this event because ExtensionStatus event
		// should follow any change in peer status
		// However, we process this in case ExtensionStatus doesn't work for some reason
		const QString extension = event.value(QStringLiteral("Peer"));
		const QString peerStatus = event.value(QStringLiteral("PeerStatus"));
		// We use the same values as for extension status
		// "Registered" and "Reachable" = ExtensionStatusNotInUse
		// "Unregistered" and "Unreachable" = ExtensionStatusUnavailable
		// The only other value is "Lagged", which we ignore
		QString status = "";
		if (peerStatus == QStringLiteral("Registered") ||
			peerStatus == QStringLiteral("Reachable"))
			status = QString::number((int)ExtensionStatusNotInUse);
		else if (peerStatus == QStringLiteral("Unregistered") ||
				 peerStatus == QStringLiteral("Unreachable"))
			status = QString::number((int)ExtensionStatusUnavailable);

		if (status.length() > 0)
			emit this->amiStatusEvent(AmiEventPeerStatus, extension, status);

		QLOG_DEBUG() << "Received new status" << peerStatus
					 << "for peer" << extension;
	} else if (eventName == QStringLiteral("PeerEntry")) {
		// This event occurs when we request peer status with commands
		// should follow any change in peer status
		// However, we process this in case ExtensionStatus doesn't work for some reason
		const QString extension = event.value(QStringLiteral("Peer"));
		const QString peerStatus = event.value(QStringLiteral("PeerStatus"));
		// We use the same values as for extension status
		// "Registered" and "Reachable" = ExtensionStatusNotInUse
		// "Unregistered" and "Unreachable" = ExtensionStatusUnavailable
		// The only other value is "Lagged", which we ignore
		QString status = "";
		if (peerStatus == QStringLiteral("Registered") ||
			peerStatus == QStringLiteral("Reachable"))
			status = QString::number((int)ExtensionStatusNotInUse);
		else if (peerStatus == QStringLiteral("Unregistered") ||
				 peerStatus == QStringLiteral("Unreachable"))
			status = QString::number((int)ExtensionStatusUnavailable);

		if (status.length() > 0)
			emit this->amiStatusEvent(AmiEventPeerStatus, extension, status);

		QLOG_DEBUG() << "Received new status" << peerStatus
					 << "for peer" << extension;
	}
}

void AmiClient::evaluateResponse(const QStringHash& response)
{
	const int actionId = response.value(QStringLiteral("ActionID")).toInt();
	if (actionId == 0) {
		QLOG_WARN() << "ActionId is set to 0 in AMIClient::evaluateResponse()";
	}

	if (this->pendingAmiCommands.contains(actionId)) {
		AmiCommand* cmd = this->pendingAmiCommands.value(actionId);
        if (cmd != 0) {
			QString responseString  = response.value(QStringLiteral("Response"));
			QString responseMessage = response.value(QStringLiteral("Message"));

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
				if (responseString == QStringLiteral("Success")) {
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
				emit this->amiResponse(cmd);
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

void AmiClient::sendCommandToAsterisk(AmiCommand* command)
{
	this->currentActionId++;

	if (this->sendDataToAsterisk(command->toString(this->currentActionId))) {
		//Command successfully sent, we add it to the list of pending commands
		this->pendingAmiCommands.insert(this->currentActionId, command);
	} else {
		//Command not sent, signal that the command has failed and decrease actionId
		//Receiving slot is responsible for deleting the command object
		command->responseString = QStringLiteral("Error");
		command->responseMessage = QStringLiteral("Command not sent");
		emit this->amiResponse(command);
		this->currentActionId--;
	}
}

QString AmiClient::getEventName(const AmiEvent event) {
	//We use a variable to exploit NRVO
	QString eventName;

	switch (event) {
	case AmiEventFullyBooted:
		eventName = QStringLiteral("FullyBooted");
		break;
	case AmiEventShutdown:
		eventName = QStringLiteral("Shutdown");
		break;
	case AmiEventNewchannel:
		eventName = QStringLiteral("Newchannel");
		break;
	case AmiEventNewexten:
		eventName = QStringLiteral("Newexten");
		break;
	case AmiEventNewstate:
		eventName = QStringLiteral("Newstate");
		break;
	case AmiEventNewCallerid:
		eventName = QStringLiteral("NewCallerid");
		break;
	case AmiEventDial:
		eventName = QStringLiteral("Dial");
		break;
	case AmiEventBridge:
		eventName = QStringLiteral("Bridge");
		break;
	case AmiEventUnlink:
		eventName = QStringLiteral("Unlink");
		break;
	case AmiEventHangup:
		eventName = QStringLiteral("Hangup");
		break;
	case AmiEventJoin:
		eventName = QStringLiteral("Join");
		break;
	case AmiEventVarSet:
		eventName = QStringLiteral("VarSet");
		break;
	case AmiEventMusicOnHold:
		eventName = QStringLiteral("MusicOnHold");
		break;
	case AmiEventExtensionStatus:
		eventName = QStringLiteral("ExtensionStatus");
		break;
	case AmiEventPeerStatus:
		eventName = QStringLiteral("PeerStatus");
		break;
	case AmiEventRTCPReceived:
		eventName = QStringLiteral("RTCPReceived");
		break;
	case AmiEventRTCPSent:
		eventName = QStringLiteral("RTCPSent");
		break;
	default:
		eventName = QStringLiteral("");
		break;
	}

	return eventName;
}

QString AmiClient::socketStateToString(QAbstractSocket::SocketState socketState)
{
	//We use a variable to exploit NRVO
	QString stateName;

	switch(socketState) {
	case QAbstractSocket::UnconnectedState:
		stateName = QStringLiteral("UnconnectedState");
		break;
	case QAbstractSocket::HostLookupState:
		stateName = QStringLiteral("HostLookupState");
		break;
	case QAbstractSocket::ConnectingState:
		stateName = QStringLiteral("ConnectingState");
		break;
	case QAbstractSocket::ConnectedState:
		stateName = QStringLiteral("ConnectedState");
		break;
	case QAbstractSocket::BoundState:
		stateName = QStringLiteral("BoundState");
		break;
	case QAbstractSocket::ClosingState:
		stateName = QStringLiteral("ClosingState");
		break;
	case QAbstractSocket::ListeningState:
		stateName = QStringLiteral("ListeningState");
		break;
	default:
		stateName = QStringLiteral("Unknown");
		break;
	}

	return stateName;
}

QString AmiClient::eventToString(const QStringHash& event)
{
	QString result = QStringLiteral("");
	if (!event.isEmpty()) {
		result = event.value(QStringLiteral("Event"));
		const int listSize = event.keys().size();
		for (int i = 0; i < listSize; i++) {
			QString argName = event.keys().at(i);
			if (argName != QStringLiteral("Event"))
				result.append(QString(" (%1=%2)").arg(argName).arg(event.value(argName)));
		}
	}
	return result;
}

QString AmiClient::extensionStatusToString(const AstCtiExtensionStatus status)
{
	QString result = QStringLiteral("");
	if (status & ExtensionStatusInUse)
		result.append(QStringLiteral("InUse|"));
	if (status & ExtensionStatusBusy)
		result.append(QStringLiteral("Busy|"));
	if (status & ExtensionStatusUnavailable)
		result.append(QStringLiteral("Unavailable|"));
	if (status & ExtensionStatusRinging)
		result.append(QStringLiteral("Ringing|"));

	if (result.length() == 0)
		result = QStringLiteral("NotInUse");
	else
		result.chop(1);

	return result;
}

void AmiClient::delay(const int secs)
{
	QTime endTime = QTime::currentTime().addSecs(secs);
	while (QTime::currentTime() < endTime && this->isRunning)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
