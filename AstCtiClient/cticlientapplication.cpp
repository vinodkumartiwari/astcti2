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

	QString serverHost = args.getSwitchArg(QStringLiteral("h"), defaultServerHost);
	if (serverHost.isEmpty()) {
		qCritical() << "No value given for Server Host (-h). If you want to use default host, "
					   "don't include the command line switch.";
        return;
    }
    bool ok;
	quint16 serverPort = args.getSwitchArg(QStringLiteral("p"), defaultServerPort).toUShort(&ok);
    if (!ok) {
		qCritical() << "Wrong value given for Server Port (-p). Expected number from 0 to 65535.";
        return;
    }
	int connectTimeout = args.getSwitchArg(QStringLiteral("t"), defaultConnectTimeout).toInt(&ok);
	if (!ok || connectTimeout < -1) {
		qCritical() << "Wrong value given for Connect Timeout (-t). "
					   "Expected number from -1 to 2147483647.";
        return;
    }
	int connectRetryInterval =
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

	this->tcpClient = new AstCtiTcpClient(serverHost, serverPort, connectTimeout,
										  connectRetryInterval, this->config->debug);
	qRegisterMetaType<AstCtiErrorCodes>("AstCtiErrorCodes");
	qRegisterMetaType<AstCtiCommands>("AstCtiCommands");
	connect(this->tcpClient, SIGNAL(connectionLost()),
			this, SLOT(connectionLost()));
	connect(this->tcpClient, SIGNAL(serverMessage(QString)),
			this, SLOT(processXmlObject(QString)));
	connect(this->tcpClient, SIGNAL(serverResponse(AstCtiErrorCodes,AstCtiCommands,QStringList)),
			this, SLOT(processResponse(AstCtiErrorCodes,AstCtiCommands,QStringList)));
	connect(this, SIGNAL(newCommand(AstCtiCommands,QStringList)),
			this->tcpClient, SLOT(sendCommand(AstCtiCommands,QStringList)));
	connect(this, SIGNAL(startClient(QString,QString)),
			this->tcpClient, SLOT(start(QString,QString)));

	QThread* clientThread = new QThread(this);
	this->tcpClient->moveToThread(clientThread);
	connect(clientThread, SIGNAL(finished()),
			this->tcpClient, SLOT(deleteLater()));
	clientThread->start();

	// We need a timer to kick off the application, because no use interaction is possible
	// before application's event loop is started
	this->startTimer = new QTimer(this);
	connect(this->startTimer, SIGNAL(timeout()),
			this, SLOT(start()));
	this->startTimer->setSingleShot(true);
	//Timer will not start before application enters event loop
	this->startTimer->start(0);

	this->loginWindow = 0;
	this->mainWindow = 0;

    //All checks passed, we are good to go
    canStart = true;
}

CtiClientApplication::~CtiClientApplication()
{
	// Client will be deleted automatically because the thread's
	// finished() signal is connected to client's deleteLater() slot
	QThread* clientThread = this->tcpClient->thread();
	clientThread->quit();
	clientThread->wait();
	delete clientThread;

	delete this->config;
	delete this->loginWindow;
	delete this->mainWindow;
    qDeleteAll(this->applications);
    qDeleteAll(this->browserWindows);
}

const bool CtiClientApplication::getCanStart() const
{
	return this->canStart;
}

void CtiClientApplication::start()
{
	// We don't need the timer any more
	delete this->startTimer;

	//We show the login window to start the application
	this->showLoginWindow(
			tr("Welcome to Asterisk CTI! \n\nPlease enter your credentials to continue."), false);
}

//Called when connection attempt is definitively aborted due to error or user disconnect
void CtiClientApplication::abortConnection(const QString& message) {
	if (config->debug)
		if (!message.isEmpty())
			qDebug() << "Connection to server closed. Reason:" << message;

	if (this->mainWindow != 0) {
		//Tell main window to close
		emit this->closeWindow(true);
		this->mainWindow = 0;
	}

	if (!message.isEmpty())
		//Redisplay login window with the error message
		this->showLoginWindow(message, false);
}

