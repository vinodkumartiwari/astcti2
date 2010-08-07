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
#include <QVector>

/*!
    Our CoreTcpServer derive from QTcpServer
*/
CoreTcpServer::CoreTcpServer(QAstCTIConfiguration *config, QObject *parent)
    : QTcpServer(parent), actionId(0)
{
    this->clients = new QHash <QString,ClientManager*>;
    this->users = new QList<QString>;
    this->isClosing = false;
    // Lets set local configuration struct
    this->config = config;
    if (config->qDebug) qDebug() << "In CoreTcpServer:CoreTcpServer()";
    // Basic init here

     /* CODE TESTING START */
    this->amiClient = new AMIClient(this->config, this );
    qRegisterMetaType<AMIEvent>("AMIEvent" );
    connect(this->amiClient, SIGNAL(amiConnectionStatusChange(AMIConnectionStatus)), this, SLOT(amiConnectionStatusChange(AMIConnectionStatus)),Qt::QueuedConnection);
    connect(this->amiClient, SIGNAL(ctiEvent(const AMIEvent, QAstCTICall*)), this, SLOT(receiveCtiEvent(const AMIEvent, QAstCTICall*)));
    connect(this->amiClient, SIGNAL(ctiResponse(int, AsteriskCommand*)), this, SLOT(receiveCtiResponse(int, AsteriskCommand*)));
    this->amiClient->start();

    //5f4dcc3b5aa765d61d8327deb882cf99
    /* CODE TESTING END*/

}


CoreTcpServer::~CoreTcpServer()
{
    if (this->config->qDebug) qDebug() << "In CoreTcpServer::~CoreTcpServer()";
    if (this->clients != 0) delete(this->clients);
    if (this->users != 0) delete(this->users);
    if (this->amiClient != 0) delete(this->amiClient);
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
    connect(this, SIGNAL(amiClientDisconnected()), thread, SLOT(disconnectRequest()));

    connect(thread, SIGNAL(addClient(const QString,ClientManager *)) , this, SLOT(addClient(const QString,ClientManager *)));
    connect(thread, SIGNAL(changeClient(const QString,const QString)), this, SLOT(changeClient(const QString,const QString)));
    connect(thread, SIGNAL(removeClient(const QString)), this, SLOT(removeClient(const QString)));
    connect(thread, SIGNAL(notifyServer(const QString)), this, SLOT(notifyClient(const QString)));


    // Connect signals to inform us about pause and login / logout
    connect(thread, SIGNAL(ctiLogin(ClientManager *)), this, SLOT(ctiClientLogin(ClientManager *)));
    connect(thread, SIGNAL(ctiLogoff(ClientManager *)), this, SLOT(ctiClientLogoff(ClientManager *)));
    connect(thread, SIGNAL(ctiPauseIn(ClientManager *)) , this, SLOT(ctiClientPauseIn(ClientManager *)));
    connect(thread, SIGNAL(ctiPauseOut(ClientManager *)) , this, SLOT(ctiClientPauseOut(ClientManager *)));

    // Connect signals to inform back the client of pause request results
    connect(this, SIGNAL(ctiClientPauseInResult(QString,bool,QString)), thread, SLOT(pauseInResult(QString,bool,QString)));
    connect(this, SIGNAL(ctiClientPauseOutResult(QString,bool,QString)), thread, SLOT(pauseOutResult(QString,bool,QString)));
    connect(this, SIGNAL(ctiResponse(QString,int,QString,QString,QString)), thread, SLOT(ctiResponse(QString,int,QString,QString,QString)));

    // This signal will notify client manager the wait condition on cti logoff before thread exit
    connect(this, SIGNAL(ctiClientLogoffSent()), thread, SLOT(unlockAfterSuccessfullLogoff()));
    thread->start();
}

