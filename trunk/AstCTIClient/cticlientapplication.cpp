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
 * AstCTIClient.  If you copy code from other releases into a copy of GNU
 * AstCTIClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTIClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include <QNetworkInterface>
#include <QXmlSimpleReader>

#include "cticlientapplication.h"

CtiClientApplication::CtiClientApplication(const QString &appId, int &argc, char **argv)
             : QtSingleApplication(appId, argc, argv)
{
    //If fatal error is encountered, we will exit the constructor without setting canStart to true
    //showLoginWindow() will then return false, which will tell main function to abort
    canStart = false;

    //Designer-generated code
    QString locale = QLocale::system().name();

    QTranslator translator;
    translator.load(QString("AstCTIClient_") + locale);
    installTranslator(&translator);

#if defined(Q_WS_MAC)
    QApplication::setQuitOnLastWindowClosed(false);
#else
    QApplication::setQuitOnLastWindowClosed(true);
#endif

    //Create configuration object
    this->config = new AstCTIConfiguration();

    //Process command-line arguments
    QArgumentList args(argc, argv);
    this->config->qDebug = args.getSwitch("--debug") | args.getSwitch("-d");

    this->config->serverHost = args.getSwitchArg("-h", defaultServerHost);
    if (config->serverHost.isEmpty()) {
        qFatal("No value given for Server Host (-h). If you want to use default host, don't include the command line switch.");
        return;
    }
    bool ok;
    this->config->serverPort = args.getSwitchArg("-p", defaultServerPort).toUShort(&ok);
    if (!ok) {
        qFatal("Wrong value given for Server Port (-p). Expected number from 0 to 65535.");
        return;
    }
    this->config->connectTimeout = args.getSwitchArg("-t", defaultConnectTimeout).toInt(&ok);
    if (!ok || this->config->connectTimeout < -1) {
        qFatal("Wrong value given for Connect Timeout (-t). Expected number from -1 to 2147483647.");
        return;
    }
    this->config->connectRetryInterval = args.getSwitchArg("-r", defaultConnectRetryInterval).toUInt(&ok);
    if (!ok) {
        qFatal("Wrong value given for Connect Retry Interval (-r). Expected number from 0 to 4294967295.");
        return;
    }

    //Set defaults
    this->config->compressionLevel = 0; //No compression initially. Compression level will be received from server.
    this->config->keepAliveInterval = defaultKeepAliveInterval; //Set to default initially. Interval will be received from server.
    this->loginWindow = 0;
    this->mainWindow = 0;
    this->connectionStatus = ConnStatusDisconnected;
    this->lastCTICommand = 0;
    this->localSocket = 0;
    this->blockSize = 0;
    this->reconnectNotify = true;

    //Initialize timers
    this->idleTimer = new QTimer(this);
    this->connectTimer = new QTimer(this);
    this->connectTimer->setSingleShot(true);
    connect(this->idleTimer, SIGNAL(timeout()), this, SLOT(idleTimerElapsed()));
    connect(this->connectTimer, SIGNAL(timeout()), this, SLOT(connectTimerElapsed()));

    //Initialize lists
    this->servicesList = new QHash<QString, QString>;
    this->queuesList = new QStringList();
    this->commandsList = new QHash<int, QString>;

    //Commands that can be sent to CTI server
    this->commandsList->insert(CmdQuit,             "QUIT");
    this->commandsList->insert(CmdCompression,      "CMPR");
    this->commandsList->insert(CmdKeepAlive,        "KEEP");
    this->commandsList->insert(CmdNoOp,             "NOOP");
    this->commandsList->insert(CmdUser,             "USER");
    this->commandsList->insert(CmdPass,             "PASS");
    this->commandsList->insert(CmdChangePassword,   "CHPW");
    this->commandsList->insert(CmdOsType,           "OSTYPE");
    this->commandsList->insert(CmdServices,         "SERVICES");
    this->commandsList->insert(CmdQueues,           "QUEUES");
    this->commandsList->insert(CmdPause,            "PAUSE");
    this->commandsList->insert(CmdOrig,             "ORIG");
    this->commandsList->insert(CmdStop,             "STOP");
    this->commandsList->insert(CmdMac,              "MAC");

    //Create socket for communication with server
    this->localSocket = new QTcpSocket();
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState" );
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError" );
    connect(this->localSocket, SIGNAL(connected()), this, SLOT (socketConnected()));
    connect(this->localSocket, SIGNAL(readyRead()), this, SLOT(receiveData()));
    connect(this->localSocket, SIGNAL(disconnected()), this, SLOT (socketDisconnected()));
    connect(this->localSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState) ), Qt::QueuedConnection);
    connect(this->localSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError) ), Qt::QueuedConnection);

    //All checks passed, we are good to go
    canStart = true;
}

