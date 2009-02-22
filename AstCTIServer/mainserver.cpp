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
#include "ctiserverapplication.h"

#include <QtSql>

/*!
    Our MainServer derive from QTcpServer
*/
MainServer::MainServer(QAstCTIConfiguration *config, QObject *parent)
    : QTcpServer(parent)
{
    this->isClosing = false;
    // Lets set local configuration struct
    this->config = config;
    if (config->qDebug) qDebug() << "In MainServer:MainServer:MainServer constructor";
    // Basic init here

    /* CODE TESTING START: used for testing purpose only */

    qDebug("CODE TESTING AT %s:%d",  __FILE__ , __LINE__);

    QAstCtiOperator oper(2);
    if (oper.Load())
    {
        if (oper.getSeat() != 0)
        {
            qDebug() << "EXTEN FOR OPERATOR SEAT IS" << oper.getSeat()->getSeatExten();
        }
        qDebug() << oper.getFullName();
        oper.setLastSeat(1);
        if (oper.Save())
        {
            qDebug() << oper.getFullName() << "updated";
            if (oper.getSeat() != 0)
            {
                qDebug() << "EXTEN FOR OPERATOR SEAT NOW IS" << oper.getSeat()->getSeatExten();
            }
        }
        else
            qDebug() << oper.getFullName() << "not updated";
    }
    else
    {
        qDebug() << "Non-existent Operator";
    }
    QString sername = "ast-cti-demo";
    qDebug() << "Count now is" << services.count();
    qDebug() << "Service alive is " << services[sername]->getServiceName();
    services.removeService(sername);
    qDebug() << "Count now is" << services.count();
    /* CODE TESTING END*/
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
    connect(thread, SIGNAL(addClient(const QString,ClientManager *)) , this, SLOT(addClient(const QString,ClientManager *)));
    connect(thread, SIGNAL(changeClient(const QString,const QString)), this, SLOT(changeClient(const QString,const QString)));
    connect(thread, SIGNAL(removeClient(const QString)), this, SLOT(removeClient(const QString)));
    connect(thread, SIGNAL(notify(const QString)), this, SLOT(clientNotify(const QString)));
    connect(thread, SIGNAL(stopRequested(QString,ClientManager*)), this, SLOT(closeServer(const QString,ClientManager *)));
    thread->start();
}

void MainServer::addClient(const QString &exten, ClientManager *cl)
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

void MainServer::removeClient(const QString &exten)
{
    if (config->qDebug) qDebug() << "Removing exten" << exten;
    mutexClientList.lock();
    if (clients.contains(exten))
    {
        clients.remove(exten);
    }
    if (config->qDebug) qDebug() << "Number of clients:" << clients.count();
    mutexClientList.unlock();

    if ((this->isClosing) & (clients.count() == 0) )
    {
        if (config->qDebug) qDebug() << "Clients are 0 and we're asked to close";
        this->close();


    }
}
void MainServer::changeClient(const QString &oldexten,const QString &newexten)
{
    if (config->qDebug) qDebug() << "Changing client exten from" << oldexten << "to" << newexten;
    mutexClientList.lock();
    if (clients.contains(oldexten))
    {
        ClientManager *cl = clients.value(oldexten);
        clients.remove(oldexten);
        clients[newexten] = cl;
    }

    mutexClientList.unlock();
}

bool MainServer::containClient(const QString &exten)
{

    mutexClientList.lock();
    bool retval = clients.contains(exten);
    mutexClientList.unlock();
    return retval;
}

void MainServer::clientNotify(const QString &data)
{
    emit dataToClient(data);
}

void MainServer::closeServer(const QString &exten, ClientManager *cl)
{
    Q_UNUSED(exten);
    Q_UNUSED(cl);

    this->isClosing = true;
    qDebug() << "Received STOP signal";

    QMutableHashIterator<QString, ClientManager*> i(this->clients);
    while (i.hasNext()) {
        i.next();
        qDebug() << "Destroiyng client" << i.key();
        i.value()->stop();

    }
    // After removed all client, exit the application.
    CtiServerApplication::instance()->exit(0);
}
