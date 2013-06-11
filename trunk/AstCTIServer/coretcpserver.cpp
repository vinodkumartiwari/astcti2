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

/*!
    Our CoreTcpServer derive from QTcpServer
*/
CoreTcpServer::CoreTcpServer(bool debug, AstCtiConfiguration *config, QObject *parent)
	: QTcpServer(parent)
{
	this->clients = new QHash<QString, ClientManager*>;
    this->users = new QList<QString>;
    this->isClosing = false;
	this->debug = debug;
    this->config = config;

	//qRegisterMetaType<AmiEvent>("AMIEvent");

	this->amiClient = new AmiClient(this->debug, this->config);
	connect(this->amiClient, SIGNAL(amiConnectionStatusChange(const AmiConnectionStatus)),
			this, SLOT(amiConnectionStatusChange(const AmiConnectionStatus)));
	connect(this->amiClient, SIGNAL(ctiEvent(const AmiEvent, AstCtiCall*)),
			this, SLOT(receiveCtiEvent(const AmiEvent, AstCtiCall*)));
	connect(this->amiClient, SIGNAL(ctiResponse(AmiCommand*)),
			this, SLOT(receiveCtiResponse(AmiCommand*)));
	connect(this, SIGNAL(newAmiCommand(AmiCommand*)),
			this->amiClient, SLOT(sendCommandToAsterisk(AmiCommand*)));

	QThread *amiThread = new QThread(this);
	this->amiClient->moveToThread(amiThread);
	connect(amiThread, SIGNAL(started()),
			this->amiClient, SLOT(run()));
	connect(amiThread, SIGNAL(finished()),
			this->amiClient, SLOT(deleteLater()));
	amiThread->start();
}

CoreTcpServer::~CoreTcpServer()
{
	this->amiClient->stop();
	// amiClient will be deleted automatically because the thread's
	// finished() signal is connected to amiClient's deleteLater() slot
	QThread *amiThread = this->amiClient->thread();
	amiThread->quit();
	amiThread->wait();
	delete amiThread;

	foreach(const QString &key, this->clients->keys()) {
		ClientManager *cm = this->clients->value(key);
		// ClientManager will be deleted automatically because the thread's
		// finished() signal is connected to ClientManager's deleteLater() slot
		QThread *cmThread = cm->thread();
		cmThread->quit();
		cmThread->wait();
		delete cmThread;
	}
	delete this->clients;

	delete this->users;
}

/*!
    Called by QTcpServer when a new connection is avaiable.
*/
void CoreTcpServer::incomingConnection(int socketDescriptor)
{
	if (this->debug)
		qDebug() << "In CoreTcpServer::incomingConnection(" << socketDescriptor << ")";

	ClientManager* cm = new ClientManager(this->debug, this->config, socketDescriptor);

    // Inform us about client authentications / disconnections
	connect(this, SIGNAL(amiClientDisconnected()),
			cm, SLOT(disconnectRequest()));

	connect(this, SIGNAL(sendDataFromServer(QString)),
			cm, SLOT(sendDataToClient(QString)));

	connect(cm, SIGNAL(addClient(const QString, ClientManager*)),
			this, SLOT(addClient(const QString, ClientManager*)));
	connect(cm, SIGNAL(changeClient(const QString, const QString)),
			this, SLOT(changeClient(const QString, const QString)));
	connect(cm, SIGNAL(removeClient(const QString)),
			this, SLOT(removeClient(const QString)));
	connect(cm, SIGNAL(notifyServer(const QString)),
			this, SLOT(notifyClient(const QString)));

    // Connect signals to inform us about pause and login / logout
	connect(cm, SIGNAL(ctiLogin(ClientManager*)),
			this, SLOT(ctiClientLogin(ClientManager*)));
	connect(cm, SIGNAL(ctiLogoff(ClientManager*)),
			this, SLOT(ctiClientLogoff(ClientManager*)));
	connect(cm, SIGNAL(ctiPauseIn(ClientManager*)),
			this, SLOT(ctiClientPauseIn(ClientManager*)));
	connect(cm, SIGNAL(ctiPauseOut(ClientManager*)),
			this, SLOT(ctiClientPauseOut(ClientManager*)));

    // Connect signals to inform back the client of pause request results
	connect(this, SIGNAL(ctiClientPauseInResult(QString, bool, QString)),
			cm, SLOT(pauseInResult(QString, bool, QString)));
	connect(this, SIGNAL(ctiClientPauseOutResult(QString, bool, QString)),
			cm, SLOT(pauseOutResult(QString, bool, QString)));
	connect(this, SIGNAL(ctiResponse(QString, QString, QString, QString)),
			cm, SLOT(ctiResponse(QString, QString, QString, QString)));

    // This signal will notify client manager the wait condition on cti logoff before thread exit
	connect(this, SIGNAL(ctiClientLogoffSent()),
			cm, SLOT(unlockAfterSuccessfullLogoff()));

	QThread *cmThread = new QThread(this);
	cm->moveToThread(cmThread);
	connect(cmThread, SIGNAL(started()),
			cm, SLOT(run()));
	connect(cmThread, SIGNAL(finished()),
			cm, SLOT(deleteLater()));
	cmThread->start();
}