CtiClientApplication::~CtiClientApplication()
{
    //Cleanup
    this->idleTimer->stop();
    this->connectTimer->stop();

    delete(this->idleTimer);
    delete(this->connectTimer);
    delete(this->config);
    delete(this->lastCTICommand);
    delete(this->localSocket);
    delete(this->commandsList);
    delete(this->servicesList);
    delete(this->queuesList);

    qDeleteAll(browserWindows);
}

//Called by main() and basically kicks of the application
//Later it is called when the user logs off
bool CtiClientApplication::showLoginWindow()
{
    //If error is encountered in constructor, we tell main() to abort
    if (!canStart) return false;

    //Create only if it doesn't exist
    if (this->loginWindow == 0) {
        this->loginWindow = new LoginWindow();

        connect(this->loginWindow, SIGNAL(accepted(QString, QString)), this, SLOT(loginAccept(QString, QString)));
        connect(this->loginWindow, SIGNAL(rejected()), this, SLOT(loginReject()));
    }

    //Set activation window for QtSingleApplication
    this->setActivationWindow(this->loginWindow, true);

    this->loginWindow->show();

    return true;
}

//Called when the user clicks OK in the login window
void CtiClientApplication::loginAccept(const QString &username, const QString &password)
{
    this->config->user = username;
    this->config->pass = password;

    //Initiate connection with server
    connectSocket();
}

//Called when the user clicks Cancel in the login window
void CtiClientApplication::loginReject()
{
    //Close the application
    this->exit(0);
}

//Called when the user clicks 'Change password' in the main window
void CtiClientApplication::changePassword(){
    PasswordWindow *w = new PasswordWindow(this->mainWindow);
    if (w->exec() == QDialog::Accepted) {
        //Remember password so we can update it in configuration when (and if) server accepts it
        this->newPassword = w->newPass;
        sendCommandToServer(CmdChangePassword, w->oldPass + " " + this->newPassword);
    }
}

//Called when the user logs off in the main window
void CtiClientApplication::logOff()
{
    //Let the server know that the user has logged off
    sendCommandToServer(CmdQuit);

    //We don't wait for server to acknowledge user logoff
    //If connection is dropped before that, the server will log user off automatically
    abortConnection(StopUserDisconnect, QString());
}

//Called when one of internal browsers closes
void  CtiClientApplication::browserWindowClosed(BrowserWindow *window)
{
    //Remove browser from the list
    this->browserWindows.removeOne(window);
}

//Initiates TCP connection
void CtiClientApplication::connectSocket()
{
    this->connectionStatus = ConnStatusConnecting;

    //Start connection with server. The connection will continue asynchronously, using signals and slots
    this->localSocket->connectToHost(config->serverHost, config->serverPort);

    //If connection times out, socket will emit error signal with SocketTimeoutError
    //Unfortunately, currently (Qt 4.5) it is not possible to change timeout value (hardcoded to 30 sec)
    //So we start our own timer set to configured timeout value
    //Connection attempt will be aborted either by socket timeout or by timer, whichever comes first
    //Then this timer will be started again, now with the value of connectRetryInterval (see socketDisconnected())
    //So the timer will either drop or start the conection, based on conectionState
    this->connectTimer->start(this->config->connectTimeout);
}

