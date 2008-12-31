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


#include "mainserver.h"


/*!
    Our MainServer derive from QTcpServer
*/
MainServer::MainServer(QAstCTIConfiguration *config, QObject *parent)
    : QTcpServer(parent)
{
    // Lets set local configuration struct
    this->config = config;
    if (config->qDebug) qDebug() << "In MainServer::MainServer constructor";
    // Basic init here

}

/*!
    Called by QTcpServer when a new connection is avaiable.
*/
void MainServer::incomingConnection(int socketDescriptor)
{
    if (config->qDebug) qDebug() << "In MainServer::incomingConnection(" << socketDescriptor << ")";

    ClientManager *thread = new ClientManager(this->config, socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    // Inform us about client authentications / disconnections
    connect(thread, SIGNAL(addClient(QString,ClientManager *)) , this, SLOT(addClient(QString,ClientManager *)));
    connect(thread, SIGNAL(removeClient(QString)), this, SLOT(removeClient(QString)));
    connect(thread, SIGNAL(notify(QString)), this, SLOT(clientNotify(QString)));
    thread->start();
}

void MainServer::addClient(QString exten, ClientManager *cl)
{
    mutexClientList.lock();
    if (!clients.contains(exten))
    {
        if (config->qDebug) qDebug() << "Adding exten" << exten;
        clients[exten] = cl;
    }
    else
    {
        if (config->qDebug) qDebug() << "Cannot add exten" << exten << "because is already in list";
    }
    if (config->qDebug) qDebug() << "Number of clients:" << clients.count();
    mutexClientList.unlock();
}

void MainServer::removeClient(QString exten)
{
    if (config->qDebug) qDebug() << "Removing exten" << exten;
    mutexClientList.lock();
    if (clients.contains(exten))
    {
        clients.remove(exten);
    }
    if (config->qDebug) qDebug() << "Number of clients:" << clients.count();
    mutexClientList.unlock();
}

bool MainServer::containClient(QString exten)
{

    mutexClientList.lock();
    bool retval = clients.contains(exten);
    mutexClientList.unlock();
    return retval;
}

void MainServer::clientNotify(QString data)
{
    emit dataToClient(data);
}