void CoreTcpServer::amiConnectionStatusChange(AMIConnectionStatus status)
{
    switch(status) {
    case AmiConnectionStatusConnected:
        // Say we're listening on port..
        if (!this->isListening()) {
            if (config->qDebug) qDebug() << "CoreTcpServer listening on port " <<  this->config->ctiServerPort;
            this->listen(QHostAddress::Any, this->config->ctiServerPort );
        } else {
            if (config->qDebug) qDebug() << "CoreTcpServer is already listening...";
        }
        break;
    case AmiConnectionStatusDisconnected:        
        if (this->isListening()) {
            if (config->qDebug) qDebug() << "CoreTcpServer stop listening..";
            this->close();
            emit this->amiClientDisconnected();
        }
        break;
    case AmiConnectionStatusEndList:
        break;
    }
}

bool CoreTcpServer::containsUser(const QString &username)
{
    return this->users->contains(username);

}
void CoreTcpServer::addUser(const QString &username)
{
    mutexUsersList.lock();
    if (!containsUser(username)) {        
        this->users->append(username);        
    }
    mutexUsersList.unlock();
}
void CoreTcpServer::removeUser(const QString &username)
{
    mutexUsersList.lock();
    if (containsUser(username)) {
        this->users->removeAll(username);
    }
    mutexUsersList.unlock();
}

void CoreTcpServer::addClient(const QString &exten, ClientManager *cl)
{
    mutexClientList.lock();
    if (!clients->contains(exten))  {
        if (config->qDebug) qDebug() << "Adding exten" << exten;
        clients->insert(exten, cl);
    } else {
        if (config->qDebug) qDebug() << "Cannot add exten" << exten << "because is already in list";
    }
    if (config->qDebug) qDebug() << "Number of clients:" << clients->count();
    mutexClientList.unlock();
}

void CoreTcpServer::removeClient(const QString &exten)
{
    if (config->qDebug) qDebug() << "Removing exten" << exten;
    mutexClientList.lock();
    if (clients->contains(exten)) {
        clients->remove(exten);
    } else {
        if (config->qDebug) qDebug() << "Cannot find client:" << exten;
    }
    int clientCount = clients->count();
    if (config->qDebug) qDebug() << "Number of clients:" << clientCount;
    if (clientCount == 0) {
        this->clients->squeeze();
    }

    mutexClientList.unlock();

    if ((this->isClosing) & (clients->count() == 0) ) {
        if (config->qDebug) qDebug() << "Clients are now 0 and we were asked to close";
        this->close();
    }
}
void CoreTcpServer::changeClient(const QString &oldExten,const QString &newExten)
{
    if (config->qDebug) qDebug() << "Changing client exten from" << oldExten << "to" << newExten;
    mutexClientList.lock();
    if (clients->contains(oldExten)) {
        ClientManager *cl = clients->take(oldExten);
        clients->insert(newExten, cl);
    }
    mutexClientList.unlock();
}

bool CoreTcpServer::containsClient(const QString &exten)
{

    mutexClientList.lock();
    bool retVal = clients->contains(exten);
    mutexClientList.unlock();
    return retVal;
}

void CoreTcpServer::notifyClient(const QString &data)
{
    emit sendDataFromServer(data);
}


void CoreTcpServer::stopTheServer(bool closeTheSocket)
{
    this->isClosing = true;
    qDebug() << "Received STOP signal";
    emit(this->serverIsClosing());

    if (closeTheSocket) {
        this->close();
    }
    
    CtiServerApplication::instance()->exit(0);
}