//Called when connection attempt is definitively aborted due to error or user disconnect
void CtiClientApplication::abortConnection(StopReason stopReason, const QString &message) {
    this->connectionStatus = ConnStatusDisconnected;

    this->localSocket->abort();

    if (config->qDebug) qDebug() << "Connection to server closed. Reason: " << stopReason << " " << message;

    QString msg;

    switch (stopReason) {
    case StopUserDisconnect:
        //User disconnected manually by rejecting login, application will be closed
        return;
    case StopInternalError:
        msg = "Internal error occured:\n" + message;
        break;
    case StopServerError:
        msg = "Server error occured:\n" + message;
        break;
    case StopInvalidCredentials:
        msg = "Your credentials were not accepted:\n" + message;
        break;
    }

    if (this->mainWindow != 0) {
        //Tell main window to close
        emit closeWindow(true);
        this->mainWindow = 0;
    }

    //Redisplay login window with the error message
    this->loginWindow->showMessage(msg, false);
    this->loginWindow->show();
}

//Called when the client can't start the connection or when connection is broken
void CtiClientApplication::connectionLost()
{
    //If the main window is displayed, the connection has been broken, otherwise it couldn't be started
    //We keep trying to connect either way
    if (this->mainWindow != 0) {
        emit statusChange(false);
    } else {
        //User won't be able to click OK button again until connection is established
        this->loginWindow->showMessage("AsteriskCTI client is unable to connect to AsteriskCTI server. "
                                      "Please check that the correct parameter is passed from the command line.\n\n"
                                      "AsteriskCTI client will keep trying to connect.", true);
        this->loginWindow->show();
    }
}

void CtiClientApplication::resetLastCTICommand()
{
    delete(this->lastCTICommand);
    this->lastCTICommand = 0;
}

//Creates AstCTIResponse object from string
AstCTIResponse CtiClientApplication::parseResponse(const QString &response)
{
    QStringList data = response.split(" ");
    AstCTIResponse newResponse;
    newResponse.code = (AstCTIResponseCodes)(QString(data.at(0)).toInt());
    data.removeFirst();
    newResponse.data = data;
    return newResponse;
}

//Analyzes block of data received from server and sends it for further processing
void CtiClientApplication::parseDataReceivedFromServer(const QString &message)
{
    if (message.isEmpty()) {
        if (config->qDebug) qDebug() << "Received empty string from server.";
        return;
    }

    QChar firstChar = message.at(0);
    if (firstChar.isNumber()) {
        //First character of received data is a number, which means it is a response from server
        AstCTIResponse response = parseResponse(message);
        processResponse(response);
    } else {
        //Otherwise it is an XML object containing call event
        processEvent(message);
    }
}

