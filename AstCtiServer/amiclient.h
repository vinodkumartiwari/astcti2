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

#ifndef AMICLIENT_H
#define AMICLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHash>

#include "astcticonfiguration.h"
#include "astctiextension.h"
#include "amicommand.h"
#include "astctichannel.h"

typedef QHash<QString, AstCtiChannel*> AstCtiChannelHash;
typedef QHash<int, AmiCommand*> AmiCommandHash;

enum AmiClientStatus {
	AmiStatusLoggedOff,
	AmiStatusLoggingIn,
	AmiStatusLoggedIn,
	AmiStatusRequestingExtensions,
	AmiStatusRequestingSip,
	AmiStatusRequestingQueues
};
Q_DECLARE_METATYPE(AmiClientStatus)

enum AmiEvent {
	AmiEventNone,
	AmiEventFullyBooted,
	AmiEventShutdown,
	AmiEventNewchannel,
	AmiEventNewexten,
	AmiEventNewstate,
	AmiEventNewCallerid,
	AmiEventNewAccountCode,
	AmiEventDial,
	AmiEventBridge,
	AmiEventUnlink,
	AmiEventHangup,
	AmiEventJoin,
	AmiEventVarSet,
	AmiEventMusicOnHold,
	AmiEventExtensionStatus,
	AmiEventPeerStatus,
	AmiEventAgentStatus, //Fake event, fired when we receive response to Queue commands
	AmiEventPeerEntry,
	AmiEventRTCPReceived,
	AmiEventRTCPSent,
	AmiEventCoreShowChannel,
	AmiEventCoreShowChannelsComplete
};
Q_DECLARE_METATYPE(AmiEvent)

enum AmiConnectionStatus {
	AmiConnectionStatusDisconnected,
	AmiConnectionStatusConnected
};
Q_DECLARE_METATYPE(AmiConnectionStatus)

class AmiClient : public QObject
{
    Q_OBJECT
	Q_ENUMS(AmiClientStatus)
	Q_ENUMS(AmiEvent)
	Q_ENUMS(AmiConnectionStatus)

public:
	explicit AmiClient();
	~AmiClient();

	void               stop();
	static QString     getEventName(const AmiEvent event);
	static QString	   extensionStatusToString(const AstCtiExtensionStatus status);
	static QString     agentStatusToString(const AstCtiAgentStatus status);

public slots:
	void               run();
	void               setParameters(AstCtiConfiguration* config);
	void               sendCommandToAsterisk(AmiCommand* command);

signals:
	void               amiChannelEvent(const AmiEvent eventId, AstCtiChannel* channel);
	void               amiStatusEvent(const AmiEvent eventId, const QString channelName,
									  const int status);
	void               amiConnectionStatusChange(const AmiConnectionStatus status);
	void               amiCommandFailed(const AmiAction action, const QString channelName);

private:
	Q_DISABLE_COPY(AmiClient)
	QString            amiHost;
	quint16            amiPort;
	QString            amiUser;
	QString            amiSecret;
	quint16            amiConnectTimeout;
	quint16            amiConnectRetryAfter;

	QTcpSocket*        localSocket;
	AstCtiChannelHash  freeChannels;
	AstCtiChannelHash  bridgedChannels;
	AmiCommandHash     pendingAmiCommands;
	int                currentActionId;
	bool               isRunning;
	bool               reconnectWarningIssued;
	QString            dataBuffer;
	AmiClientStatus    amiClientStatus;
	bool               sendDataToAsterisk(const QString& data);
	void               buildSocket();
	void               parseDataReceivedFromAsterisk();
	void               performLogin();
	void               performLogoff();
	QStringHash        hashFromMessage(const QString& data);
	void               bridgeChannels(const QString& uniqueId1, const QString& uniqueId2,
									  const QString& channelName1, const QString& channelName2);
	AstCtiChannel*     addConfChannelToBridge(AstCtiChannel* channel, const int startFrom);
	AstCtiChannel*     addChannelToBridge(const int bridgeId, const QString& uniqueId,
										  const QString& channelName);
	void               removeChannelFromBridge(const QString& uniqueId, const QString& channelName);
	bool               isLocalChannel(const QString& channelName);
	void               evaluateEvent(const QStringHash& event);
	void               evaluateResponse(const QStringHash& response);
	QDateTime          parseDuration(const QString& duration);
	QString            socketStateToString(QAbstractSocket::SocketState socketState);
	QString            eventToString(const QStringHash& event);
	void               delay(const int secs);

private slots:
	void               receiveData();
	void               socketDisconnected();
	void               socketStateChanged(QAbstractSocket::SocketState socketState);
	void               socketError(QAbstractSocket::SocketError error);
};

#endif // AMICLIENT_H
