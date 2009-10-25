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

#include <QNetworkInterface>
#include <QSysInfo>
#include <QXmlSimpleReader>
#include <QCryptographicHash>

#include "serverconnection.h"

Q_DECLARE_METATYPE(QAbstractSocket::SocketState);
Q_DECLARE_METATYPE(QAbstractSocket::SocketError);

ServerConnection::ServerConnection(AstCTIConfiguration *config)
        : QThread()
{
    this->config = config;
    this->connectionStatus = ConnStatusNotDefined;
    this->lastCTICommand = 0;
    this->localSocket = 0;
    this->blockSize = 0;
    this->reconnectNotify = true;
    this->idleTimer = new QTimer(this);
    this->commandsList = new QHash<int, QString>;
    this->servicesList = new QHash<QString, QString>;
    this->queuesList = new QStringList();

    connect(this->idleTimer, SIGNAL(timeout()), this, SLOT(idleTimerElapsed()));
    connect (this, SIGNAL(started()), this, SLOT(threadStarted()));

    this->initCTICommands();

    this->moveToThread(this);
}

ServerConnection::~ServerConnection()
{
    this->idleTimer->stop();
    delete(this->idleTimer);
    delete(this->lastCTICommand);
    delete(this->localSocket);
    delete(this->commandsList);
    delete(this->servicesList);
    delete(this->queuesList);
}

void ServerConnection::initCTICommands()
{
    this->commandsList->insert(CmdQuit, "QUIT");
    this->commandsList->insert(CmdCompression, "CMPR");
    this->commandsList->insert(CmdKeepAlive, "KEEP");
    this->commandsList->insert(CmdNoOp, "NOOP");
    this->commandsList->insert(CmdUser, "USER");
    this->commandsList->insert(CmdPass, "PASS");
    this->commandsList->insert(CmdOsType, "OSTYPE");
    this->commandsList->insert(CmdServices, "SERVICES");
    this->commandsList->insert(CmdQueues, "QUEUES");
    this->commandsList->insert(CmdPause, "PAUSE");
    this->commandsList->insert(CmdOrig, "ORIG");
    this->commandsList->insert(CmdStop, "STOP");
    this->commandsList->insert(CmdMac, "MAC");
}

void ServerConnection::buildSocket()
{
    this->localSocket = new QTcpSocket();

    if (config->qDebug) {
        qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState" );
        qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError" );
        connect(this->localSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState) ), Qt::QueuedConnection);
        connect(this->localSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError) ), Qt::QueuedConnection);
    }
}

void ServerConnection::run()
{
    this->exec();
}

void ServerConnection::threadStarted()
{
    if (this->localSocket == 0) {
        buildSocket();
    }

    connect(localSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));
    connect(localSocket, SIGNAL(disconnected()), this, SLOT (socketDisconnected()));

    connectSocket();
}

void ServerConnection::connectSocket()
{
    this->connectionStatus = ConnStatusConnecting;

    bool connected = false;

    while (!connected) {
        this->localSocket->connectToHost(config->serverHost, config->serverPort);

        if (!this->localSocket->waitForConnected(this->config->connectTimeout) ) {
            if (this->reconnectNotify) {
                if (config->qDebug) {
                    qDebug() << "Unable to connect to server. I will keep trying to connect at " << this->config->connectRetryInterval << " second intervals.";
                }
                emit this->connectionLost();
                this->reconnectNotify = false;
            }
            this->sleep(this->config->connectRetryInterval);
        } else {
            //Get MAC Address
            this->macAddress = "";
            QHostAddress localAddress = this->localSocket->localAddress();
            QList<QNetworkInterface> ifaceList = QNetworkInterface::allInterfaces();
            foreach (QNetworkInterface iface, ifaceList) {
                if (iface.allAddresses().contains(localAddress)) {
                    this->macAddress = iface.hardwareAddress();
                    break;
                }
            }
            if (this->macAddress.isEmpty()) {
                qCritical() << "Unable to determine local MAC address. Aborting.";
                abortThread(StopInternalError, "Unable to determine local MAC address.");
                return;
            }
            connected = true;
        }
    }
}

