/* Copyright (C) 2007-2010 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * Copyright (C) 2007-2010 Lumiss d.o.o.
 * http://www.lumiss.hr
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
 * AstCtiClient.  If you copy code from other releases into a copy of GNU
 * AstCtiClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCtiClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include <QNetworkInterface>

#include "argumentlist.h"
#include "passwordwindow.h"
#include "compactwindow.h"
#include "managerwindow.h"
#include "cticlientapplication.h"

CtiClientApplication::CtiClientApplication(const QString& appId, int &argc, char* *argv)
             : QtSingleApplication(appId, argc, argv)
{
    //If fatal error is encountered, we will exit the constructor without setting canStart to true
	//main function will read this variable and abort
    canStart = false;

    //Designer-generated code
    QString locale = QLocale::system().name();

    QTranslator translator;
    translator.load(QString("AstCtiClient_") + locale);
    installTranslator(&translator);

//On Mac, the common behaviour is to leave application running even when last window is closed
//However, that doesn't make sense for this type of application
//#if defined(Q_WS_MAC)
//    QApplication::setQuitOnLastWindowClosed(false);
//#else
//    QApplication::setQuitOnLastWindowClosed(true);
//#endif

    //Create configuration object
    this->config = new AstCtiConfiguration();

    //Process command-line arguments
	ArgumentList args(this->arguments());
	this->config->debug = args.getSwitch(QStringLiteral("debug")) |
						  args.getSwitch(QStringLiteral("d"));

	this->config->serverHost = args.getSwitchArg(QStringLiteral("h"), defaultServerHost);
    if (config->serverHost.isEmpty()) {
		qCritical() << "No value given for Server Host (-h). If you want to use default host, "
					   "don't include the command line switch.";
        return;
    }
    bool ok;
	this->config->serverPort =
			args.getSwitchArg(QStringLiteral("p"), defaultServerPort).toUShort(&ok);
    if (!ok) {
		qCritical() << "Wrong value given for Server Port (-p). Expected number from 0 to 65535.";
        return;
    }
	this->config->connectTimeout =
			args.getSwitchArg(QStringLiteral("t"), defaultConnectTimeout).toInt(&ok);
    if (!ok || this->config->connectTimeout < -1) {
		qCritical() << "Wrong value given for Connect Timeout (-t). "
					   "Expected number from -1 to 2147483647.";
        return;
    }
	this->config->connectRetryInterval =
			args.getSwitchArg(QStringLiteral("r"), defaultConnectRetryInterval).toUInt(&ok);
    if (!ok) {
		qCritical() << "Wrong value given for Connect Retry Interval (-r). "
					   "Expected number from 0 to 4294967295.";
        return;
    }
	if (args.getSwitch(QStringLiteral("callcenter")))
		this->clientType = CtiClientCallCenter;
	else
		this->clientType = CtiClientPhoneManager;

    //Set defaults
	//No compression initially. Compression level will be received from server.
	this->config->compressionLevel = 0;
	//Set to default initially. Interval will be received from server.
	this->config->keepAliveInterval = defaultKeepAliveInterval;
    this->loginWindow = 0;
    this->mainWindow = 0;
    this->connectionStatus = ConnStatusDisconnected;
    this->lastCtiCommand = 0;
    this->localSocket = 0;
    this->blockSize = 0;
    this->reconnectNotify = true;

    //Initialize timers
    this->idleTimer = new QTimer(this);
    this->connectTimer = new QTimer(this);
	connect(this->idleTimer, SIGNAL(timeout()),
			this, SLOT(idleTimerElapsed()));
	connect(this->connectTimer, SIGNAL(timeout()),
			this, SLOT(connectTimerElapsed()));
	this->connectTimer->setSingleShot(true);
	//Timer will not start before application enters event loop
	this->connectTimer->start(0);

    //Create socket for communication with server
    this->localSocket = new QTcpSocket();
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState" );
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError" );
	connect(this->localSocket, SIGNAL(connected()),
			this, SLOT(socketConnected()));
	connect(this->localSocket, SIGNAL(readyRead()),
			this, SLOT(receiveData()));
	connect(this->localSocket, SIGNAL(disconnected()),
			this, SLOT(socketDisconnected()));
	connect(this->localSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
			this, SLOT(socketStateChanged(QAbstractSocket::SocketState)), Qt::QueuedConnection);
	connect(this->localSocket, SIGNAL(error(QAbstractSocket::SocketError)),
			this, SLOT(socketError(QAbstractSocket::SocketError)), Qt::QueuedConnection);

    //All checks passed, we are good to go
    canStart = true;
}

CtiClientApplication::~CtiClientApplication()
{
    //Cleanup
    this->idleTimer->stop();
    this->connectTimer->stop();

	this->resetLastCtiCommand();
	delete this->idleTimer;
	delete this->connectTimer;
	delete this->config;
	delete this->localSocket;

    qDeleteAll(this->applications);
    qDeleteAll(this->browserWindows);
}

//Called when the user clicks OK in the login window
void CtiClientApplication::loginAccept(const QString& username, const QString& password)
{
	this->config->userName = username;
	this->config->password = password;

    //Initiate connection with server
	this->connectSocket();
}

//Called when the user clicks Cancel in the login window
void CtiClientApplication::loginReject()
{
	QApplication::processEvents(QEventLoop::AllEvents);
    //Close the application
	this->quit();
}

//Called when the user clicks 'Pause' in the main window
void CtiClientApplication::pause(bool paused)
{
	sendCommandToServer(CmdPause, paused ? QStringLiteral("IN") : QStringLiteral("OUT"));
}

//Called when the user clicks 'Change password' in the main window
void CtiClientApplication::changePassword(){
    PasswordWindow* w = new PasswordWindow(this->mainWindow);
    if (w->exec() == QDialog::Accepted) {
        //Remember password so we can update it in configuration when (and if) server accepts it
        this->newPassword = w->newPass;
		this->sendCommandToServer(CmdChangePassword,
								  QString("%1 %2").arg(w->oldPass, this->newPassword));
    }

    //TEMP
	//this->newBrowserWindow(QUrl("http://www.google.com"));
}

//Called when the user logs off in the main window
void CtiClientApplication::logOff()
{
    //Let the server know that the user has logged off
	this->sendCommandToServer(CmdQuit);

    //Close all open applications (application is closed when QProcess is destroyed)
    qDeleteAll(this->applications);
    this->applications.clear();

	//Close all browser windows (window is automatically removed from array when closed,
	//see browserWindowClosed())
	for (int i = this->browserWindows.count() - 1; i > -1; i--)
        this->browserWindows[i]->close();

    //We don't wait for server to acknowledge user logoff
    //If connection is dropped before that, the server will log user off automatically
	this->abortConnection(StopUserDisconnect, QStringLiteral(""));
}

//Called when one of internal browsers closes
void  CtiClientApplication::browserWindowClosed(BrowserWindow* window)
{
    //Remove browser from the list
    this->browserWindows.removeOne(window);
}

//Called when one of started applications closes
void  CtiClientApplication::applicationClosed(QProcess* process)
{
    //Remove process from the list
    this->applications.removeOne(process);
}

//Initiates TCP connection
void CtiClientApplication::connectSocket()
{
    this->connectionStatus = ConnStatusConnecting;

	//Start connection with server. The connection will continue asynchronously,
	//using signals and slots
    this->localSocket->connectToHost(config->serverHost, config->serverPort);

    //If connection times out, socket will emit error signal with SocketTimeoutError
	//Unfortunately, currently (Qt 5.0) it is not possible to change timeout value
	//(hardcoded to 30 sec)
    //So we start our own timer set to configured timeout value
    //Connection attempt will be aborted either by socket timeout or by timer, whichever comes first
	//Then this timer will be started again, now with the value of connectRetryInterval
	//(see socketDisconnected())
    //So the timer will either drop or start the conection, based on connection state
    this->connectTimer->start(this->config->connectTimeout);
}

//Called when connection attempt is definitively aborted due to error or user disconnect
void CtiClientApplication::abortConnection(const StopReason stopReason, const QString& message){
    this->connectionStatus = ConnStatusDisconnected;

    this->localSocket->abort();

	if (config->debug)
		qDebug() << "Connection to server closed. Reason:" << stopReason << message;

    QString msg;

    switch (stopReason) {
    case StopUserDisconnect:
        //User disconnected manually by rejecting login, application will be closed
        return;
    case StopInternalError:
		msg = tr("Internal error occured:\n%1").arg(message);
        break;
    case StopServerError:
		msg = tr("Server error occured:\n%1").arg(message);
        break;
    case StopInvalidCredentials:
		msg = tr("Your credentials were not accepted:\n%1").arg(message);
        break;
    }

    if (this->mainWindow != 0) {
        //Tell main window to close
		emit this->closeWindow(true);
        this->mainWindow = 0;
    }

    //Redisplay login window with the error message
	this->showLoginWindow(msg, false);
}

//Called when the client can't start the connection or when connection is broken
void CtiClientApplication::connectionLost()
{
	//If the main window is displayed, the connection has been broken,
	//otherwise it couldn't be started. We keep trying to connect either way
    if (this->mainWindow != 0) {
		emit statusChange(false);
    } else {
        //User won't be able to click OK button again until connection is established
		this->loginWindow->showMessage(
				tr("AsteriskCTI client is unable to connect to AsteriskCTI server. "
				   "Please check that correct parameters are passed from the command line.\n\n"
				   "AsteriskCTI client will keep trying to connect."), true);
        this->loginWindow->show();
    }
}

void CtiClientApplication::resetLastCtiCommand()
{
    delete this->lastCtiCommand;
    this->lastCtiCommand = 0;
}

//Creates AstCtiResponse object from string
AstCtiResponse CtiClientApplication::parseResponse(const QString& response)
{
    QStringList data = response.split(" ");
    AstCtiResponse newResponse;
    newResponse.code = (AstCtiResponseCodes)(QString(data.at(0)).toInt());
	// Remove response code
    data.removeFirst();
	// Response code is usually followed by "OK" or "KO" which we don't care about
	if (data.first() == QStringLiteral("OK") || data.first() == QStringLiteral("KO"))
		data.removeFirst();
	newResponse.data = data;
    return newResponse;
}

//Analyzes block of data received from server and sends it for further processing
void CtiClientApplication::parseDataReceivedFromServer(const QString& message)
{
    if (message.isEmpty()) {
		if (config->debug)
			qWarning() << "Received empty string from server.";
        return;
    }

    QChar firstChar = message.at(0);
	if (firstChar.isNumber())
        //First character of received data is a number, which means it is a response from server
		this->processResponse(this->parseResponse(message));
	else
		//Otherwise it is an XML object containing a message from server
		this->processXmlObject(message);
}

//Analyzes server response and takes appropriate action
//The handshake protocol is followed until authentication is complete or error occurs
void CtiClientApplication::processResponse(const AstCtiResponse &response) {
	if (config->debug)
		qDebug() << "Processing response: <<" << response.code << response.data.join(" ");

    switch (response.code) {
    case RspAuthOK:
        resetLastCtiCommand();
        sendCommandToServer(CmdKeepAlive);
        break;
    case RspCompressLevel:
        this->config->compressionLevel = response.data.last().toInt();
        resetLastCtiCommand();
		sendCommandToServer(CmdUser, this->config->userName);
        break;
    case RspDisconnectOK:
        //Nothing to do
        break;
    case RspError:
        if (this->lastCtiCommand != 0) {
            switch (this->lastCtiCommand->command) {
            case CmdUser:
            case CmdPass:
                abortConnection(StopInvalidCredentials, response.data.join(" "));
                break;
            case CmdChangePassword:
                resetLastCtiCommand();
				this->newMessage(tr("Changing of password did not succeed: %1")
									.arg(response.data.join(" ")), QSystemTrayIcon::Critical);
                break;
            case CmdMac:
            case CmdOsType:
                abortConnection(StopServerError, response.data.join(" "));
                break;
            case CmdOrig:
                //TODO
                break;
            default:
				if (config->debug)
					qDebug() << "Received unexpected response from server:"
							 << response.code << response.data.join(" ")
							 << "to command" << this->lastCtiCommand->command;
                resetLastCtiCommand();
                break;
            }
        } else {
			if (config->debug)
				qDebug() << "Received unexpected response from server:"
						 << response.code << response.data.join(" ")
						 << "to unknown command";
        }
        break;
    case RspKeepAlive:
        this->config->keepAliveInterval = response.data.last().toInt();
        //Reduce the interval to allow for network latency
        if (this->config->keepAliveInterval > 4000)
            this->config->keepAliveInterval -= 2000;
        else
            this->config->keepAliveInterval /= 2;
        resetLastCtiCommand();
		switch (this->clientType) {
		case CtiClientCallCenter:
			// Call center operators must authenticate with MAC address
			sendCommandToServer(CmdMac, this->macAddress);
			break;
		case CtiClientPhoneManager:
			sendCommandToServer(CmdOsType, osType);
			break;
		}

        break;
    case RspOK:
        if (this->lastCtiCommand != 0) {
            switch (this->lastCtiCommand->command) {
            case CmdUser:
                resetLastCtiCommand();
				sendCommandToServer(CmdPass, this->config->password);
                break;
            case CmdPass:
                resetLastCtiCommand();
				sendCommandToServer(CmdKeepAlive);
                break;
            case CmdChangePassword:
                resetLastCtiCommand();
                //Update password
				this->config->password = this->newPassword;
				this->newMessage(tr("Changing of password was successfull."),
								 QSystemTrayIcon::Information);
                break;
            case CmdMac:
				resetLastCtiCommand();
                sendCommandToServer(CmdOsType, osType);
                break;
            case CmdOsType:
                resetLastCtiCommand();
                this->connectionStatus = ConnStatusLoggedIn;
				// We wait for XML object with configuration before we show the main window
                break;
            case CmdNoOp:
                resetLastCtiCommand();
                break;
            case CmdOrig:
                //TODO
                break;            
            default:
				if (config->debug)
					qDebug() << "Received unexpected response from server:"
							 << response.code << response.data.join(" ")
							 << "to command" << this->lastCtiCommand->command;
                resetLastCtiCommand();
                break;
            }
        } else {
			if (response.data.length() > 0) {
				if (response.data.at(0) == QStringLiteral("Welcome"))
                    sendCommandToServer(CmdCompression);
            } else {
				if (config->debug)
					qDebug() << "Received unexpected response from server:"
							 << response.code << response.data.join(" ")
							 << "to unknown command";
            }
        }
        break;
    case RspPauseError:
        resetLastCtiCommand();
        emit pauseError(response.data.join(" "));
        break;
    case RspPauseOK:
        resetLastCtiCommand();
        emit pauseAccepted();
        break;
    case RspPausePending:
        //Pause is pending, it will be confirmed or denied later by RspPauseOK or RspPauseError
        resetLastCtiCommand();
        break;
    }
}

//Reads and processes XML object received from server, which can be channel or config object
void CtiClientApplication::processXmlObject(const QString& xmlString) {
	if (config->debug)
		qDebug() << "Processing xml object:\n" << xmlString;

	QXmlStreamReader reader(xmlString);

	if (reader.readNextStartElement()) {
		if (reader.name() == QStringLiteral("AsteriskChannel")) {
			AstCtiChannel* channel = new AstCtiChannel();

			channel->uniqueId = reader.attributes().value(QStringLiteral("UniqueId")).toString();

			while (reader.readNextStartElement()) {
				if (reader.name() == QStringLiteral("Event"))
					channel->lastEvent = reader.readElementText();
				else if (reader.name() == QStringLiteral("Channel"))
					channel->channel = reader.readElementText();
				else if (reader.name() == QStringLiteral("ParsedChannel"))
					channel->parsedChannel = reader.readElementText();
				else if (reader.name() == QStringLiteral("ChannelExten"))
					channel->channelExten = reader.readElementText();
				else if (reader.name() == QStringLiteral("CallerIdNum"))
					channel->callerIdNum = reader.readElementText();
				else if (reader.name() == QStringLiteral("CallerIdName"))
					channel->callerIdName = reader.readElementText();
				else if (reader.name() == QStringLiteral("Context"))
					channel->context = reader.readElementText();
				else if (reader.name() == QStringLiteral("DialedLineNum"))
					channel->dialedLineNum = reader.readElementText();
				else if (reader.name() == QStringLiteral("ConnectedLineNum"))
					channel->connectedLineNum = reader.readElementText();
				else if (reader.name() == QStringLiteral("Queue"))
					channel->queue = reader.readElementText();
				else if (reader.name() == QStringLiteral("State"))
					channel->state = reader.readElementText();
				else if (reader.name() == QStringLiteral("AccountCode"))
					channel->accountCode = reader.readElementText();
				else if (reader.name() == QStringLiteral("MusicOnHoldState"))
					channel->musicOnHoldState = reader.readElementText();
				else if (reader.name() == QStringLiteral("HangupCause"))
					channel->hangupCause = reader.readElementText();
				else if (reader.name() == QStringLiteral("BridgeId"))
					channel->bridgeId = reader.readElementText().toInt();
				else if (reader.name() == QStringLiteral("Variables"))
					while (reader.readNextStartElement())
						channel->variables.insert(reader.name().toString(),
												 reader.readElementText());
				else if (reader.name() == QStringLiteral("Actions"))
					while (reader.readNextStartElement()) {
						AstCtiActionPtr action = AstCtiActionPtr(new AstCtiAction);
						if (reader.name() == QStringLiteral("Application"))
							action->type = ActionTypeApplication;
						else if (reader.name() == QStringLiteral("Browser"))
							action->type = ActionTypeExternalBrowser;
						else if (reader.name() == QStringLiteral("InternalBrowser"))
							action->type = ActionTypeInternalBrowser;
						else if (reader.name() == QStringLiteral("TcpMessage"))
							action->type = ActionTypeTcpMessage;
						else if (reader.name() == QStringLiteral("UdpMessage"))
							action->type = ActionTypeUdpMessage;

						while (reader.readNextStartElement()) {
							if (reader.name() == QStringLiteral("Destination"))
								action->destination = reader.readElementText();
							else if (reader.name() == QStringLiteral("Parameters"))
								action->destination = reader.readElementText();
							else if (reader.name() == QStringLiteral("Encoding"))
								action->encoding = reader.readElementText();
						}

						channel->actions.append(action);
					}
			}

			//Let main window know about the event
			//TODO: Perhaps process event (or some events) here?
			emit channelEventReceived(channel);
		} else if (reader.name() == QStringLiteral("Operator")) {
			config->fullName = reader.attributes().value(QStringLiteral("FullName")).toString();
			while (reader.readNextStartElement()) {
				if (reader.name() == QStringLiteral("IsCallCenter"))
					config->isCallCenter = reader.readElementText().toInt() != 0;
				else if (reader.name() == QStringLiteral("BeginInPause"))
					config->beginInPause = reader.readElementText().toInt() != 0;
				else if (reader.name() == QStringLiteral("CanMonitor"))
					config->canMonitor = reader.readElementText().toInt() != 0;
				else if (reader.name() == QStringLiteral("CanAlterSpeedDials"))
					config->canAlterSpeedDials = reader.readElementText().toInt() != 0;
				else if (reader.name() == QStringLiteral("CanRecord"))
					config->canRecord = reader.readElementText().toInt() != 0;
				else if (reader.name() == QStringLiteral("Extensions"))
					while (reader.readNextStartElement()) {
						AstCtiExtensionPtr extension = AstCtiExtensionPtr(new AstCtiExtension);
						extension->number =
								reader.attributes().value(QStringLiteral("Number")).toString();
						while (reader.readNextStartElement()) {
							if (reader.name() == QStringLiteral("Channel"))
								extension->channel = reader.readElementText();
							else if (reader.name() == QStringLiteral("Name"))
								extension->name = reader.readElementText();
							else if (reader.name() == QStringLiteral("CanAutoAnswer"))
								extension->canAutoAnswer = reader.readElementText().toInt() != 0;
							else if (reader.name() == QStringLiteral("LoggedInQueue"))
								extension->loggedInQueue = reader.readElementText().toInt() != 0;
							else if (reader.name() == QStringLiteral("Status"))
								extension->status =
										(AstCtiExtensionStatus)reader.readElementText().toInt();
						}
						config->extensions.append(extension);
					}
				else if (reader.name() == QStringLiteral("SpeedDials"))
					while (reader.readNextStartElement()) {
						AstCtiSpeedDialPtr speedDial = AstCtiSpeedDialPtr(new AstCtiSpeedDial);
						while (reader.readNextStartElement()) {
							if (reader.name() == QStringLiteral("GroupName"))
								speedDial->groupName = reader.readElementText();
							else if (reader.name() == QStringLiteral("Name"))
								speedDial->name = reader.readElementText();
							else if (reader.name() == QStringLiteral("Number"))
								speedDial->number = reader.readElementText();
							else if (reader.name() == QStringLiteral("BLF"))
								speedDial->isBlf = reader.readElementText().toInt() != 0;
							else if (reader.name() == QStringLiteral("Order"))
								speedDial->order = reader.readElementText().toShort();
							else if (reader.name() == QStringLiteral("ExtensionStatus"))
								speedDial->extensionStatus =
										(AstCtiExtensionStatus)reader.readElementText().toInt();
						}
						// For key, we use groupName:order, with order left padded with zeros
						const QString key = QString("%1:%2")
											.arg(speedDial->groupName)
											.arg(speedDial->order, 3, 10, QChar('0'));
						config->speedDials.insert(key, speedDial);
					}
				else if (reader.name() == QStringLiteral("Services"))
					while (reader.readNextStartElement()) {
						QString name = "";
						QString serviceType = "";
						// This data is informational. User is informed about the service name
						// and type, which can be "Inbound", "Outbound" or "Queue"
						while (reader.readNextStartElement()) {
							if (reader.name() == QStringLiteral("Name"))
								name = reader.readElementText();
							else if (reader.name() == QStringLiteral("Queue"))
								serviceType = "Queue";
							else if (reader.name() == QStringLiteral("ContextType"))
								if (serviceType.length() == 0)
									serviceType = reader.readElementText();
						}
						config->services.insert(name, serviceType);
					}
			}
			this->showMainWindow();
		}
	}
}

//Prepares data for sending to server, using compression if so indicated
QByteArray CtiClientApplication::convertDataForSending(const QString& data)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
	//out.setVersion(QDataStream::Qt_4_5);

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
void CtiClientApplication::sendDataToServer(const QString& data)
{
	if (config->debug)
		qDebug() << "Sending to server:   >>" << data.simplified();

	this->localSocket->write(this->convertDataForSending(data));
    this->localSocket->flush();

    //Restart the idle timer
    this->idleTimer->start(this->config->keepAliveInterval);
}

//Sends a parameterless command to server (overloaded)
void CtiClientApplication::sendCommandToServer(const AstCtiCommands command)
{
	this->sendCommandToServer(command, QString());
}

//Sends a command to server
void CtiClientApplication::sendCommandToServer(const AstCtiCommands command,
											   const QString& parameters)
{
    if (this->lastCtiCommand != 0) {
        //New command was requested before we received a response to last command
        //This should not happen, it could indicate a bug in the client or server
		qCritical() << "New command ("  << this->getCommandName(command)
					<< ") requested before receiving response from server to command"
					<< this->getCommandName(this->lastCtiCommand->command);

        //We exit without sending the command
        return;
    }
// Test
    //Create and remember command object
    AstCtiCommand* cmd = new AstCtiCommand();
    cmd->command = command;
    cmd->parameters = parameters;
    this->lastCtiCommand = cmd;

    //Convert command to string and send to server
	QString data = this->getCommandName(cmd->command);
    if (!parameters.isEmpty())
		data.append(" ").append(parameters);

	this->sendDataToServer(data);
}

//Displays the login window
//Called by connectTimer for the first time
//Later it is called when the user logs off
void CtiClientApplication::showLoginWindow(const QString& message, const bool connectionLost)
{
	//Create only if it doesn't exist
	if (this->loginWindow == 0) {
		this->loginWindow = new LoginWindow();

		connect(this->loginWindow, SIGNAL(accepted(QString, QString)),
				this, SLOT(loginAccept(QString, QString)));
		connect(this->loginWindow, SIGNAL(rejected()),
				this, SLOT(loginReject()));
	}

	//Set activation window for QtSingleApplication
	this->setActivationWindow(this->loginWindow, true);

	this->loginWindow->showMessage(message, connectionLost);
	this->loginWindow->show();
}

//Displays the main window
void CtiClientApplication::showMainWindow()
{
    if (this->mainWindow == 0) {
		// Main window was not displayed yet, we create it and show it
        switch (this->clientType) {
        case CtiClientPhoneManager:
			this->mainWindow = new ManagerWindow(this->config);
            break;
        default:
			this->mainWindow = new CompactWindow(this->config);
        }

		connect(this, SIGNAL(newMessage(QString,QSystemTrayIcon::MessageIcon)),
				this->mainWindow, SLOT(showMessage(QString,QSystemTrayIcon::MessageIcon)));
		connect(this, SIGNAL(newConfig(AstCtiConfiguration*)),
				this->mainWindow, SLOT(newConfig(AstCtiConfiguration*)));
		connect(this, SIGNAL(channelEventReceived(AstCtiChannel*)),
				this->mainWindow, SLOT(receiveChannelEvent(AstCtiChannel*)));
		connect(this, SIGNAL(statusChange(bool)),
				this->mainWindow, SLOT(setStatus(bool)));
		connect(this, SIGNAL(closeWindow(bool)),
				this->mainWindow, SLOT(quit(bool)));
		connect(this, SIGNAL(pauseAccepted()),
				this->mainWindow, SLOT(pauseAccepted()));
		connect(this, SIGNAL(pauseError(QString)),
				this->mainWindow, SLOT(pauseError(QString)));

		connect(this->mainWindow, SIGNAL(logOff()),
				this, SLOT(logOff()));
		connect(this->mainWindow, SIGNAL(changePassword()),
				this, SLOT(changePassword()));
		connect(this->mainWindow, SIGNAL(pauseRequest(bool)),
				this, SLOT(pause(bool)));

		this->mainWindow->show();
	} else {
		// Main window already exists, we inform it about the change in configuration
		emit this->newConfig(this->config);
	}

    //Set activation window for QtSingleApplication
    this->setActivationWindow(this->mainWindow, true);

	if (this->loginWindow->isVisible())
        this->loginWindow->hide();

    emit statusChange(true);
}

//Displays the internal browser with the specified URL
void CtiClientApplication::newBrowserWindow(QUrl url)
{
	BrowserWindow* browser = new BrowserWindow(this->config->userName, url, 0);
    this->browserWindows.append(browser);
	connect(browser, SIGNAL(windowClosing(BrowserWindow*)),
			this, SLOT(browserWindowClosed(BrowserWindow*)));
	connect(browser, SIGNAL(linkClicked(QUrl)),
			this, SLOT(newBrowserWindow(QUrl)));
    browser->show();
}

//Starts a new application
void CtiClientApplication::newApplication(const QString& path, const QString& parameters)
{
    //We keep track of started applications by their processes
	//When main window closes, all QProcess objects will be destroyed,
	//thus closing the applications
    QProcess* process = new QProcess();
    this->applications.append(process);
	connect(process, SIGNAL(destroyed(QObject*)),
			this, SLOT(applicationClosed(QProcess*)));
    process->start(path, QStringList(parameters), QIODevice::ReadOnly);
}

//Slot which receives socket's connected signal
void CtiClientApplication::socketConnected()
{
	//We are connected, so connect timer is no longer necessary
    this->connectTimer->stop();

    //Get MAC Address (necessary for authentication with server)
    this->macAddress = "";
    QHostAddress localAddress = this->localSocket->localAddress();
    if (localAddress == QHostAddress::LocalHost) {
		this->macAddress = QStringLiteral("00:00:00:00:00:00");
    } else {
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
    }
    if (this->macAddress.isEmpty()) {
        qCritical() << "Unable to determine local MAC address. Aborting.";
		abortConnection(StopInternalError, tr("Unable to determine local MAC address."));
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
    resetLastCtiCommand();

    if (this->connectionStatus != ConnStatusDisconnected) {
		if (config->debug)
			qDebug() << "Conection to server lost. I will keep trying to reconnect at"
					 << this->config->connectRetryInterval << "second intervals.";

        //Inform user
		this->connectionLost();

        //Retry connection
		this->connectSocket();
    }
}

//Slot which receives socket's stateChanged signal
void CtiClientApplication::socketStateChanged(QAbstractSocket::SocketState socketState)
{
    if (config->debug) {
        QString newState;
        switch(socketState) {
        case QAbstractSocket::UnconnectedState:
			newState = QStringLiteral("UnconnectedState");
            break;
        case QAbstractSocket::HostLookupState:
			newState = QStringLiteral("HostLookupState");
            break;
        case QAbstractSocket::ConnectingState:
			newState = QStringLiteral("ConnectingState");
            break;
        case QAbstractSocket::ConnectedState:
			newState = QStringLiteral("ConnectedState");
            break;
        case QAbstractSocket::BoundState:
			newState = QStringLiteral("BoundState");
            break;
        case QAbstractSocket::ClosingState:
			newState = QStringLiteral("ClosingState");
            break;
        case QAbstractSocket::ListeningState:
			newState = QStringLiteral("ListeningState");
            break;
        }
        qDebug() << "Socket state changed: " << newState;
    }
}

//Slot which receives socket's error signal
void CtiClientApplication::socketError(QAbstractSocket::SocketError error)
{
	if (error == QAbstractSocket::SocketTimeoutError) {
        if (this->connectionStatus == ConnStatusConnecting) {
            //Notify user only about first reconnect attempt
            if (this->reconnectNotify) {
				if (config->debug)
					qDebug() << "Unable to connect to server. I will keep trying to connect at"
							 << this->config->connectRetryInterval << "second intervals.";

				this->connectionLost();

                this->reconnectNotify = false;
            }
        }

        this->connectionStatus = ConnStatusDisconnected;

        this->localSocket->abort();

        //Restart connect timer so it reinitiates connection after a specified interval
        this->connectTimer->start(this->config->connectRetryInterval);
    } else {
		if (config->debug)
			qDebug() << "Socket Error:" << error << this->localSocket->errorString();
    }
}

//Slot which receives socket's readyRead signal
void CtiClientApplication::receiveData()
{
    QDataStream in(this->localSocket);
	//in.setVersion(QDataStream::Qt_4_5);

    while (!in.atEnd()) {
        if (this->blockSize == 0) {
            //Read data block size from frame descriptor (if enough data available)
            if (this->localSocket->bytesAvailable() >= (int)sizeof(quint16))
                in >> this->blockSize;
        }

        //Wait for entire data block to be received
        //If not, readyRead will be emitted again when more data is available
		if (this->blockSize > 0 && this->localSocket->bytesAvailable() >= this->blockSize) {
			QByteArray receivedData;
			QString receivedString;
            if (this->config->compressionLevel > 0) {
                //Compressed data will be UTF8 encoded
				in >> receivedData;
				QByteArray uncompressedData = qUncompress(receivedData);
                if (uncompressedData.isEmpty()) {
					qCritical() << "Unable to uncompress data received from server. "
								   "Trying to use uncompressed data.";
					receivedString = QString::fromUtf8(receivedData);
                } else {
					receivedString = QString::fromUtf8(uncompressedData);
                }
            } else {
                //Uncompressed data will be sent as-is
				in >> receivedString;
            }

            //Reset block size
            this->blockSize = 0;

            //Analyze received data
			parseDataReceivedFromServer(receivedString);
        }
    }

    //Restart the idle timer
    this->idleTimer->start(this->config->keepAliveInterval);
}

//Called when connection has been idle for a specified period
void CtiClientApplication::idleTimerElapsed()
{
    if (this->lastCtiCommand != 0) {
        //Idle timer elapsed before we received a response to last command
        //This should not happen, it probably indicates a bug in the server
		qCritical() << "Idle timer elapsed before receiving response from server to command"
					<< this->getCommandName(this->lastCtiCommand->command);

		//We exit without sending keep-alive signal,
		//this will probably result in server dropping the connection
        return;
    }

    //Send NoOp command to server to keep connection alive
    if (this->localSocket->state() == QAbstractSocket::ConnectedState) {
        if (this->connectionStatus == ConnStatusLoggedIn)
            sendCommandToServer(CmdNoOp);
    }
}

//Called when given connection timeout or connection retry interval has elapsed
//Appropriate action will be taken depending on connection state
//(see connectSocket() and socketError())
void CtiClientApplication::connectTimerElapsed()
{
    if (this->connectionStatus == ConnStatusConnecting) {
        //Simulate socket timeout error
		this->socketError(QAbstractSocket::SocketTimeoutError);
    } else {
		if (this->loginWindow == 0)
			//We show the login window the first time this is called
			this->showLoginWindow(
					tr("Welcome to Asterisk CTI!/n/nPlease enter your credentials to continue."),
					false);
		else
			//Retry connection
			this->connectSocket();
    }
}

//Returns a string representation of CTI command
QString CtiClientApplication::getCommandName(const AstCtiCommands command)
{
	//We use a variable to exploit NRVO
	QString commandName;

	switch (command) {
	case CmdQuit:
		commandName = QStringLiteral("QUIT");
		break;
	case CmdCompression:
		commandName = QStringLiteral("CMPR");
		break;
	case CmdKeepAlive:
		commandName = QStringLiteral("KEEP");
		break;
	case CmdNoOp:
		commandName = QStringLiteral("NOOP");
		break;
	case CmdUser:
		commandName = QStringLiteral("USER");
		break;
	case CmdPass:
		commandName = QStringLiteral("PASS");
		break;
	case CmdChangePassword:
		commandName = QStringLiteral("CHPW");
		break;
	case CmdOsType:
		commandName = QStringLiteral("OSTYPE");
		break;
	case CmdPause:
		commandName = QStringLiteral("PAUSE");
		break;
	case CmdOrig:
		commandName = QStringLiteral("ORIG");
		break;
	case CmdMac:
		commandName = QStringLiteral("MAC");
		break;
	default:
		commandName = QStringLiteral("");
		break;
	}

	return commandName;
}