void CoreTcpServer::receiveCtiEvent(const AMIEvent &eventId, QAstCTICall *theCall)
{
    QString logMsg("");

    if (theCall != 0) {
        QString callChannel = theCall->getParsedDestChannel();

        logMsg = "for call " + theCall->getUniqueId();
        if (callChannel.length() > 0) {
            QString identifier = QString("exten-%1").arg(callChannel);

            if (eventId==AmiEventBridge) {
                mutexClientList.lock();
                if (clients->contains(identifier)) {
                    ClientManager* client = clients->value(identifier);
                    if (client != 0) {
                        QString clientOperatingSystem = client->getClientOperatingSystem();
                        if (clientOperatingSystem != "") {
                            // Here we add information about CTI application to start
                            theCall->setOperatingSystem( clientOperatingSystem );
                            QString xmlData = theCall->toXml();
                            client->sendDataToClient(xmlData);
                        }
                    } else {
                        qDebug() << ">> receiveCtiEvent << Client is null";
                    }
                } else {
                    qDebug() << ">> receiveCtiEvent << Identifier" << identifier << "not found in client list";
                }
                mutexClientList.unlock();
            }
        } else {
            qDebug() << ">> receiveCtiEvent << Call Channel is empty";
        }
    }
    qDebug() << "Received CTI Event" << eventId << logMsg;
}

void CoreTcpServer::receiveCtiResponse(const int &actionId, AsteriskCommand *cmd)
{
    QString identifier = QString("exten-%1").arg(cmd->channel);

    if (clients->contains(identifier)) {        
        QString commandName = cmd->commandName;
        QString responseString = cmd->responseString;
        QString responseMessage = cmd->responseMessage;
        if (commandName == "QueuePause") {
            bool result = (responseString.toLower() == "success");
            mutexClientList.lock();
            ClientManager* client = clients->value(identifier);
            mutexClientList.unlock();
            if (client != 0) {
                if (client->getState() == StatePauseInRequested) {
                    emit this->ctiClientPauseInResult(identifier, result, responseMessage);
                } else if (client->getState() == StatePauseOutReuqested) {
                    emit this->ctiClientPauseOutResult(identifier, result, responseMessage);
                }
            } else {
                qDebug() << ">> receiveCtiResponse << Identifier" << identifier << "not found in client list";
            }
        } else {
            emit this->ctiResponse(identifier, actionId, commandName, responseString, responseMessage);
        }
    }
    // Free resources
    delete(cmd);
}

void CoreTcpServer::ctiClientLogin(ClientManager *cl)
{
    QAstCTIOperator *theOperator = cl->getActiveOperator();
    if (theOperator == 0) {
        return;
    }
    QAstCTISeat *theSeat = theOperator->getLastSeat();
    if (theSeat == 0) {
        return;
    }
    QHash<QString,int> *servicesList = theOperator->getListOfServices();
    if (servicesList == 0) {
        return;
    }

    QHash<QString, int>::const_iterator i = servicesList->constBegin();
    while (i != servicesList->constEnd()) {
        QString serviceName = i.key();

        // Let's give the pointer a restricted range of life
        {
            // Here we get the service object from CtiServerApplication instance
            QAstCTIService *theService = 0;
            theService = CtiServerApplication::instance()->getServices()->operator [](serviceName);

            // Check if we got an existent service.. we should even!
            if (theService != 0) {
                QString beginInPause = (theOperator->getBeginInPause() ? "true" : "false");
                // Get the right interface for the operator fromit's seat
                QString interface = theSeat->getSeatExten();
                int penalty = i.value();

                // We can send a login if AMIClient is connected
                // and the service is a queue where we can login
                if (this->amiClient->isConnected() & (theService->getServiceIsQueue()) ) {
                    QString cmd = QString("")
                                  .append("Action: QueueAdd\r\n")
                                  .append("ActionID: %1\r\n")
                                  .append("Queue: %2\r\n")
                                  .append("Interface: %3\r\n")
                                  .append("Penalty: %4\r\n")
                                  .append("Paused: %5\r\n\r\n")
                                  .arg(this->incrementAndGetActionId())
                                  .arg(theService->getServiceQueueName())
                                  .arg(interface)
                                  .arg(penalty)
                                  .arg(beginInPause);

                    this->amiClient->sendCommandToAsterisk(actionId,"QueueAdd", cmd, interface);

                }
            }
        }
        ++i;
    }
}

