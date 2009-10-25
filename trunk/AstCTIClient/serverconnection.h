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

#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include <QStringList>
#include <QDebug>
#include <QSettings>
#include <QHash>
#include <QStringList>
#include <QTimer>

#include "cticonfig.h"
#include "astcticommand.h"
#include "astcticallxmlparser.h"

#ifdef Q_WS_WIN
const QString osType = "WIN";
#endif
#ifdef Q_WS_MAC
const QString osType = "MAC";
#endif
#ifdef Q_WS_X11
const QString osType = "LIN";
#endif
#ifdef Q_WS_QWS
const QString osType = "QWS";
#endif

const QString separator = "\n";

enum ServerConnectionStatus {
    ConnStatusNotDefined,
    ConnStatusConnecting,
    ConnStatusLoggingIn,
    ConnStatusLoggedIn,
    ConnStatusDisconnecting
};

enum StopReason {
    StopUserDisconnect,
    StopInvalidCredentials,
    StopInternalError,
    StopServerError
};

enum AstCTIResponseCodes {
    RspOK = 100,
    RspError = 101,
    RspAuthOK = 102,
    RspCompressLevel = 103,
    RspKeepAlive = 104,
    RspServiceData = 200,
    RspServiceListEnd = 201,
    RspQueueData = 202,
    RspQueueListEnd = 203,
    RspPauseOK = 300,
    RspPausePending = 301,
    RspPauseError = 302,
    RspDisconnectOK = 900
};

struct AstCTIResponse
{
    AstCTIResponseCodes code;
    QStringList data;
};

class ServerConnection : public QThread
{
    Q_OBJECT

public:
    ServerConnection(AstCTIConfiguration *config);
    ~ServerConnection();
    bool                    isConnected();
    void                    sendDataToServer(const QString &data);
    void                    sendCommandToServer(const AstCTICommands command);
    void                    sendCommandToServer(const AstCTICommands command, const QString &parameters);

signals:
    void                    error(int socketError, const QString &message);
    void                    eventReceived(AstCTICall *astCTICall);
    void                    servicesReceived(QHash<QString, QString> *servicesList);
    void                    queuesReceived(QStringList *queuesList);
    void                    loggedIn(const QString &extension);
    void                    pauseAccepted();
    void                    pauseError(const QString &message);
    void                    connectionLost();
    void                    threadStopped(StopReason stopReason, const QString &message);

private:
    AstCTIConfiguration     *config;
    AstCTICommand           *lastCTICommand;
    QHash<int, QString>     *commandsList;
    QHash<QString, QString> *servicesList;
    QStringList             *queuesList;
    QTimer                  *idleTimer;
    ServerConnectionStatus  connectionStatus;
    QString                 macAddress;
    quint16                 blockSize;
    bool                    reconnectNotify;

    void                    initCTICommands();
    void                    abortThread(StopReason stopReason, const QString &message);
    void                    buildSocket();
    void                    connectSocket();
    void                    parseDataReceivedFromServer(const QString &message);
    AstCTIResponse          parseResponse(const QString &response);
    void                    processResponse(AstCTIResponse &response);
    void                    processEvent(const QString &eventData);
    QByteArray              convertDataForSending(const QString &data);
    QTcpSocket              *localSocket;

private slots:
    void                    threadStarted();
    void                    socketDisconnected();
    void                    socketStateChanged(QAbstractSocket::SocketState socketState);
    void                    socketError(QAbstractSocket::SocketError socketError);
    void                    receiveData();
    void                    idleTimerElapsed();

protected:
    void                    run();
};

#endif // SERVERCONNECTION_H