void ServerConnection::socketDisconnected()
{
    if (this->connectionStatus != ConnStatusDisconnecting) {
        if (config->qDebug) qDebug() << "Conection to server lost. I will keep trying to reconnect at " << this->config->connectRetryInterval << " second intervals.";

        emit this->connectionLost();
        connectSocket();
    }
}

bool ServerConnection::isConnected()
{
    return (this->localSocket != 0) && (this->localSocket->state() == QAbstractSocket::ConnectedState);
}

void ServerConnection::abortThread(StopReason stopReason, const QString &message) {
    //This will tell the socketDisconnected function to stop trying to connect
    this->connectionStatus = ConnStatusDisconnecting;

    if (isConnected())
        this->localSocket->close();

    if (config->qDebug) qDebug() << "Connection to server closed. Reason: " << stopReason << " " << message;

    emit this->threadStopped(stopReason, message);

    this->sleep(1000);

    this->quit();
}

void ServerConnection::receiveData()
{
    QDataStream in(this->localSocket);
    in.setVersion(QDataStream::Qt_4_5);

    while (!in.atEnd()) {
        if (this->blockSize == 0) {
            if (this->localSocket->bytesAvailable() >= (int)sizeof(quint16))
                in >> this->blockSize;
        }

        if (this->localSocket->bytesAvailable() >= this->blockSize) {
            QByteArray dataFromSocket;
            QString receivedData;
            if (this->config->compressionLevel > 0) {
                in >> dataFromSocket;
                QByteArray uncompressedData = qUncompress(dataFromSocket);
                if (uncompressedData.isEmpty()) {
                    qCritical() << "Unable to uncompress data received from server. Try to use uncompressed data.";
                    receivedData = QString::fromUtf8(dataFromSocket);
                } else {
                    receivedData = QString::fromUtf8(uncompressedData);
                }
            } else {
                in >> receivedData;
            }

            this->blockSize = 0;

            parseDataReceivedFromServer(receivedData);
        }
    }

    //Restart the idle timer
    this->idleTimer->start(this->config->keepAliveInterval);
}

AstCTIResponse ServerConnection::parseResponse(const QString &response)
{
    QStringList data = response.split(" ");
    AstCTIResponse newResponse;
    newResponse.code = (AstCTIResponseCodes)(QString(data.at(0)).toInt());
    data.removeFirst();
    newResponse.data = data;
    return newResponse;
}

void ServerConnection::parseDataReceivedFromServer(const QString &message)
{
    if (message.isEmpty()) {
        if (config->qDebug) qDebug() << "Received empty string from server.";
        return;
    }
    
    QChar firstChar = message.at(0);
    if (firstChar.isNumber()) {
        AstCTIResponse response = parseResponse(message);
        processResponse(response);
    } else {
        processEvent(message);
    }
}

