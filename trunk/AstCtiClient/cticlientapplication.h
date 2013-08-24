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
 * AstCtiClient.  If you copy code from other releases into a copy of GNU
 * AstCtiClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCtiClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#ifndef CTICLIENTAPPLICATION_H
#define CTICLIENTAPPLICATION_H

#include <QStringList>
#include <QTcpSocket>
#include <QtSingleApplication>

#include "cticonfig.h"
#include "astcticommand.h"
#include "astctichannel.h"
#include "loginwindow.h"
#include "cticlientwindow.h"
#include "browserwindow.h"

const QString defaultServerHost = "localhost";
const QString defaultServerPort = "5000";
const QString defaultConnectTimeout = "5000";
const QString defaultConnectRetryInterval = "2";
const int     defaultKeepAliveInterval = 5000;

#ifdef Q_OS_WIN
	const QString osType = "Windows";
#endif
#ifdef Q_OS_MAC
	const QString osType = "Macintosh";
#endif
#ifdef Q_OS_LINUX
	const QString osType = "Linux";
#endif

enum CtiClientType {
	CtiClientCallCenter,
	CtiClientPhoneManager
};

enum AstCtiResponseCodes {
    RspOK = 100,
	RspError = 101
};

enum AstCtiErrorCodes {
	ErrNotDefined = 0,
	ErrUnknownCmd = 900,
	ErrWrongParam = 901,
	ErrNoAuth = 902,
	ErrUserLoggedIn = 910,
	ErrWrongCreds = 911,
	ErrUnknownOs = 912,
	ErrWrongMac = 913,
	ErrPassChgFail = 914,
	ErrUnknownChan = 920
};

struct AstCtiResponse {
	AstCtiResponseCodes code;
	AstCtiErrorCodes errorCode;
    QStringList data;
};

class CtiClientApplication : public QtSingleApplication
{
    Q_OBJECT

public:
    CtiClientApplication(const QString& appId, int &argc, char* *argv);
    ~CtiClientApplication();

	bool                     canStart;

public slots:
    //Signals from login window
	void                     loginAccept(const QString& username, const QString& password);
	void                     loginReject();

    //Signals from main window
	void                     changePassword();
	void                     logOff();
	void                     pause(const QString& channelName);

    //Signals from browser
	void                     browserWindowClosed(BrowserWindow* window);
	void                     newBrowserWindow(QUrl url);

    //Signals from QProcess
	void                     applicationClosed(QProcess* process);

signals:
	void                     channelEventReceived(AstCtiChannel* channel);
	void                     agentStatusChanged(const QString& channelName,
												const AstCtiAgentStatus status);
	void                     newMessage(const QString& message,
										QSystemTrayIcon::MessageIcon severity);
	void                     statusChange(bool status);
	void                     newConfig(AstCtiConfiguration* config);
	void                     closeWindow(bool skipCheck);

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

	CtiClientType            clientType;
	ServerConnectionStatus   connectionStatus;
	quint16                  blockSize;
	QString                  macAddress;
	bool                     reconnectNotify;
	QString                  newPassword;
	AstCtiConfiguration*     config;
	AstCtiCommand*           lastCtiCommand;
	QTimer*                  idleTimer;
	QTimer*                  connectTimer;
	QTcpSocket*              localSocket;
	LoginWindow*             loginWindow;
	CtiClientWindow*         mainWindow;
	QList<BrowserWindow*>    browserWindows;
	QList<QProcess*>         applications;


	void                     connectSocket();
	void                     abortConnection(const StopReason stopReason, const QString& message);
	void                     connectionLost();
	void                     resetLastCtiCommand();
	void                     setKeepAliveInterval(const int miliseconds);
	AstCtiResponse           parseResponse(const QString& responseString);
	void                     parseDataReceivedFromServer(const QString& message);
	void                     processResponse(const AstCtiResponse &response);
	void                     processXmlObject(const QString& xmlString);
	QByteArray               convertDataForSending(const QString& data);
	void                     sendDataToServer(const QString& data);
	void                     sendCommandToServer(const AstCtiCommands command);
	void                     sendCommandToServer(const AstCtiCommands command,
												 const QString& parameters);
	void                     showLoginWindow(const QString& message, const bool connectionLost);
	void                     showMainWindow();
	void                     newApplication(const QString& path, const QString& parameters);
	QString                  getCommandName(const AstCtiCommands command);
	QString                  getErrorText(const AstCtiErrorCodes error);

private slots:
	void                     socketConnected();
	void                     socketDisconnected();
	void                     socketStateChanged(QAbstractSocket::SocketState socketState);
	void                     socketError(QAbstractSocket::SocketError error);
	void                     receiveData();
	void                     idleTimerElapsed();
	void                     connectTimerElapsed();
};

#endif // CTICLIENTAPPLICATION_H
