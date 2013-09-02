/* Copyright (C) 2007-2013 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * Copyright (C) 2007-2013 Lumiss d.o.o.
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

#ifndef ASTCTITCPCLIENT_H
#define ASTCTITCPCLIENT_H

#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QQueue>

#include "astcticommand.h"

// Server responses
#define RSP_OK             "OK"
#define RSP_ERR            "KO"

const int defaultKeepAliveInterval = 5000;

#ifdef Q_OS_WIN
	const QString osType = "Windows";
#endif
#ifdef Q_OS_MAC
	const QString osType = "Macintosh";
#endif
#ifdef Q_OS_LINUX
	const QString osType = "Linux";
#endif

enum AstCtiResponseCodes {
	RspOK,
	RspError
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
	ErrSeatTaken = 914,
	ErrPassChgFail = 915,
	ErrUnknownChan = 920
};

struct AstCtiResponse {
	AstCtiResponseCodes code;
	AstCtiErrorCodes errorCode;
	QStringList data;
};

class AstCtiTcpClient : public QObject
{
	Q_OBJECT

public:
	explicit AstCtiTcpClient(const QString& serverHost, const quint16 serverPort,
							 const int connectTimeout, const quint32 connectRetryInterval,
							 const bool debug, QObject *parent = 0);
	~AstCtiTcpClient();

public slots:
	void sendCommand(AstCtiCommands command, const QStringList& parameters);
	void start(const QString& username, const QString& password);

signals:
	void connectionLost();
	void serverMessage(const QString& message);
	void serverResponse(AstCtiErrorCodes errorCode, AstCtiCommands command,
						const QStringList& parameters);
	
private:
	enum ServerConnectionStatus {
		ConnStatusDisconnected,
		ConnStatusConnecting,
		ConnStatusLoggingIn,
		ConnStatusLoggedIn
	};

	void                     connectSocket();
	void                     abortConnection();
	void                     resetLastCtiCommand();
	void                     setKeepAliveInterval(const int miliseconds);
	const QByteArray         convertDataForSending(const QString& data);
	void                     sendCommand(AstCtiCommand* const cmd);
	void                     sendDataToServer(const QString& data);
	void                     parseDataReceivedFromServer(const QString& message);
	const AstCtiResponse     parseResponse(const QString& responseString);
	void                     processResponse(const AstCtiResponse& response);
	const QString            getCommandName(const AstCtiCommands command);

	ServerConnectionStatus   connectionStatus;
	quint16                  blockSize;
	bool                     debug;
	QString                  serverHost;
	quint16                  serverPort;
	int                      connectTimeout;
	quint32                  connectRetryInterval;
	int                      compressionLevel;
	int                      keepAliveInterval;
	QString                  userName;
	QString                  password;
	QString                  macAddress;
	bool                     reconnectNotify;
	QQueue<AstCtiCommand*>   commandQueue;
	AstCtiCommand*           lastCtiCommand;
	QTimer*                  idleTimer;
	QTimer*                  connectTimer;
	QTcpSocket*              localSocket;

private slots:
	// Signals from localSocket
	void                     socketConnected();
	void                     socketDisconnected();
	void                     socketStateChanged(const QAbstractSocket::SocketState socketState);
	void                     socketError(const QAbstractSocket::SocketError error);
	void                     receiveData();

	// Signals from timers
	void                     idleTimerElapsed();
	void                     connectTimerElapsed();
};

#endif // ASTCTITCPCLIENT_H