//Analyzes server response and takes appropriate action
//The handshake protocol is followed until authentication is complete or error occurs
//If authentication is succesful, main window is displayed
void CtiClientApplication::processResponse(AstCTIResponse &response) {
    if (config->qDebug) qDebug() << "Processing server response: " << response.code << response.data.join(" ");

    switch (response.code) {
    case RspAuthOK:
        resetLastCTICommand();
        sendCommandToServer(CmdKeepAlive);
        break;
    case RspCompressLevel:
        this->config->compressionLevel = response.data.last().toInt();
        resetLastCTICommand();
        sendCommandToServer(CmdUser, this->config->user);
        break;
    case RspDisconnectOK:
        //Nothing to do
        break;
    case RspError:
        if (this->lastCTICommand != 0) {
            switch (this->lastCTICommand->command) {
            case CmdUser:
            case CmdPass:
                abortConnection(StopInvalidCredentials, response.data.join(" "));
                break;
            case CmdChangePassword:
                resetLastCTICommand();
                this->newMessage("Changing of password did not succeed: " + response.data.join(" "), QSystemTrayIcon::Critical);
                break;
            case CmdMac:
            case CmdOsType:
                abortConnection(StopServerError, response.data.join(" "));
                break;
            case CmdOrig:
                //TODO
                break;
            default:
                if (config->qDebug) qDebug() << "Received unexpected response from server: \'" << response.code << " " << response.data.join(" ") << "\' to command " << this->lastCTICommand->command;
                resetLastCTICommand();
                break;
            }
        } else {
            if (config->qDebug) qDebug() << "Received unexpected response from server: \'" << response.code << " " << response.data.join(" ") << "\' to unknown command";
        }
        break;
    case RspKeepAlive:
        this->config->keepAliveInterval = response.data.last().toInt();
        //Reduce the interval to allow for network latency
        if (this->config->keepAliveInterval > 4000)
            this->config->keepAliveInterval -= 2000;
        else
            this->config->keepAliveInterval /= 2;
        resetLastCTICommand();
        sendCommandToServer(CmdMac, this->macAddress);
        break;
    case RspOK:
        if (this->lastCTICommand != 0) {
            switch (this->lastCTICommand->command) {
            case CmdUser:
                resetLastCTICommand();
                sendCommandToServer(CmdPass, this->config->pass);
                break;
            case CmdPass:
                resetLastCTICommand();
                sendCommandToServer(CmdKeepAlive);
                break;
            case CmdChangePassword:
                resetLastCTICommand();
                //Update password
                this->config->pass = this->newPassword;
                this->newMessage("Changing of password was successfull.", QSystemTrayIcon::Information);
                break;
            case CmdMac:
                resetLastCTICommand();
                sendCommandToServer(CmdOsType, osType);
                break;
            case CmdOsType:
                resetLastCTICommand();
                this->connectionStatus = ConnStatusLoggedIn;
                showMainWindow(response.data.last().split("-").last());
                break;
            case CmdNoOp:
                resetLastCTICommand();
                break;
            case CmdOrig:
                //TODO
                break;
            default:
                if (config->qDebug) qDebug() << "Received unexpected response from server: \'" << response.code << " " << response.data.join(" ") << "\' to command " << this->lastCTICommand->command;
                resetLastCTICommand();
                break;
            }
        } else {
            if (response.data.length() > 1) {
                if (response.data.at(1) == "Welcome")
                    sendCommandToServer(CmdCompression);
            } else {
                if (config->qDebug) qDebug() << "Received unexpected response from server: \'" << response.code << " " << response.data.join(" ") << "\' to unknown command";
            }
        }
        break;
    case RspPauseError:
        resetLastCTICommand();
        emit pauseError(response.data.join(" "));
        break;
    case RspPauseOK:
        resetLastCTICommand();
        emit pauseAccepted();
        break;
    case RspPausePending:
        //Pause is pending, it will be confirmed or denied later by RspPauseOK or RspPauseError
        resetLastCTICommand();
        break;
    case RspQueueData:
        this->queuesList->append(response.data.last());
        break;
    case RspQueueListEnd:
        resetLastCTICommand();
        emit queuesReceived(this->queuesList);
        break;
    case RspServiceData:
        this->servicesList->insert(response.data.first(), response.data.last());
        break;
    case RspServiceListEnd:
        resetLastCTICommand();
        emit servicesReceived(this->servicesList);
        break;
    }
}

//Creates AstCTICall object from XML using custom parser
void CtiClientApplication::processEvent(const QString &eventData) {
    if (config->qDebug) qDebug() << "Processing event: \n" << eventData;

    AstCTICall call;

    AstCTICallXMLParser handler(&call);

    QXmlInputSource source;
    source.setData(eventData);

    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);

    reader.parse(&source);

    //Let main window know about the event
    //TODO: Perhaps process event (or some events) here?
    emit eventReceived(&call);
}

//Prepares data for sending to server, using compression if so indicated
QByteArray CtiClientApplication::convertDataForSending(const QString &data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);

    //First two bytes are used for framing
    out << (quint16)0;
    if (this->config->compressionLevel > 0) {
        //Convert to UTF8 before compressing
        out << qCompress(data.toUtf8(), this->config->compressionLevel);
    } else {
        //Uncompressed data is sent as-is
        out << data;
    }
    //Write data size in frame descriptor
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    return block;
}

