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

#include "amiclient.h"
#include "ctiserverapplication.h"

Q_DECLARE_METATYPE(QAbstractSocket::SocketState);
Q_DECLARE_METATYPE(QAbstractSocket::SocketError);

Q_DECLARE_METATYPE(AMIEvent);

AMIClient::AMIClient(QAstCTIConfiguration *config, QObject *parent)
        : QThread(parent)

{

    this->config = config;
    this->activeCalls  = new QHash<QString, QAstCTICall*>;
    this->commandsHashtable = new QHash<int, AsteriskCommand*>;
    this->quit = false;
    this->localSocket = 0;
    this->amiClientStatus = AmiStatusNotDefined;
}

AMIClient::~AMIClient()
{
    if (config->qDebug) qDebug() << "In AMIClient::~AMIClient()";
    delete(this->localSocket);
    delete(this->activeCalls);
    delete(this->commandsHashtable);
}

void AMIClient::buildTheSocket()
{
    // Builds the socket
    this->localSocket = new QTcpSocket();


    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState" );
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError" );
    connect(this->localSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState) ), Qt::QueuedConnection);
    connect(this->localSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError) ), Qt::QueuedConnection);

    // Initialize how many times we need to restart
    if (this->config->amiReconnectRetries > 0) {
        this->retries = this->config->amiReconnectRetries;
    }    
}

void AMIClient::run()
{
    // We need to build here the socket to avoid warnings from QObject like
    // "Cannot create children for a parent that is in a different thread.
    if (this->localSocket == 0) {
        // Build the socket only once
        buildTheSocket();
    }

    forever {
        this->localSocket->connectToHost(config->amiHost, config->amiPort);

        if (!this->localSocket->waitForConnected(1500) ) { // config->amiConnectTimeout)) {
            this->retries --;
            if (this->retries == 0) {
                qDebug() << "AMI Client will now stop definitively";
                emit amiClientNoRetries();
                break;
            }
            this->amiClientStatus = AmiStatusNotDefined;

            qDebug() << "AMI Client will retry reconnect within " << this->config->amiConnectTimeout << " seconds";
            this->sleep(this->config->amiConnectTimeout);
        }
        else {
            // Reset retries
            this->retries = config->amiReconnectRetries;
            forever {
                //http://qt.gitorious.org/qt/miniak/blobs/6a994abef70012e2c0aa3d70253ef4b9985b2f20/src/corelib/kernel/qeventdispatcher_win.cpp
                while(this->localSocket->waitForReadyRead(500)) {

                    QByteArray dataFromSocket = localSocket->readAll();
                    QString receivedData = QString(dataFromSocket);
                    if (receivedData.trimmed().length() > 0) {
                        // emit this->receiveDataFromAsterisk(data_received);
                        parseDataReceivedFromAsterisk(receivedData);
                    }
                }
                if (this->localSocket->state() != QAbstractSocket::ConnectedState) {
                    break;
                }
            }
        }
    }
}

bool AMIClient::isConnected()
{
    return (this->localSocket != 0) & ( this->localSocket->state() == QAbstractSocket::ConnectedState);
}

void AMIClient::performLogin()
{
    this->amiClientStatus = AmiStatusLoggingIn;
    QString loginData = QString("Action: Login\r\nUsername: %1\r\nSecret: %2\r\n\r\n")
                            .arg(this->config->amiUser)
                            .arg(this->config->amiSecret);
    if (config->qDebug) qDebug() << loginData;
    this->sendDataToAsterisk(loginData);
}

void AMIClient::parseDataReceivedFromAsterisk(const QString& message)
{
    if (this->amiClientStatus != AmiStatusNotDefined) {
        this->dataBuffer+=message;
        if (this->dataBuffer.contains("Authentication failed")) {
            qDebug() << "Authentication failed. AMI Client will now stop definitively";
            emit amiClientNoRetries();
        }
        if (this->dataBuffer.contains("\r\n\r\n")) {
            // here we can execute actions on buffer
            this->executeActions();
        }
    } else {
        if (message.contains( "Asterisk Call Manager", Qt::CaseInsensitive  )) {
            this->performLogin();
        }        
    }
}

