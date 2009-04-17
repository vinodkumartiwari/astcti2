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


#include "coretcpserver.h"
#include "ctiserverapplication.h"


#include <QtSql>

/*!
    Our CoreTcpServer derive from QTcpServer
*/
CoreTcpServer::CoreTcpServer(QAstCTIConfiguration *config, QObject *parent)
    : QTcpServer(parent)
{
    this->clients = new QHash <QString,ClientManager*>;

    this->isClosing = false;
    // Lets set local configuration struct
    this->config = config;
    if (config->qDebug) qDebug() << "In CoreTcpServer:CoreTcpServer()";
    // Basic init here

     /* CODE TESTING START */
    this->ct = new AMIClient(this->config, 0 );
    connect(this->ct, SIGNAL(ami_client_noretries()), this, SLOT(stop_the_server()));
    connect(this->ct, SIGNAL(cti_event(const AMIEvent, QAstCTICall*)), this, SLOT(receive_cti_event(const AMIEvent, QAstCTICall*)));
    this->ct->start();

    //e3928a3bc4be46516aa33a79bbdfdb08
    /* CODE TESTING END*/
}


CoreTcpServer::~CoreTcpServer()
{
    if (this->config->qDebug) qDebug() << "In CoreTcpServer::~CoreTcpServer()";
    if (this->clients != 0) delete(this->clients);
    if (this->ct != 0) delete(this->ct);
}

/*!
    Called by QTcpServer when a new connection is avaiable.
*/
void CoreTcpServer::incomingConnection(int socketDescriptor)
{
    if (config->qDebug) qDebug() << "In CoreTcpServer::incomingConnection(" << socketDescriptor << ")";

    ClientManager* thread = new ClientManager(this->config, socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    // Inform us about client authentications / disconnections
    connect(thread, SIGNAL(add_client(const QString,ClientManager *)) , this, SLOT(add_client(const QString,ClientManager*)));
    connect(thread, SIGNAL(change_client(const QString,const QString)), this, SLOT(change_client(const QString,const QString)));
    connect(thread, SIGNAL(remove_client(const QString)), this, SLOT(remove_client(const QString)));
    connect(thread, SIGNAL(notify_server(const QString)), this, SLOT(notify_client(const QString)));
    connect(thread, SIGNAL(stop_request(QString,ClientManager*)), this, SLOT(stop_the_server()));
    thread->start();
}

void CoreTcpServer::add_client(const QString &exten, ClientManager *cl)
{
    mutexClientList.lock();
    if (!clients->contains(exten))
    {
        if (config->qDebug) qDebug() << "Adding exten" << exten;
        clients->insert(exten, cl);
    }
    else
    {
        if (config->qDebug) qDebug() << "Cannot add exten" << exten << "because is already in list";
    }
    if (config->qDebug) qDebug() << "Number of clients:" << clients->count();
    mutexClientList.unlock();
}

void CoreTcpServer::remove_client(const QString &exten)
{
    if (config->qDebug) qDebug() << "Removing exten" << exten;
    mutexClientList.lock();
    if (clients->contains(exten))
    {
        clients->remove(exten);
    }
    else
    {
        if (config->qDebug) qDebug() << "Cannot find client:" << exten;
    }
    int clientCount = clients->count();
    if (config->qDebug) qDebug() << "Number of clients:" << clientCount;
    if (clientCount == 0)
    {
        this->clients->squeeze();
    }

    mutexClientList.unlock();

    if ((this->isClosing) & (clients->count() == 0) )
    {

        if (config->qDebug) qDebug() << "Clients are now 0 and we were asked to close";
        this->close();

    }
}
void CoreTcpServer::change_client(const QString &oldexten,const QString &newexten)
{
    if (config->qDebug) qDebug() << "Changing client exten from" << oldexten << "to" << newexten;
    mutexClientList.lock();
    if (clients->contains(oldexten))
    {
        ClientManager *cl = clients->take(oldexten);
        clients->insert(newexten, cl);
    }

    mutexClientList.unlock();
}

bool CoreTcpServer::contain_client(const QString &exten)
{

    mutexClientList.lock();
    bool retval = clients->contains(exten);
    mutexClientList.unlock();
    return retval;
}

void CoreTcpServer::notify_client(const QString &data)
{
    emit send_data_from_server(data);
}

void CoreTcpServer::stop_the_server()
{
    this->stop_the_server(false);
}

void CoreTcpServer::stop_the_server(bool close_the_socket)
{
    this->isClosing = true;
    qDebug() << "Received STOP signal";
    emit(this->server_is_closing());

    if (close_the_socket)
    {
        this->close();
    }
    
    CtiServerApplication::instance()->exit(0);
}

void CoreTcpServer::receive_cti_event(const AMIEvent& eventid, QAstCTICall* the_call)
{
    QString log_msg("");
    if (the_call != 0)
    {
        log_msg = "for call" + the_call->get_uniqueid();
    }

    qDebug() << "Received CTI Event" << eventid << log_msg;
}
