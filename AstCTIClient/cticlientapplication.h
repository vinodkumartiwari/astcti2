/* Copyright (C) 2007-2010 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * Copyright (C) 2007-2010 Lumiss d.o.o.
 * http://www.lumiss.hr
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
 * AstCTIClient.  If you copy code from other releases into a copy of GNU
 * AstCTIClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTIClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#ifndef CTICLIENTAPPLICATION_H
#define CTICLIENTAPPLICATION_H

#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtCore/QUrl>
#include <QtCore/QPointer>
#include <QTcpSocket>
#include <QStringList>
#include <QHash>
#include <QTimer>

#include <QtSingleApplication>

#include "coreconstants.h"
#include "argumentlist.h"
#include "cticonfig.h"
#include "astcticommand.h"
#include "astcticallxmlparser.h"
#include "loginwindow.h"
#include "passwordwindow.h"
#include "compactwindow.h"
#include "managerwindow.h"
#include "browserwindow.h"

#ifdef Q_WS_WIN
const QString osType = "WINDOWS";
#endif
#ifdef Q_WS_MAC
const QString osType = "MACINTOSH";
#endif
#ifdef Q_WS_X11
const QString osType = "LINUX";
#endif
#ifdef Q_WS_QWS
const QString osType = "QWS";
#endif

const QString defaultServerHost = "localhost";
const QString defaultServerPort = "5000";
const QString defaultConnectTimeout = "2000";
const QString defaultConnectRetryInterval = "2";
const int defaultKeepAliveInterval = 5000;

enum CTIClientType {
    CTIClientCallCenter,
    CTIClientPhoneManager
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

struct AstCTIResponse {
    AstCTIResponseCodes code;
    QStringList data;
};

class CtiClientApplication : public QtSingleApplication
{
    Q_OBJECT

public:
    CtiClientApplication(const QString &appId, int &argc, char **argv);
    ~CtiClientApplication();

    bool                    showLoginWindow();

public slots:
    //Signals from login window
    void                    loginAccept(const QString &username, const QString &password);
    void                    loginReject();

    //Signals from main window
    void                    changePassword();
    void                    logOff();
    void                    pause(bool paused);

    //Signals from browser
    void                    browserWindowClosed(BrowserWindow *window);
    void                    newBrowserWindow(QUrl url);

    //Signals from QProcess
    void                    applicationClosed(QProcess *process);

signals:
    void                    eventReceived(AstCTICall *astCTICall);
    void                    servicesReceived(QHash<QString, QString> *servicesList);
    void                    queuesReceived(QStringList *queuesList);
    void                    pauseAccepted();
    void                    pauseError(const QString &message);
    void                    newMessage(const QString &message, QSystemTrayIcon::MessageIcon severity);
    void                    statusChange(bool status);
    void                    closeWindow(bool skipCheck);

private:
    enum ServerConnectionStatus {
        ConnStatusDisconnected,
        ConnStatusConnecting,
        ConnStatusLoggingIn,
        ConnStatusLoggedIn
    };
    enum StopReason {
        StopUserDisconnect,
        StopInvalidCredentials,
        StopInternalError,
        StopServerError
    };

	bool                     canStart;
	CTIClientType            clientType;
	ServerConnectionStatus   connectionStatus;
	quint16                  blockSize;
	QString                  macAddress;
	QString                  extension;
	bool                     reconnectNotify;
	QString                  newPassword;
    AstCtiConfiguration     *config;
    AstCTICommand           *lastCTICommand;
    QHash<int, QString>     *commandsList;
    QHash<QString, QString> *servicesList;
    QStringList             *queuesList;
    QTimer                  *idleTimer;
    QTimer                  *connectTimer;
    QTcpSocket              *localSocket;
    LoginWindow             *loginWindow;
    CTIClientWindow         *mainWindow;
	QList<BrowserWindow*>    browserWindows;
	QList<QProcess*>         applications;

	void                     connectSocket();
	void                     abortConnection(StopReason stopReason, const QString &message);
	void                     connectionLost();
	void                     resetLastCTICommand();
	AstCTIResponse           parseResponse(const QString &response);
	void                     parseDataReceivedFromServer(const QString &message);
	void                     processResponse(AstCTIResponse &response);
	void                     processEvent(const QString &eventData);
	QByteArray               convertDataForSending(const QString &data);
	void                     sendDataToServer(const QString &data);
	void                     sendCommandToServer(const AstCTICommands command);
	void                     sendCommandToServer(const AstCTICommands command,
												 const QString &parameters);
	void                     showMainWindow(const QString &extension);
	void                     newApplication(const QString &path, const QString &parameters);

private slots:
	void                     socketConnected();
	void                     socketDisconnected();
	void                     socketStateChanged(QAbstractSocket::SocketState socketState);
	void                     socketError(QAbstractSocket::SocketError socketError);
	void                     receiveData();
	void                     idleTimerElapsed();
	void                     connectTimerElapsed();
};

#endif // CTICLIENTAPPLICATION_H