void AMIClient::executeActions()
{
    // do a copy of the local buffer and...
    QString localBuffer = this->dataBuffer;
    // ... then clean it!
    this->dataBuffer.clear();

    // now check our state. The parser can be invoked only
    // after we're authenticated
    switch(this->amiClientStatus) {
    case AmiStatusLoggingIn:
        if (localBuffer.contains("Message: Authentication accepted")) {
            qDebug() << "We're authenticated by AMI Server";
            this->amiClientStatus = AmiStatusLoggedIn;
        }
        else {
            this->amiClientStatus = AmiStatusNotDefined;
        }
        break;

    case AmiStatusLoggedIn:
        {
            // here we can begin evaluate AMI events
            QStringList events = localBuffer.split("\r\n\r\n");

            // we need to check if the buffer contains more than
            // one event and parse them all in the right order
            QStringList::const_iterator events_iterator;
            for (events_iterator = events.constBegin();
                events_iterator != events.constEnd();
                ++events_iterator) {
                QString eventBuffer  = (*events_iterator).toLocal8Bit().constData();
                if (eventBuffer.length() > 0) {
                    QHash<QString, QString>* evt = this->hashFromMessage(eventBuffer);
                    // we should have at least one Event key in the hash!
                    if (evt->contains("Event")) {
                        this->evaluateEvent(evt);
                    }
                    else if (evt->contains("Response")) {
                        this->evaluateResponse(evt);
                    }
                }
            }
        }
        break;

    default:
        return;

    }
}


QHash<QString, QString>* AMIClient::hashFromMessage(QString data)
{
    QHash<QString, QString>* hash = new QHash<QString, QString>;
    QStringList lines = data.split('\n');
    QStringList::const_iterator linesIterator;
    for(linesIterator = lines.constBegin(); linesIterator != lines.constEnd(); ++linesIterator) {
        QString line = (*linesIterator).toLocal8Bit().constData();
        if (line.contains(':')) {
            QStringList keyValue = line.split(':');
            if (keyValue.length() > 1) {
                QString key     = keyValue.at(0);
                QString value   = "";
                for(int i=1; i<keyValue.length(); i++) {
                    value.append(QString(keyValue.at(i)).trimmed());
                    if (i < keyValue.length()-1) value.append(": ");
                }
                hash->insert(key.trimmed() , value.trimmed());
            }
        }
    }
    return hash;
}

void AMIClient::evaluateEvent(QHash<QString, QString>* event)
{
    QString eventName = event->value("Event");
    if (config->qDebug) qDebug() << "Received event " << eventName << "In context: " << event->value("Context");

    if (eventName == "Shutdown") {
        // for the moment we can ignore shutdown event
        // because the socket will be closed from the asterisk
        // server...
        this->activeCalls->clear();

        // TODO: Add destionation and call
        emit this->ctiEvent(AmiEventShutdown, 0);
    } else if( eventName == "Newchannel" ) {
        // Build here a new asterisk call object and add it to hashtable
        qDebug() << "Building new QAstCTICall object";
        QAstCTICall *newCall = new QAstCTICall();
        QString uniqueId = event->value("Uniqueid");
        newCall->setChannel(event->value("Channel"));
        newCall->setCalleridNum(event->value("CallerIDNum"));
        newCall->setCalleridName(event->value("CallerIDName"));
        newCall->setState(event->value("ChannelStateDesc"));
        newCall->setUniqueId(uniqueId);

        this->activeCalls->insert(uniqueId, newCall);

        // TODO: Add destionation and call
        emit this->ctiEvent(AmiEventNewChannel, newCall);

        // qDebug() << "Newchannel" << unique_id << "in state: " << event->value("ChannelStateDesc");
    } else if( eventName == "Hangup" ) {
        QString unique_id = event->value("Uniqueid");
        if (this->activeCalls->contains(unique_id) )
        {
            QAstCTICall *newCall = this->activeCalls->operator [](unique_id);
            if (newCall != 0) {
                // TODO: Add destionation and call
                emit this->ctiEvent(AmiEventHangup, newCall);
            }
            this->activeCalls->remove(unique_id);

        }
        // Delete here the call from the hashtable
        qDebug() << "Hangup: " << event->value("Uniqueid");
    } else if (eventName == "Newexten") {
        QString context = event->value("Context");
        QString unique_id = event->value("Uniqueid");
        if (this->activeCalls->contains(unique_id) )
        {
            QAstCTICall *newCall = this->activeCalls->operator [](unique_id);
            if (newCall != 0) {
                newCall->setContext(context);
                qDebug() << "Call context for " << unique_id << "is now" << context;

                // TODO: Add destionation and call
                emit this->ctiEvent(AmiEventNewExten, newCall);
            }
        }

    }
    else if (eventName == "VarSet") {
        QString unique_id = event->value("Uniqueid");
        if (this->activeCalls->contains(unique_id) ) {
            QAstCTICall *newCall = this->activeCalls->operator [](unique_id);
            if (newCall != 0) {
                // let's retrieve call context
                QString context = newCall->getContext();
                QString variable= event->value("Variable");
                if (context != "") {
                    QAstCTIServices *services = CtiServerApplication::instance()->getServices();
                    QAstCTIService *service = services->operator [](context);
                    if (service != 0) {
                        // Check if the variable we're getting exists and is relevant in our configuration
                        if (service->getVariables()->contains( variable ) ) {
                            // Add the new variable to the current call
                            newCall->addVariable(variable, event->value("Value"));
                            qDebug() << "Reading variable " << variable << "with value" << event->value("Value");
                            // TODO: Add destination and call

                            emit this->ctiEvent(AmiEventVarSet, newCall);
                        } else {
                            qDebug() << "Variable " << variable << "doesn't exists in context" << context;
                        }
                    }
                }
            }
        }
    } else if (eventName == "Join") {
        qDebug() << "Channel" << event->value("Channel") << "is joining queue" << event->value("Queue");
        // TODO: Add destionation and call
        emit this->ctiEvent(AmiEventJoin,0);
    } else if (eventName == "Bridge") {
        // we come from a queue
        /*
        Event: Bridge
        Privilege: call,all
        Bridgestate: Link
        Bridgetype: core
        Channel1: SIP/200-0974a250 <- source channel
        Channel2: SIP/201-097525f8 <- dest channel
        Uniqueid1: bruno-1240133625.2 <- source uid
        Uniqueid2: bruno-1240133628.3 <- dest uid
        CallerID1: 200
        CallerID2: 200
        */

        QString unique_id = event->value("Uniqueid1");
        if (this->activeCalls->contains(unique_id) ) {
            QAstCTICall *newCall = this->activeCalls->operator [](unique_id);
            if (newCall != 0) {
                // let's retrieve call context
                QString context = newCall->getContext();
                QString variable= event->value("Variable");
                if (context != "") {
                    QAstCTIServices *services = CtiServerApplication::instance()->getServices();
                    QAstCTIService *service = services->operator [](context);
                    if (service != 0) {
                        // Here we add reference to service applications.
                        // Before the call is passed to the clientmanager
                        // will be selected the right application using client's
                        // operating system.
                        newCall->setApplications(service->getApplications());

                        // Let's say where the call is directed. The destination
                        // will let us know the client to signal.
                        newCall->setDestUniqueId(event->value("Uniqueid2"));
                        newCall->setDestChannel(event->value("Channel2"));

                        emit this->ctiEvent(AmiEventBridge,newCall);
                    }
                }
            }
        }
    }
    delete(event);
}