//Sends a block of data to server
void CtiClientApplication::sendDataToServer(const QString &data)
{
    if (config->qDebug) qDebug() << "Sending data to server: " << data;

    this->localSocket->write(convertDataForSending(data));
    this->localSocket->flush();

    //Restart the idle timer
    this->idleTimer->start(this->config->keepAliveInterval);
}

//Sends a parameterless command to server (overloaded)
void CtiClientApplication::sendCommandToServer(const AstCTICommands command)
{
    sendCommandToServer(command, QString());
}

//Sends a command to server
void CtiClientApplication::sendCommandToServer(const AstCTICommands command, const QString &parameters)
{
    if (this->lastCTICommand != 0) {
        //New command was requested before we received a response to last command
        //This should not happen, it could indicate a bug in the client or server
        qCritical() << "New command ("  << command << ") requested before receiving response from server to command " << this->lastCTICommand->command;

        //We exit without sending the command
        return;
    }

    //Create and remember command object
    AstCTICommand *cmd = new AstCTICommand();
    cmd->command = command;
    cmd->parameters = parameters;
    this->lastCTICommand = cmd;

    if (cmd->command == CmdServices)
        //Prepare for receiving services
        this->servicesList->clear();

    if (cmd->command == CmdQueues)
        //Prepare for receiving queues
        this->queuesList->clear();

    //Convert command to string and send to server
    QString data = this->commandsList->value(cmd->command);
    if (!parameters.isEmpty())
        data.append(" " + parameters);

    sendDataToServer(data);
}

//Displays the main window
void CtiClientApplication::showMainWindow(const QString &extension)
{
    if (this->mainWindow == 0) {
        CompactWindow *w = new CompactWindow(this->config->user);

        connect(this, SIGNAL(newMessage(QString,QSystemTrayIcon::MessageIcon)), w, SLOT(showMessage(QString,QSystemTrayIcon::MessageIcon)));
        connect(this, SIGNAL(statusChange(bool)), w, SLOT(setStatus(bool)));
        connect(this, SIGNAL(closeWindow(bool)), w, SLOT(quit(bool)));

        connect(w, SIGNAL(logOff()), this, SLOT(logOff()));
        connect(w, SIGNAL(changePassword()), this, SLOT(changePassword()));

        this->mainWindow = dynamic_cast<QWidget*>(w);
        w->show();
    }

    //Set activation window for QtSingleApplication
    this->setActivationWindow(this->mainWindow, true);

    if (this->loginWindow->isVisible()) {
        this->loginWindow->hide();
    }

    emit statusChange(true);
    //TODO: Extension
}

//Displays the internal browser
void CtiClientApplication::newBrowserWindow()
{
    // All browsers will be child of our main window
    // if we close main window all browser' windows will close too
    BrowserWindow *browser = new BrowserWindow(this->mainWindow);
    this->browserWindows.append(browser);
    browser->show();
}

//Slot which receives socket's connected signal
void CtiClientApplication::socketConnected()
{
    //We are connected. so connect timer is no longer necessary
    this->connectTimer->stop();

    //Get MAC Address (necessary for authentication with server)
    this->macAddress = "";
    QHostAddress localAddress = this->localSocket->localAddress();
    foreach(QNetworkInterface iface, QNetworkInterface::allInterfaces()) {
        if (iface.flags().testFlag(QNetworkInterface::IsRunning)) {
            foreach (QNetworkAddressEntry entry, iface.addressEntries()) {
                if (entry.ip() == localAddress) {
                    this->macAddress = iface.hardwareAddress();
                    break;
                }
            }
        }
        if (!this->macAddress.isEmpty())
            break;
    }
    if (this->macAddress.isEmpty()) {
        qCritical() << "Unable to determine local MAC address. Aborting.";
        abortConnection(StopInternalError, "Unable to determine local MAC address.");
        return;
    }

    this->connectionStatus = ConnStatusLoggingIn;
}

