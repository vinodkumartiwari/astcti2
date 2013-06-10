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

#ifndef AMICLIENT_H
#define AMICLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHash>

#include "cticonfig.h"
#include "amicommand.h"
#include "qastctiservice.h"
#include "qastctiservices.h"
#include "qastcticall.h"

enum AmiClientStatus {
	AmiStatusLoggedOff,
	AmiStatusLoggingIn,
	AmiStatusLoggedIn,
    AmiStatusRequestingExtensions,
    AmiStatusRequestingSip,
	AmiStatusRequestingQueues
};

enum AmiEvent {
	AmiEventFullyBooted,
	AmiEventShutdown,
	AmiEventNewchannel,
	AmiEventNewexten,
	AmiEventNewstate,
	AmiEventHangup,
	AmiEventVarSet,
	AmiEventJoin,
	AmiEventBridge,
	AmiEventExtensionStatus,
	AmiEventPeerStatus
};

enum AmiConnectionStatus {
    AmiConnectionStatusDisconnected,
	AmiConnectionStatusConnected
};

class AmiClient : public QObject
{
    Q_OBJECT
	Q_ENUMS(AmiClientStatus)
	Q_ENUMS(AmiEvent)
	Q_ENUMS(AmiConnectionStatus)

public:
	explicit AmiClient(AstCTIConfiguration *config);
	~AmiClient();
	QString                         getActionName(const AmiAction action);
	QString                         getEventName(const AmiEvent event);
	void                            stop();

public slots:
	void                            run();
	void                            sendCommandToAsterisk(AmiCommand *command);

signals:
	//void                            error(int socketError, const QString &message);
	//void                            receiveDataFromAsterisk(const QString &data);
	void                            ctiEvent(const AmiEvent &eventId, QAstCTICall *call);
	void                            ctiResponse(AmiCommand *command);
	void                            amiConnectionStatusChange(const AmiConnectionStatus status);

private:
	AstCTIConfiguration            *config;
	QHash<QString, QAstCTICall*>   *activeCalls;
	QHash<int, AmiCommand*>        *pendingAmiCommands;
	int                             currentActionId;
	bool                            isRunning;
    QString                         dataBuffer;
	AmiClientStatus                 amiClientStatus;
	bool                            sendDataToAsterisk(const QString &data);
	void                            buildSocket();
	void                            parseDataReceivedFromAsterisk();
	void                            performLogin();
	void                            performLogoff();
	QHash<QString, QString>*        hashFromMessage(QString data);
	void                            evaluateEvent(QHash<QString, QString> *event);
	void                            evaluateResponse(QHash<QString, QString> *response);
	void							delay(const int secs);

private slots:
    void                            socketStateChanged(QAbstractSocket::SocketState socketState);
    void                            socketError(QAbstractSocket::SocketError socketError);
    void                            socketDisconnected();

protected:
	QTcpSocket                     *localSocket;
};

#endif // AMICLIENT_H