void CoreTcpServer::amiConnectionStatusChange(const AmiConnectionStatus status)
{
    switch(status) {
    case AmiConnectionStatusConnected:
		/* CODE TESTING START */
		//this->stopServer(true);
		//return;
		/* CODE TESTING END */

        // Say we're listening on port..
        if (!this->isListening()) {
			if (this->debug)
				qDebug() << "CoreTcpServer listening on socket"
						 << this->config->ctiServerAddress << ":" << this->config->ctiServerPort;
			QHostAddress bindAddress;
			if (!bindAddress.setAddress(this->config->ctiServerAddress)) {
				bindAddress = QHostAddress::Any;
			}

			this->listen(bindAddress, this->config->ctiServerPort);
        } else {
			if (this->debug)
				qDebug() << "CoreTcpServer is already listening on socket"
						 << this->config->ctiServerAddress << ":" << this->config->ctiServerPort;
		}
        break;
    case AmiConnectionStatusDisconnected:        
        if (this->isListening()) {
			if (this->debug)
				qDebug() << "CoreTcpServer stopped listening on socket"
						 << this->config->ctiServerAddress << ":" << this->config->ctiServerPort;
			this->close();
            emit this->amiClientDisconnected();
        }
        break;
    }
}

bool CoreTcpServer::containsUser(const QString &username)
{
	mutexUsersList.lock();
	bool retVal = this->users->contains(username);
	mutexUsersList.unlock();
	return retVal;
}

void CoreTcpServer::addUser(const QString &username)
{
	if (!containsUser(username)) {
		mutexUsersList.lock();
		this->users->append(username);
		mutexUsersList.unlock();
	}
}

void CoreTcpServer::removeUser(const QString &username)
{
    if (containsUser(username)) {
		mutexUsersList.lock();
		this->users->removeAll(username);
		mutexUsersList.unlock();
	}
}

void CoreTcpServer::addClient(const QString &exten, ClientManager *cl)
{
    mutexClientList.lock();
	if (!this->clients->contains(exten))  {
		if (this->debug)
			qDebug() << "Adding exten" << exten;
		this->clients->insert(exten, cl);
    } else {
		if (this->debug)
			qDebug() << "Cannot add exten" << exten << "because it is already in the list";
    }
	if (this->debug)
		qDebug() << "Number of clients:" << this->clients->count();
    mutexClientList.unlock();
}

void CoreTcpServer::removeClient(const QString &exten)
{
	if (this->debug)
		qDebug() << "Removing exten" << exten;

	mutexClientList.lock();

	if (this->clients->contains(exten))
		this->clients->remove(exten);
	else
		if (this->debug)
			qDebug() << "Cannot find client:" << exten;

	int clientCount = clients->count();
	if (this->debug)
		qDebug() << "Number of clients:" << clientCount;
	if (clientCount == 0)
        this->clients->squeeze();

    mutexClientList.unlock();

	if ((this->isClosing) && (clientCount == 0)) {
		if (this->debug)
			qDebug() << "Clients are now 0 and we were asked to close";
        this->close();
    }
}

void CoreTcpServer::changeClient(const QString &oldExten, const QString &newExten)
{
	if (this->debug)
		qDebug() << "Changing client exten from" << oldExten << "to" << newExten;
    mutexClientList.lock();
	if (this->clients->contains(oldExten)) {
		ClientManager *cl = this->clients->take(oldExten);
		this->clients->insert(newExten, cl);
    }
    mutexClientList.unlock();
}

bool CoreTcpServer::containsClient(const QString &exten)
{
    mutexClientList.lock();
	bool retVal = this->clients->contains(exten);
    mutexClientList.unlock();
    return retVal;
}

void CoreTcpServer::notifyClient(const QString &data)
{
    emit sendDataFromServer(data);
}

void CoreTcpServer::stopServer(bool closeSocket)
{
	// TODO : There is currently no way to call this method
	// TODO : A mechanism to close the application must be implemented, maybe using QtService

	this->isClosing = true;
	if (this->debug)
		qDebug() << "Received STOP signal";
	emit this->serverIsClosing();

	if (closeSocket)
        this->close();
    
	CtiServerApplication::instance()->quit();
}

void CoreTcpServer::receiveCtiEvent(const AmiEvent &eventId, AstCtiCall *call)
{
	QString logMsg = "";

	if (call != 0) {
		QString callChannel = call->getParsedDestChannel();

		logMsg = "for call " + call->getUniqueId();
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
							call->setOperatingSystem(clientOperatingSystem);
							QString xmlData = call->toXml();
                            client->sendDataToClient(xmlData);
                        }
                    } else {
						if (this->debug)
							qDebug() << ">> receiveCtiEvent << Client is null";
                    }
                } else {
					if (this->debug)
						qDebug() << ">> receiveCtiEvent << Identifier" << identifier
								 << "not found in client list";
                }
                mutexClientList.unlock();
            }
        } else {
			if (this->debug)
				qDebug() << ">> receiveCtiEvent << Call Channel is empty";
        }
    }

	// Call object is owned by another thread so it is unsafe to delete it here.
	// Instead, we invoke QObject's deleteLater() method which will
	// delete the object once the control returns to the event loop.
	call->deleteLater();

	if (this->debug)
		qDebug() << "Received CTI Event" << this->amiClient->getEventName(eventId) << logMsg;
}