//Slot which receives socket's disconnected signal
void CtiClientApplication::socketDisconnected()
{
    //Reset compression level to zero (initial communication with server is always uncompressed)
    this->config->compressionLevel = 0;

    //Delete last command if it exists
    resetLastCTICommand();

    if (this->connectionStatus != ConnStatusDisconnected) {
        if (config->qDebug) qDebug() << "Conection to server lost. I will keep trying to reconnect at " << this->config->connectRetryInterval << " second intervals.";

        //Inform user
        connectionLost();

        //Retry connection
        connectSocket();
    }
}

//Slot which receives socket's stateChanged signal
void CtiClientApplication::socketStateChanged(QAbstractSocket::SocketState socketState)
{
    if (config->qDebug) {
        QString newState;
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
}

//Slot which receives socket's error signal
void CtiClientApplication::socketError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QAbstractSocket::SocketTimeoutError) {
        if (this->connectionStatus == ConnStatusConnecting) {
            //Notify user only about first reconnect attempt
            if (this->reconnectNotify) {
                if (config->qDebug) qDebug() << "Unable to connect to server. I will keep trying to connect at " << this->config->connectRetryInterval << " second intervals.";

                connectionLost();

                this->reconnectNotify = false;
            }
        }

        this->connectionStatus = ConnStatusDisconnected;

        this->localSocket->abort();

        //Restart connect timer so it reinitiates connection after a specified interval
        this->connectTimer->start(this->config->connectTimeout);
    } else {
        if (config->qDebug) qDebug() << "Socket Error: " << socketError << " " << this->localSocket->errorString();
    }
}

//Slot which receives socket's readyRead signal
void CtiClientApplication::receiveData()
{
    QDataStream in(this->localSocket);
    in.setVersion(QDataStream::Qt_4_5);

    while (!in.atEnd()) {
        if (this->blockSize == 0) {
            //Read data block size from frame descriptor (if enough data available)
            if (this->localSocket->bytesAvailable() >= (int)sizeof(quint16))
                in >> this->blockSize;
        }

        //Wait for entire data block to be received
        //If not, readyRead will be emitted again when more data is available
        if (this->localSocket->bytesAvailable() >= this->blockSize) {
            QByteArray dataFromSocket;
            QString receivedData;
            if (this->config->compressionLevel > 0) {
                //Compressed data will be UTF8 encoded
                in >> dataFromSocket;
                QByteArray uncompressedData = qUncompress(dataFromSocket);
                if (uncompressedData.isEmpty()) {
                    qCritical() << "Unable to uncompress data received from server. Try to use uncompressed data.";
                    receivedData = QString::fromUtf8(dataFromSocket);
                } else {
                    receivedData = QString::fromUtf8(uncompressedData);
                }
            } else {
                //Uncompressed data will be sent as-is
                in >> receivedData;
            }

            //Reset block size
            this->blockSize = 0;

            //Analyze received data
            parseDataReceivedFromServer(receivedData);
        }
    }

    //Restart the idle timer
    this->idleTimer->start(this->config->keepAliveInterval);
}

//Called when connection has been idle for a specified period
void CtiClientApplication::idleTimerElapsed()
{
    if (this->lastCTICommand != 0) {
        //Idle timer elapsed before we received a response to last command
        //This should not happen, it probably indicates a bug in the server
        qCritical() << "Idle timer elapsed before receiving response from server to command " << this->lastCTICommand->command;

        //We exit without sending keep-alive signal, this will probably result in server dropping the connection
        return;
    }

    //Send NoOp command to server to keep connection alive
    if (this->localSocket->state() == QAbstractSocket::ConnectedState) {
        if (this->connectionStatus == ConnStatusLoggedIn)
            sendCommandToServer(CmdNoOp);
    }
}

//Called when given connection timeout or connection retry interval has elapsed
//Appropriate action will be taken depending on connection state (see connectSocket() and socketError())
void CtiClientApplication::connectTimerElapsed()
{
    if (this->connectionStatus == ConnStatusConnecting) {
        //Simulate socket timeout error
        socketError(QAbstractSocket::SocketTimeoutError);
    } else {
        //Retry connection
        connectSocket();
    }
}
