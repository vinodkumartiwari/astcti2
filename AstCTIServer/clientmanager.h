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

#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include <QSemaphore>
#include <QTcpSocket>
#include <QStringList>
#include <QHash>

#include "cticonfig.h"
#include "qastctioperator.h"
#include "qastctiseat.h"

struct AstCTICommand
{
    QString command;
    QStringList parameters;
};

enum AstCTICommands {
    CmdNotDefined,
    CmdNoOp,
    CmdQuit,
    CmdCompression,
    CmdUser,
    CmdPass,
    CmdChangePassword,
    CmdMac,
    CmdKeep,
    CmdOsType,
    CmdIden,
    CmdServices,
    CmdQueues,
    CmdPause,
    CmdOrig,
	CmdStop
};

enum AstCTIClientState {
	StateLoggedOff,
    StateLoggedIn,
    StatePaused,
    StatePauseInRequested,
	StatePauseOutReuqested
};

class ClientManager : public  QObject
{
    Q_OBJECT

public:
	ClientManager(AstCTIConfiguration *config, int socketDescriptor);
    ~ClientManager();
	QAstCTIOperator     *getActiveOperator();
	AstCTIClientState    getState();
	QString              getClientOperatingSystem();
	QString              getLocalIdentifier();
	//void                 stop();

public slots:
	void                 run();

	///////////////////////////////////////////////////////////////////////////
    // Socket
	void                 socketDataReceived();
	void                 socketDisconnected();
	void                 sendDataToClient(const QString &data);
	void                 parseReceivedData(const QString &data);
    ///////////////////////////////////////////////////////////////////////////
    // CTI Server
	void                 disconnectRequest();
	void                 unlockAfterSuccessfullLogoff();
	void                 pauseInResult(const QString &identifier, const bool &result,
									   const QString& reason);
	void                 pauseOutResult(const QString &identifier, const bool &result,
										const QString& reason);
	void                 ctiResponse(const QString &identifier, const QString &actionName,
									 const QString &responseString, const QString &responseMessage);

signals:
    ///////////////////////////////////////////////////////////////////////////
    // Socket
	void                 dataReceived(const QString &data);

    ///////////////////////////////////////////////////////////////////////////
    // CTI Server
	void                 addClient(const QString &exten, ClientManager *cl);
	void                 changeClient(const QString &oldexten, const QString &newexten);
	void                 removeClient(const QString &exten);
	void                 notifyServer(const QString &data);

    ///////////////////////////////////////////////////////////////////////////
    // CTI Status change request
	void                 ctiPauseIn(ClientManager *cl);
	void                 ctiPauseOut(ClientManager *cl);
	void                 ctiLogin(ClientManager *cl);
	void                 ctiLogoff(ClientManager *cl);

private:
	AstCTIConfiguration *config;
	QAstCTIOperator     *activeOperator;
	QAstCTISeat         *activeSeat;
	AstCTIClientState    state;
	QHash<QString, int>  commandsList;
	QSemaphore           waitBeforeQuit;
	QString              clientOperatingSystem;
	QString              ctiUserName;
	QString              localIdentifier;
	//bool                 isRunning;
	bool                 authenticated;
	bool                 ctiLogoffOnClose;
	int                  socketDescriptor;
	int                  compressionLevel;
	int                  retries;
	quint16              blockSize;

	AstCTICommand        parseCommand(const QString &command);
	void                 initParserCommands();

	QTcpSocket          *localSocket;
};

#endif // CLIENTMANAGER_H
