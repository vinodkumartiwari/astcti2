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

#ifndef CORETCPSERVER_H
#define CORETCPSERVER_H

#include <QObject>
#include <QThread>
#include <QtNetwork>
#include <QHash>
#include <QStringList>

#include "clientmanager.h"
#include "astcticonfiguration.h"
#include "amiclient.h"

// Server responses
#define RSP_OK             "OK"
#define RSP_ERR            "KO"

// Server error codes
#define ERR_UNKNOWN_CMD    "900"
#define ERR_WRONG_PARAM    "901"
#define ERR_NO_AUTH        "902"

#define ERR_USER_LOGGED_IN "910"
#define ERR_WRONG_CREDS    "911"
#define ERR_UNKNOWN_OS     "912"
#define ERR_WRONG_MAC      "913"
#define ERR_SEAT_TAKEN     "914"
#define ERR_PASS_CHG_FAIL  "915"

#define ERR_UNKNOWN_CHAN   "920"
#define ERR_UNKNOWN_QUEUE  "921"

enum AstCtiCommands {
	CmdNotDefined,
	CmdLogin,
	CmdKeepAlive,
	CmdChangePassword,
	CmdOriginate,
	CmdConference,
	CmdStart,
	CmdPause,
	CmdQuit
};

struct AstCtiCommand
{
	AstCtiCommands command;
	QStringList parameters;
};

class CoreTcpServer : public QTcpServer
{
    Q_OBJECT
	Q_ENUMS(AstCtiCommands)

public:
	explicit CoreTcpServer(AstCtiConfiguration* const config, QObject* parent = 0);
    ~CoreTcpServer();

	void setConfig(AstCtiConfiguration* const newConfig);

public slots:
	void stopServer();

signals:
	//AmiClient
	void newAmiConfiguration(AstCtiConfiguration* config);
	void newAmiCommand(AmiCommand* command);

	//TODO
	void serverIsClosing();

private:
	Q_DISABLE_COPY(CoreTcpServer)

	AstCtiCommand                       parseCommand(const QString& command);
	AstCtiCommands                      parseCommandType(const QString& commandName);
	bool                                startListening();
	void                                stopListening();
	bool                                containsUser(const QString& username);
	void                                addUser(const QString& username);
	void                                removeUser(const QString& username);
	void                                processClientData(QTcpSocket* const socket,
														  const QString& data);
	void                                sendDataToClient(QTcpSocket* const socket,
														 const QString& data);
	void                                addClient(ClientManager* const cm);
	void                                removeClient(ClientManager* const cm);
	void                                disconnectClients();
	ClientManager*                      getClientByChannelName(const QString& channelName);
	void                                agentLogin(ClientManager* const cm,
												   const QString& channelName,
												   const QString& queue);
	void                                agentLogout(ClientManager* const cm);
	void                                agentPause(ClientManager* const cm,
												   const QString& channelName,
												   const QString& queue);
	void                                sendAmiCommand(AmiCommand* const cmd);

	AstCtiConfiguration*                config;
	AmiClient*                          amiClient;
	QHash<QTcpSocket*, ClientManager*>  clients;
	QStringList                         users;
	bool                                isClosing;
	AmiConnectionStatus                 amiStatus;

private slots:
	//QTcpSocket
	void connectionInitiated();
	void socketDataReceived();
	void socketDisconnected();
	void socketError(QAbstractSocket::SocketError error);

	//AmiClient
	void handleAmiConnectionStatusChange(AmiConnectionStatus status);
	void handleAmiChannelEvent(AmiEvent eventId, AstCtiChannel* channel);
	void handleAmiStatusEvent(AmiEvent eventId, QString channelName, QString data, int status);
	void handleAmiCommandFailed(AmiAction action, QString channelName);
};

#endif // CORETCPSERVER_H
