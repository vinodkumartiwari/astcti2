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


#include "clientmanager.h"
#include "cticonfig.h"
#include "amiclient.h"

class CoreTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    CoreTcpServer(QAstCTIConfiguration *config, QObject *parent=0);
    ~CoreTcpServer();

signals:
    void                            send_data_from_server(const QString &data);
    void                            server_is_closing();
    void                            ami_client_pause_in(ClientManager* cl);
    void                            ami_client_pause_out(ClientManager* cl);
    void                            ami_client_login(ClientManager* cl);
    void                            ami_client_logoff(ClientManager* cl);

protected:
    QHash<QString, ClientManager*>* clients;
    void                            incomingConnection(int socketDescriptor);
    bool                            contain_client(const QString &exten);


protected slots:
    void                            add_client(const QString& exten, ClientManager* cl);
    void                            change_client(const QString& oldexten, const QString& newexten);
    void                            remove_client(const QString& exten);
    void                            notify_client(const QString& data);
    void                            stop_the_server();
    void                            stop_the_server(bool close_the_socket);
    // TODO: Complete slot declaration
    void                            receive_cti_event(const AMIEvent& eventid, QAstCTICall* the_call);
    // Slots to receive CTI Client events
    void                            cti_client_pause_in(ClientManager* cl);
    void                            cti_client_pause_out(ClientManager* cl);
    void                            cti_client_login(ClientManager* cl);
    void                            cti_client_logoff(ClientManager* cl);

private:
    QSettings               settings;
    QAstCTIConfiguration*   config;
    QMutex                  mutexClientList;
    bool                    isClosing;
    AMIClient*              ct;

};


#endif // CORETCPSERVER_H