void CoreTcpServer::ctiClientLogoff(ClientManager *cl)
{
    QAstCTIOperator *theOperator = cl->getActiveOperator();
    if (theOperator == 0) return;
    QAstCTISeat *theSeat = theOperator->getLastSeat();
    if (theSeat == 0) return;
    QHash<QString,int> *servicesList = theOperator->getListOfServices();
    if (servicesList == 0) return;

    QHash<QString, int>::const_iterator i = servicesList->constBegin();
    while (i != servicesList->constEnd()) {
        QString serviceName = i.key();

        // Let's give the pointer a restricted range of life
        {
            // Here we get the service object from CtiServerApplication instance
            QAstCTIService *theService = 0;
            theService = CtiServerApplication::instance()->getServices()->operator [](serviceName);

            // Check if we got an existent service.. we should even!
            if (theService != 0) {
                QString begin_in_pause = (theOperator->getBeginInPause() ? "true" : "false");
                // Get the right interface for the operator fromit's seat
                QString interface = theSeat->getSeatExten();

                // We can send a login if AMIClient is connected
                // and the service is a queue where we can login
                if (this->amiClient->isConnected() & (theService->getServiceIsQueue()) ) {
                    QString cmd = QString("")
                                  .append("Action: QueueRemove\r\n")
                                  .append("ActionID: %1\r\n")
                                  .append("Queue: %2\r\n")
                                  .append("Interface: %3\r\n\r\n")
                                  .arg(this->incrementAndGetActionId())
                                  .arg(theService->getServiceQueueName())
                                  .arg(interface);

                    this->amiClient->sendCommandToAsterisk(actionId,"QueueRemove", cmd, interface);

                    emit this->ctiClientLogoffSent();
                }
            }
        }
        ++i;
    }
}

void CoreTcpServer::ctiClientPauseIn(ClientManager* cl)
{
    QAstCTIOperator *theOperator = cl->getActiveOperator();
    if (theOperator == 0) return;
    QAstCTISeat *theSeat = theOperator->getLastSeat();
    if (theSeat == 0) return;

    // Get the right interface for the operator fromit's seat
    QString interface = theSeat->getSeatExten();

    // We can send a login if AMIClient is connected
    // and the service is a queue where we can login
    if (this->amiClient->isConnected() ) {
        QString cmd = QString("")
                      .append("Action: QueuePause\r\n")
                      .append("ActionID: %1\r\n")
                      .append("Interface: %2\r\n")
                      .append("Paused: %3\r\n\r\n")
                      .arg(this->incrementAndGetActionId())
                      .arg(interface)
                      .arg("true");

        this->amiClient->sendCommandToAsterisk(actionId,"QueuePause", cmd, interface);
    }

}

void CoreTcpServer::ctiClientPauseOut(ClientManager* cl)
{
    QAstCTIOperator *theOperator = cl->getActiveOperator();
    if (theOperator == 0) return;
    QAstCTISeat *theSeat = theOperator->getLastSeat();
    if (theSeat == 0) return;

    // Get the right interface for the operator fromit's seat
    QString interface = theSeat->getSeatExten();

    // We can send a login if AMIClient is connected
    // and the service is a queue where we can login
    if (this->amiClient->isConnected() ) {
        QString cmd = QString("")
                      .append("Action: QueuePause\r\n")
                      .append("ActionID: %1\r\n")
                      .append("Interface: %2\r\n")
                      .append("Paused: %3\r\n\r\n")
                      .arg(this->incrementAndGetActionId())
                      .arg(interface)
                      .arg("false");

        this->amiClient->sendCommandToAsterisk(actionId,"QueuePause", cmd, interface);
    }
}

int CoreTcpServer::incrementAndGetActionId() {
    this->actionId++;
    return this->actionId;
}