void CoreTcpServer::receiveCtiResponse(AmiCommand *cmd)
{
	QString identifier = QString("exten-%1").arg(cmd->exten);

    if (clients->contains(identifier)) {        
        QString responseString = cmd->responseString;
        QString responseMessage = cmd->responseMessage;
		if (cmd->action == AmiActionQueuePause) {
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
				if (this->debug)
					qDebug() << ">> receiveCtiResponse << Identifier" << identifier
							 << "not found in client list";
            }
        } else {
			emit this->ctiResponse(identifier, this->amiClient->getActionName(cmd->action),
								   responseString, responseMessage);
        }
    }

	// Cmd object is owned by another thread so it is unsafe to delete it here.
	// Instead, we invoke QObject's deleteLater() method which will
	// delete the object once the control returns to the event loop.
	cmd->deleteLater();
}

void CoreTcpServer::ctiClientLogin(ClientManager *cm)
{
	AstCtiSeat *seat = cm->getActiveSeat();
	AstCtiOperator *op = cm->getActiveOperator();
	if (seat != 0 && op != 0) {
		QHash<AstCtiService*, int> *services = op->getServices();

		QHashIterator<AstCtiService*, int> i(*services);
		while (i.hasNext()) {
			i.next();
			AstCtiService *service = i.key();
			// Check if we got an existent service and it is a queue
			if (service != 0 && service->getServiceIsQueue()) {
				QString beginInPause = (op->getBeginInPause() ? "true" : "false");
				// Get the right interface for the operator from it's seat
				QString interface = seat->getExten();
				QString penalty = QString::number(i.value());

				AmiCommand *cmd = new AmiCommand;
				cmd->action = AmiActionQueueAdd;
				cmd->exten = interface;
				cmd->parameters = new QHash<QString, QString>;
				cmd->parameters->insert("Queue", service->getQueueName());
				cmd->parameters->insert("Interface", interface);
				cmd->parameters->insert("Penalty", penalty);
				cmd->parameters->insert("Paused", beginInPause);
				emit this->newAmiCommand(cmd);
			}
		}
	}
}

void CoreTcpServer::ctiClientLogoff(ClientManager *cm)
{
	AstCtiSeat *seat = cm->getActiveSeat();
	AstCtiOperator *op = cm->getActiveOperator();
	if (seat != 0 && op != 0) {
		QHash<AstCtiService*, int> *services = op->getServices();

		QHashIterator<AstCtiService*, int> i(*services);
		while (i.hasNext()) {
			i.next();
			AstCtiService *service = i.key();
			// Check if we got an existent service and it is a queue
			if (service != 0 && service->getServiceIsQueue()) {
				// Get the right interface for the operator from it's seat
				QString interface = seat->getExten();

				AmiCommand *cmd = new AmiCommand;
				cmd->action = AmiActionQueueRemove;
				cmd->exten = interface;
				cmd->parameters = new QHash<QString, QString>;
				cmd->parameters->insert("Queue", service->getQueueName());
				cmd->parameters->insert("Interface", interface);
				emit this->newAmiCommand(cmd);
			}
		}
	}

	emit this->ctiClientLogoffSent();
}

void CoreTcpServer::ctiClientPauseIn(ClientManager* cm)
{
	AstCtiSeat *seat = cm->getActiveSeat();
	AstCtiOperator *op = cm->getActiveOperator();
	if (seat != 0 && op != 0) {
		// Get the right interface for the operator from it's seat
		QString interface = seat->getExten();

		AmiCommand *cmd = new AmiCommand;
		cmd->action = AmiActionQueuePause;
		cmd->exten = interface;
		cmd->parameters = new QHash<QString, QString>;
		cmd->parameters->insert("Interface", interface);
		cmd->parameters->insert("Paused", "true");
		emit this->newAmiCommand(cmd);
	}
}

void CoreTcpServer::ctiClientPauseOut(ClientManager* cm)
{
	AstCtiSeat *seat = cm->getActiveSeat();
	AstCtiOperator *op = cm->getActiveOperator();
	if (seat != 0 && op != 0) {
		// Get the right interface for the operator from it's seat
		QString interface = seat->getExten();

		AmiCommand *cmd = new AmiCommand;
		cmd->action = AmiActionQueuePause;
		cmd->exten = interface;
		cmd->parameters = new QHash<QString, QString>;
		cmd->parameters->insert("Interface", interface);
		cmd->parameters->insert("Paused", "false");
		emit this->newAmiCommand(cmd);
	}
}