void AMIClient::evaluateResponse(QHash<QString, QString> *response)
{
    QString responseString  = response->value("Response");
    QString responseMessage = response->value("Message");
    int actionId = response->value("ActionID").toInt();
    if (actionId == 0) {
        qWarning() << "ActionId is set to 0 in AMIClient::evaluateResponse()";
    }
    if (this->commandsHashtable->contains(actionId)) {
        AsteriskCommand *cmd = this->commandsHashtable->value(actionId);
        if (cmd != 0) {
            QString channel = cmd->channel;
            QString commandName = cmd->commandName;

            qDebug() << "Evaluated response " << responseString << ". For channel:" << channel << ".ActionId:"<< actionId << ".Cause:"<< responseMessage;;
            cmd->responseString = responseString;
            cmd->responseMessage = responseMessage;
            emit this->ctiResponse(actionId, cmd);
        }
    }
}

void AMIClient::sendDataToAsterisk(const QString& data)
{
    if (config->qDebug) qDebug() << "In AMIClient::sendDataToAsterisk(" << data << ")";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // We output simple ascii strings (no utf8)    
    out.writeRawData(data.toAscii(), data.length());

    qDebug() << block;

    this->localSocket->write(block);
    this->localSocket->flush();
}

void AMIClient::sendCommandToAsterisk(const int &actionId, const QString &commandName, const QString &data, const QString &channel)
{
    AsteriskCommand  *cmd = new AsteriskCommand();
    cmd->commandName    = commandName;
    cmd->command        = data;
    cmd->actionId       = actionId;
    cmd->channel        = channel;
    this->commandsHashtable->insert(actionId, cmd);

    this->sendDataToAsterisk(data);
}

void AMIClient::socketStateChanged(QAbstractSocket::SocketState socketState)
{
    QString newState("");
    switch(socketState) {
    case QAbstractSocket::UnconnectedState:
        newState = "UnconnectedState";
        break;
    case QAbstractSocket::HostLookupState:
        newState = "HostLookupState";
        break;
    case QAbstractSocket::ConnectingState:
        newState = "ConnectingState";
        break;
    case QAbstractSocket::ConnectedState:
        newState = "ConnectedState";
        break;
    case QAbstractSocket::BoundState:
        newState = "BoundState";
        break;
    case QAbstractSocket::ClosingState:
        newState = "ClosingState";
        break;
    case QAbstractSocket::ListeningState:
        newState = "ListeningState";
        break;
    }
    if (config->qDebug) qDebug() << "AMIClient socket state changed: " << newState;
}

void AMIClient::socketError(QAbstractSocket::SocketError socketError)
{
    // Q_UNUSED(socketError);
    if (socketError != QAbstractSocket::SocketTimeoutError) {
        qDebug() << "AMIClient Socket Error: " << socketError << " " << this->localSocket->errorString();
    }
}