//Called when the user clicks OK in the login window
void CtiClientApplication::loginAccept(const QString& userName, const QString& password)
{
	this->config->userName = userName;
	emit this->startClient(userName, password);
}

//Called when the user clicks Cancel in the login window
void CtiClientApplication::loginReject()
{
	QApplication::processEvents(QEventLoop::AllEvents);
    //Close the application
	this->quit();
}

//Called when the user clicks 'Start' in the main window
void CtiClientApplication::agentStart(const QString& queue, const QString& channelName)
{
	emit this->newCommand(CmdStart, QStringList() << channelName << queue);
}

//Called when the user clicks 'Pause' in the main window
void CtiClientApplication::agentPause(const QString& queue, const QString& channelName)
{
	emit this->newCommand(CmdPause, QStringList() << channelName << queue);
}

//Called when the user clicks 'Change password' in the main window
void CtiClientApplication::changePassword(){
    PasswordWindow* w = new PasswordWindow(this->mainWindow);
	if (w->exec() == QDialog::Accepted)
		emit this->newCommand(CmdChangePassword, QStringList() << w->oldPass << w->newPass);

    //TEMP
	//this->newBrowserWindow(QUrl("http://www.google.com"));
}

//Called when the user logs off in the main window
void CtiClientApplication::logOff()
{
    //Let the server know that the user has logged off
	//emit this->newCommand(CmdQuit, QStringList());

    //Close all open applications (application is closed when QProcess is destroyed)
    qDeleteAll(this->applications);
    this->applications.clear();

	//Close all browser windows (window is automatically removed from array when closed,
	//see browserWindowClosed())
	for (int i = this->browserWindows.count() - 1; i > -1; i--)
        this->browserWindows[i]->close();

	this->mainWindow = 0;

    //We don't wait for server to acknowledge user logoff
    //If connection is dropped before that, the server will log user off automatically
	this->abortConnection(QStringLiteral(""));
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

//Reads and processes XML object received from server, which can be one of the following:
//Config
//Channel
//Extension status
//Agent status
//Disconnect
void CtiClientApplication::processXmlObject(const QString& xmlString) {
	if (config->debug)
		qDebug() << "Processing xml object:" << xmlString;

	QXmlStreamReader reader(xmlString);

	if (reader.readNextStartElement()) {
		if (reader.name() == QStringLiteral("Operator")) {
			this->config->fullName =
					reader.attributes().value(QStringLiteral("FullName")).toString();
			while (reader.readNextStartElement()) {
				if (reader.name() == QStringLiteral("IsCallCenter"))
					this->config->isCallCenter = reader.readElementText().toInt() != 0;
				else if (reader.name() == QStringLiteral("BeginInPause"))
					this->config->beginInPause = reader.readElementText().toInt() != 0;
				else if (reader.name() == QStringLiteral("CanMonitor"))
					this->config->canMonitor = reader.readElementText().toInt() != 0;
				else if (reader.name() == QStringLiteral("CanAlterSpeedDials"))
					this->config->canAlterSpeedDials = reader.readElementText().toInt() != 0;
				else if (reader.name() == QStringLiteral("CanRecord"))
					this->config->canRecord = reader.readElementText().toInt() != 0;
				else if (reader.name() == QStringLiteral("Extensions"))
					while (reader.readNextStartElement()) {
						AstCtiExtensionPtr extension = AstCtiExtensionPtr(new AstCtiExtension);
						extension->number =
								reader.attributes().value(QStringLiteral("Number")).toString();
						while (reader.readNextStartElement()) {
							if (reader.name() == QStringLiteral("Channel"))
								extension->channelName = reader.readElementText();
							else if (reader.name() == QStringLiteral("Name"))
								extension->name = reader.readElementText();
							else if (reader.name() == QStringLiteral("CanAutoAnswer"))
								extension->canAutoAnswer = reader.readElementText().toInt() != 0;
							else if (reader.name() == QStringLiteral("Status"))
								extension->status =
										(AstCtiExtensionStatus)reader.readElementText().toInt();
							else if (reader.name() == QStringLiteral("Queues"))
								while (reader.readNextStartElement()) {
									QString queueName = reader.attributes().value(
															QStringLiteral("Name")).toString();
									extension->queues.insert(queueName,
											(AstCtiAgentStatus)reader.readElementText().toInt());
								}
						}
						this->config->extensions.append(extension);
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
						this->config->speedDials.insert(key, speedDial);
					}
				else if (reader.name() == QStringLiteral("Services"))
					while (reader.readNextStartElement()) {
						QString name = QStringLiteral("");
						QString serviceType = QStringLiteral("");
						// This data is informational. User is informed about the service name
						// and type, which can be "Inbound", "Outbound" or "Queue"
						while (reader.readNextStartElement()) {
							if (reader.name() == QStringLiteral("Name")) {
								name = reader.readElementText();
							} else if (reader.name() == QStringLiteral("QueueName")) {
								if (reader.readElementText().length() > 0)
									serviceType = "Queue";
							} else if (reader.name() == QStringLiteral("ContextType")) {
								if (serviceType.length() == 0)
									serviceType = reader.readElementText();
								else
									// We must read element text in order to progress
									reader.readElementText();
							}
						}
						this->config->services.insert(name, serviceType);
					}
			}

			this->showMainWindow();
		} else if (reader.name() == QStringLiteral("AsteriskChannel")) {
			AstCtiChannel* channel = new AstCtiChannel();

			channel->uniqueId = reader.attributes().value(QStringLiteral("UniqueId")).toString();

			while (reader.readNextStartElement()) {
				if (reader.name() == QStringLiteral("Event"))
					channel->lastEvent = reader.readElementText();
				else if (reader.name() == QStringLiteral("Channel"))
					channel->channelId = reader.readElementText();
				else if (reader.name() == QStringLiteral("ParsedChannel"))
					channel->channelName = reader.readElementText();
				else if (reader.name() == QStringLiteral("ChannelExten"))
					channel->number = reader.readElementText();
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
			emit this->channelEventReceived(channel);
		} else if (reader.name() == QStringLiteral("AgentStatus")) {
			const QString queue =
					reader.attributes().value(QStringLiteral("Queue")).toString();
			const QString channelName =
					reader.attributes().value(QStringLiteral("ChannelName")).toString();
			const AstCtiAgentStatus status = (AstCtiAgentStatus)reader.readElementText().toInt();
			const int listSize = this->config->extensions.size();
			if (status != AgentStatusLoginFailed && status != AgentStatusPauseFailed) {
				for (int i = 0; i < listSize; i++) {
					AstCtiExtensionPtr extension = this->config->extensions.at(i);
					if (extension->channelName == channelName) {
						if (extension->queues.contains(queue))
							extension->queues.insert(queue, status);
						break;
					}
				}
			}
			//Let main window know about the event
			emit this->agentStatusChanged(queue, channelName, status);
		} else if (reader.name() == QStringLiteral("ExtensionStatus")) {
			//TODO
		} else if (reader.name() == QStringLiteral("Disconnect")) {
			const QString message = tr("Server has closed the connection. You need to log in again."
									   " If you can't log in with your current credentials, please"
									   " contact system administrator.");
			this->showInfoMessage(message, QMessageBox::Critical);
			this->abortConnection(message);
		}
	}
}

void CtiClientApplication::processResponse(AstCtiErrorCodes errorCode, AstCtiCommands command,
										   const QStringList& parameters)
{
	if (errorCode == ErrNotDefined) {
		if (command == CmdChangePassword)
			this->newMessage(tr("Changing of password was successful."),
							 QSystemTrayIcon::Information);
	} else {
		switch (command) {
		case CmdLogin:
			this->abortConnection(this->getErrorText(errorCode));
			break;
		case CmdChangePassword:
			this->newMessage(tr("Changing of password was not successful."),
							 QSystemTrayIcon::Critical);
			break;
		case CmdOriginate:
			// This should not happen, it indicates a bug in the client or server
			//TODO
			break;
		case CmdConference:
			// This should not happen, it indicates a bug in the client or server
			//TODO
			break;
		case CmdStart:
			// This should not happen, it indicates a bug in the client or server
			emit this->agentStatusChanged(parameters.at(0), parameters.at(1),
										  AgentStatusLoginFailed);
			break;
		case CmdPause:
			// This should not happen, it indicates a bug in the client or server
			emit this->agentStatusChanged(parameters.at(0), parameters.at(1),
										  AgentStatusPauseFailed);
			break;
		}
	}
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
			if (!this->config->isCallCenter) {
				this->showInfoMessage(tr("This client layout can only be used for call-center "
										 "operators. Please log in as a different user "
										 "or choose another layout."), QMessageBox::Critical);

				this->logOff();

				return;
			}
			if (this->config->extensions.size() > 1) {
				this->showInfoMessage(tr("Multiple extensions are not supported for this client "
										 "layout. Please log in as a different user "
										 "or choose another layout."), QMessageBox::Critical);

				this->logOff();

				return;
			}
			this->mainWindow = new CompactWindow(this->config);
			break;
        }

		connect(this, SIGNAL(newMessage(QString,QSystemTrayIcon::MessageIcon)),
				this->mainWindow, SLOT(showMessage(QString,QSystemTrayIcon::MessageIcon)));
		connect(this, SIGNAL(newConfig(AstCtiConfiguration*)),
				this->mainWindow, SLOT(newConfig(AstCtiConfiguration*)));
		connect(this, SIGNAL(channelEventReceived(AstCtiChannel*)),
				this->mainWindow, SLOT(handleChannelEvent(AstCtiChannel*)));
		connect(this, SIGNAL(statusChange(bool)),
				this->mainWindow, SLOT(setStatus(bool)));
		connect(this, SIGNAL(closeWindow(bool)),
				this->mainWindow, SLOT(quit(bool)));
		connect(this, SIGNAL(agentStatusChanged(QString,QString,AstCtiAgentStatus)),
				this->mainWindow, SLOT(agentStatusChanged(QString,QString,AstCtiAgentStatus)));

		connect(this->mainWindow, SIGNAL(logOff()),
				this, SLOT(logOff()));
		connect(this->mainWindow, SIGNAL(changePassword()),
				this, SLOT(changePassword()));
		connect(this->mainWindow, SIGNAL(startRequest(QString,QString)),
				this, SLOT(agentStart(QString,QString)));
		connect(this->mainWindow, SIGNAL(pauseRequest(QString,QString)),
				this, SLOT(agentPause(QString,QString)));

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
void CtiClientApplication::newBrowserWindow(const QUrl& url)
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

void CtiClientApplication::showInfoMessage(const QString& message, QMessageBox::Icon icon)
{
	QMessageBox msgBox;

	msgBox.setText(message);
	QAbstractButton* okBtn = msgBox.addButton(tr("OK"), QMessageBox::AcceptRole);
	okBtn->setIcon(QIcon(QPixmap(QStringLiteral(":/res/res/ok.png"))));

	msgBox.setIcon(icon);
	msgBox.exec();
}

const QString CtiClientApplication::getErrorText(const AstCtiErrorCodes error)
{
	//We use a variable to exploit NRVO
	QString errorText;

	switch (error) {
	case ErrUnknownCmd:
		errorText = tr("Unknown command.");
		break;
	case ErrWrongParam:
		errorText = tr("Wrong number of parameters.");
		break;
	case ErrNoAuth:
		errorText = tr("The client is not authenticated.");
		break;
	case ErrUserLoggedIn:
		errorText = tr("Client with this user name is already logged in.");
		break;
	case ErrWrongCreds:
		errorText = tr("Your user name or password was not accepted by the server.");
		break;
	case ErrUnknownOs:
		errorText = tr("Unknown operating system.");
		break;
	case ErrWrongMac:
		errorText = tr("Your MAC address was not recognized by the server.");
		break;
	case ErrSeatTaken:
		errorText = tr("Requested seat is already taken by another user.");
		break;
	case ErrPassChgFail:
		errorText = tr("Changing of password did not succeed due to server error.");
		break;
	case ErrUnknownChan:
		errorText = tr("Unknown channel.");
		break;
	default:
		errorText = QStringLiteral("");
		break;
	}

	return errorText;
}
