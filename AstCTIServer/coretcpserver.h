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

#ifndef CORETCPSERVER_H
#define CORETCPSERVER_H

#include <QObject>
#include <QMutex>
#include <QtNetwork>
#include <QSettings>
#include <QHash>
#include <QList>

#include "clientmanager.h"
#include "cticonfig.h"
#include "amiclient.h"

class CoreTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    CoreTcpServer(QAstCTIConfiguration *config, QObject *parent=0);
    ~CoreTcpServer();
    bool                            containsUser(const QString &username);

public slots:
    void                            addUser(const QString &username);
    void                            removeUser(const QString &username);

signals:
    void                            sendDataFromServer(const QString &data);
    void                            serverIsClosing();
    void                            amiClientPauseIn(ClientManager *cl);
    void                            amiClientPauseOut(ClientManager *cl);
    void                            amiClientLogin(ClientManager *cl);
    void                            amiClientLogoff(ClientManager *cl);
    void                            ctiClientLogoffSent();
    void                            ctiClientPauseInResult(const QString &identifier, const bool &result, const QString &reason);
    void                            ctiClientPauseOutResult(const QString &identifier, const bool &result, const QString &reason);
    void                            ctiResponse(const QString &identifier, const int actionId, const QString &commandName, const QString &responseString, const QString &responseMessage);

protected:
    QHash<QString, ClientManager*>  *clients;
    QList<QString>                  *users;
    void                            incomingConnection(int socketDescriptor);    
    bool                            containsClient(const QString &exten);

protected slots:
    void                            addClient(const QString &exten, ClientManager *cl);
    void                            changeClient(const QString &oldexten, const QString &newexten);
    void                            removeClient(const QString &exten);
    void                            notifyClient(const QString &data);
    void                            stopTheServer();
    void                            stopTheServer(bool closeTheSocket);
    void                            receiveCtiEvent(const AMIEvent &eventid, QAstCTICall *the_call);
    void                            receiveCtiResponse(const int &actionId, AsteriskCommand *the_command);
    // Slots to receive CTI Client events
    void                            ctiClientPauseIn(ClientManager *cl);
    void                            ctiClientPauseOut(ClientManager *cl);
    void                            ctiClientLogin(ClientManager *cl);
    void                            ctiClientLogoff(ClientManager *cl);

private:
    QSettings               settings;
    QAstCTIConfiguration*   config;
    QMutex                  mutexClientList;
    QMutex                  mutexUsersList;
    bool                    isClosing;
    AMIClient*              amiClient;
    int                     actionId;
    int                     incrementAndGetActionId();


};


#endif // CORETCPSERVER_H