void ServerConnection::processResponse(AstCTIResponse &response) {
    int param;
    QString exten;
    QCryptographicHash md5(QCryptographicHash::Md5);
    QByteArray result;

    switch (response.code) {
    case RspAuthOK:
        sendCommandToServer(CmdKeepAlive);
        break;
    case RspCompressLevel:
        param = response.data.last().toInt();
        this->config->compressionLevel = param;
        sendCommandToServer(CmdUser, this->config->user);
        break;
    case RspDisconnectOK:
        abortThread(StopUserDisconnect, QString());
        break;
    case RspError:
        if (this->lastCTICommand != 0) {
            switch (this->lastCTICommand->command) {
            case CmdUser:
            case CmdPass:
                abortThread(StopInvalidCredentials, response.data.join(" "));
                break;
            case CmdMac:
            case CmdOsType:
                abortThread(StopServerError, response.data.join(" "));
                break;
            case CmdOrig:
                //TODO
                break;
            default:
                if (config->qDebug) qDebug() << "Received unexpected response from server: \'" << response.code << " " << response.data.join(" ") << "\' to command " << this->lastCTICommand->command;
                break;
            }
        } else {
            if (config->qDebug) qDebug() << "Received unexpected response from server: \'" << response.code << " " << response.data.join(" ") << "\' to unknown command";
        }
        break;
    case RspKeepAlive:
        param = response.data.last().toInt();
        if (param > 500)
            param -= 500;
        this->config->keepAliveInterval = param;
        sendCommandToServer(CmdMac, this->macAddress);
        break;
    case RspOK:
        if (this->lastCTICommand != 0) {
            switch (this->lastCTICommand->command) {
            case CmdUser:
                md5.addData(QByteArray(this->config->pass.toAscii()));
                result = md5.result();
                sendCommandToServer(CmdPass, QString(result.toHex()));
                break;
            case CmdPass:
                sendCommandToServer(CmdKeepAlive);
                break;
            case CmdMac:
                sendCommandToServer(CmdOsType, osType);
                break;
            case CmdOsType:
                this->lastCTICommand = 0;
                this->connectionStatus = ConnStatusLoggedIn;
                exten = response.data.last();
                exten = exten.split("-").last();
                emit this->loggedIn(exten);
                break;
            case CmdNoOp:
                this->lastCTICommand = 0;
                break;
            case CmdOrig:
                //TODO
                break;
            default:
                if (config->qDebug) qDebug() << "Received unexpected response from server: \'" << response.code << " " << response.data.join(" ") << "\' to command " << this->lastCTICommand->command;
                break;
            }
        } else {
            if (response.data.length() > 1) {
                if (response.data.at(1) == "Welcome") {
                    sendCommandToServer(CmdCompression);
                }
            } else {
                if (config->qDebug) qDebug() << "Received unexpected response from server: \'" << response.code << " " << response.data.join(" ") << "\' to unknown command";
            }
        }
        break;
    case RspPauseError:
        this->lastCTICommand = 0;
        emit this->pauseError(response.data.join(" "));
        break;
    case RspPauseOK:
        this->lastCTICommand = 0;
        emit this->pauseAccepted();
        break;
    case RspPausePending:
        //TODO: Not sure what to do here. Is this necessary?
        break;
    case RspQueueData:
        this->queuesList->append(response.data.last());
        break;
    case RspQueueListEnd:
        emit this->queuesReceived(this->queuesList);
        break;
    case RspServiceData:
        this->servicesList->insert(response.data.first(), response.data.last());
        break;
    case RspServiceListEnd:
        emit this->servicesReceived(this->servicesList);
        break;
    }
}

void ServerConnection::processEvent(const QString &eventData) {
    //TODO
    AstCTICall *call = new AstCTICall();

    AstCTICallXMLParser handler(call);

    QXmlInputSource *source = new QXmlInputSource();
    source->setData(eventData);

    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);

    reader.parse(source);

    emit this->eventReceived(call);
}

void ServerConnection::idleTimerElapsed()
{
    if (this->localSocket->state() == QAbstractSocket::ConnectedState) {
        if (this->connectionStatus == ConnStatusLoggedIn)
            sendCommandToServer(CmdNoOp);
    }
}

QByteArray ServerConnection::convertDataForSending(const QString &data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);

    out << (quint16)0;
    if (this->config->compressionLevel > 0) {
        out << qCompress(data.toUtf8(), this->config->compressionLevel);
    } else {
        out << data;
    }
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    return block;
}

void ServerConnection::sendDataToServer(const QString &data)
{
    this->localSocket->write(convertDataForSending(data));
    this->localSocket->flush();

    //Restart the idle timer
    this->idleTimer->start(this->config->keepAliveInterval);
}

void ServerConnection::sendCommandToServer(const AstCTICommands command)
{
    sendCommandToServer(command, QString());
}

void ServerConnection::sendCommandToServer(const AstCTICommands command, const QString &parameters)
{
    AstCTICommand *cmd = new AstCTICommand();
    cmd->command = command;
    cmd->parameters = parameters;
    this->lastCTICommand = cmd;

    if (cmd->command == CmdServices)
        this->servicesList->clear();

    if (cmd->command == CmdQueues)
        this->queuesList->clear();

    QString data = this->commandsList->value(cmd->command);
    if (!parameters.isEmpty())
        data.append(" " + parameters);

    sendDataToServer(data);
}

void ServerConnection::socketStateChanged(QAbstractSocket::SocketState socketState)
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
    qDebug() << "Socket state changed: " << newState;
}

void ServerConnection::socketError(QAbstractSocket::SocketError socketError)
{
    if (socketError != QAbstractSocket::SocketTimeoutError) {
        qDebug() << "Socket Error: " << socketError << " " << this->localSocket->errorString();
    }
}
