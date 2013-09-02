/* Copyright (C) 2007-2013 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * Copyright (C) 2007-2013 Lumiss d.o.o.
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

#include "astctitcpclient.h"

AstCtiTcpClient::AstCtiTcpClient(const QString& serverHost, const quint16 serverPort,
								 const int connectTimeout, const quint32 connectRetryInterval,
								 const bool debug, QObject *parent)
	: QObject(parent)
{
	this->serverHost = serverHost;
	this->serverPort = serverPort;
	this->connectTimeout = connectTimeout;
	this->connectRetryInterval = connectRetryInterval;
	this->debug = debug;

	//Set defaults
	//No compression initially. Compression level will be received from server.
	this->compressionLevel = 0;
	//Set to default initially. Interval will be received from server.
	this->keepAliveInterval = defaultKeepAliveInterval;
	this->connectionStatus = ConnStatusDisconnected;
	this->lastCtiCommand = 0;
	this->blockSize = 0;
	this->reconnectNotify = true;

	this->idleTimer = 0;
	this->connectTimer = 0;
	this->localSocket = 0;
}

AstCtiTcpClient::~AstCtiTcpClient()
{
	//Cleanup
	if (this->idleTimer != 0)
		this->idleTimer->stop();
	if (this->connectTimer != 0)
		this->connectTimer->stop();
	this->abortConnection();
	delete this->idleTimer;
	delete this->connectTimer;
	delete this->localSocket;
}

void AstCtiTcpClient::start(const QString &userName, const QString &password)
{
	// We must allocate heap objects here, because if we do it in a constructor,
	// allocation would be performed on the main thread,
	// meaning that the newly created objects are then owned by the main thread
	// and not the new thread managed by QThread
	if (this->localSocket == 0) {
		//Initialize timers
		this->idleTimer = new QTimer(this);
		this->connectTimer = new QTimer(this);
		connect(this->idleTimer, SIGNAL(timeout()),
				this, SLOT(idleTimerElapsed()));
		connect(this->connectTimer, SIGNAL(timeout()),
				this, SLOT(connectTimerElapsed()));
		this->connectTimer->setSingleShot(true);

		//Create socket for communication with server
		this->localSocket = new QTcpSocket();

		qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
		qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
		connect(this->localSocket, SIGNAL(connected()),
				this, SLOT(socketConnected()));
		connect(this->localSocket, SIGNAL(readyRead()),
				this, SLOT(receiveData()));
		connect(this->localSocket, SIGNAL(disconnected()),
				this, SLOT(socketDisconnected()));
		connect(this->localSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
				this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
		connect(this->localSocket, SIGNAL(error(QAbstractSocket::SocketError)),
				this, SLOT(socketError(QAbstractSocket::SocketError)));
	}

	this->userName = userName;
	this->password = password;

	this->abortConnection();

	this->connectSocket();
}

// Send a command to server
void AstCtiTcpClient::sendCommand(AstCtiCommands command, const QStringList& parameters)
{
	AstCtiCommand* cmd = new AstCtiCommand();
	cmd->command = command;
	cmd->parameters = parameters;

	if (this->lastCtiCommand != 0) {
		//New command was requested before we received a response to last command
		//We add new command to the queue
		this->commandQueue.enqueue(cmd);

		//We exit without sending the command
		return;
	}

	this->sendCommand(cmd);
}

// Send a command object to server
void AstCtiTcpClient::sendCommand(AstCtiCommand* const cmd)
{
	//Remember command object
	this->lastCtiCommand = cmd;

	//Convert command to string and send to server
	QString data = this->getCommandName(cmd->command);
	if (!cmd->parameters.isEmpty())
		data.append(QStringLiteral(" ")).append(cmd->parameters.join(' '));

	this->sendDataToServer(data);
}

//Initiates TCP connection
void AstCtiTcpClient::connectSocket()
{
	this->connectionStatus = ConnStatusConnecting;

	//Start connection with server. The connection will continue asynchronously,
	//using signals and slots
	this->localSocket->connectToHost(this->serverHost, this->serverPort);

	//If connection times out, socket will emit error signal with SocketTimeoutError
	//Unfortunately, currently (Qt 5.0) it is not possible to change timeout value
	//(hardcoded to 30 sec)
	//So we start our own timer set to configured timeout value
	//Connection attempt will be aborted either by socket timeout or by timer, whichever comes first
	//Then this timer will be started again, now with the value of connectRetryInterval
	//(see socketDisconnected())
	//So the timer will either drop or start the conection, based on connection state
	this->connectTimer->start(this->connectTimeout);
}

//Called when connection attempt is definitively aborted due to error or user disconnect
void AstCtiTcpClient::abortConnection() {
	if (this->localSocket != 0) {
		const ServerConnectionStatus tempStatus = this->connectionStatus;
		this->connectionStatus = ConnStatusDisconnected;

		this->commandQueue.clear();
		this->resetLastCtiCommand();

		if (tempStatus == ConnStatusLoggedIn)
			this->sendCommand(CmdQuit, QStringList());

		this->localSocket->abort();

		if (tempStatus != ConnStatusDisconnected)
			emit this->connectionLost();
	}
}

void AstCtiTcpClient::resetLastCtiCommand()
{
	delete this->lastCtiCommand;
	this->lastCtiCommand = 0;

	if (!this->commandQueue.isEmpty())
		this->sendCommand(this->commandQueue.dequeue());
}

// Sets the keep-alive interval in miliseconds
void AstCtiTcpClient::setKeepAliveInterval(const int miliseconds)
{
	//Reduce the interval to allow for network latency
	if (miliseconds > 4000)
		this->keepAliveInterval = miliseconds - 2000;
	else
		this->keepAliveInterval = miliseconds / 2;
}

//Prepares data for sending to server, using compression if so indicated
const QByteArray AstCtiTcpClient::convertDataForSending(const QString& data)
{
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	//out.setVersion(QDataStream::Qt_4_5);

	//First two bytes are used for framing
	out << (quint16)0;
	if (this->compressionLevel > 0) {
		//Convert to UTF8 before compressing
		out << qCompress(data.toUtf8(), this->compressionLevel);
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
void AstCtiTcpClient::sendDataToServer(const QString& data)
{
	if (this->debug)
		qDebug() << "Sending to server:   >>" << data.simplified();

	this->localSocket->write(this->convertDataForSending(data));
	this->localSocket->flush();

	//Restart the idle timer
	this->idleTimer->start(this->keepAliveInterval);
}

//Analyzes block of data received from server and sends it for further processing
void AstCtiTcpClient::parseDataReceivedFromServer(const QString& message)
{
	if (message.isEmpty()) {
		if (this->debug)
			qWarning() << "Received empty string from server.";
		return;
	}

	if (message.startsWith('<'))
		//First character of received data is 'a number'<', which means it is an XML object
		emit this->serverMessage(message);
	else
		//Otherwise it is a response from server
		this->processResponse(this->parseResponse(message));
}

//Creates AstCtiResponse object from string
const AstCtiResponse AstCtiTcpClient::parseResponse(const QString& responseString)
{
	if (this->debug)
		qDebug() << "Processing response: <<" << responseString;

	AstCtiResponse response;

	QStringList data = responseString.split(" ");
	response.code = data.at(0) == RSP_OK ? RspOK : RspError;
	// Remove response code
	data.removeFirst();

	if (response.code == RspError) {
		response.errorCode = (AstCtiErrorCodes)data.at(0).toInt();
		// Remove error code
		data.removeFirst();
	} else {
		response.errorCode = ErrNotDefined;
	}

	response.data = data;

	return response;
}

//Analyzes server response and takes appropriate action
//The handshake protocol is followed until authentication is complete or error occurs
void AstCtiTcpClient::processResponse(const AstCtiResponse& response) {
	switch (response.code) {
	case RspOK:
		if (this->lastCtiCommand != 0) {
			switch (this->lastCtiCommand->command) {
			case CmdLogin:
				this->setKeepAliveInterval(response.data.last().toInt());
				this->connectionStatus = ConnStatusLoggedIn;
				// We don't send a notification that log-in was successful because server will send
				// a configuration object, which will tell the application that we are logged in
				break;
			case CmdChangePassword:
				//Update password
				this->password = this->lastCtiCommand->parameters.at(1);
				emit this->serverResponse(ErrNotDefined, CmdChangePassword, QStringList());
				break;
			case CmdKeepAlive:
				this->setKeepAliveInterval(response.data.last().toInt());
				break;
			case CmdOriginate:
				//TODO
				break;
			case CmdConference:
				//TODO
				break;
			case CmdStart:
				//Server acknowledged start request, result will be sent later as an event
				break;
			case CmdPause:
				//Server acknowledged pause request, result will be sent later as an event
				break;
			case CmdQuit:
				//Nothing to do, we shouldn't even receive this
				break;
			default:
				if (this->debug)
					qDebug() << "Received unexpected response from server:"
							 << (response.code == RspOK ? RSP_OK : RSP_ERR)
							 << response.errorCode << response.data.join(" ")
							 << "to command" << this->lastCtiCommand->command;
				break;
			}
			this->resetLastCtiCommand();
		} else {
			if (response.data.length() > 0 && response.data.at(0) == QStringLiteral("WELCOME")) {
				this->compressionLevel = response.data.last().toInt();

				this->sendCommand(CmdLogin, QStringList() << this->userName << this->password
														  << osType << this->macAddress);
			} else {
				if (this->debug)
					qDebug() << "Received unexpected response from server:"
							 << (response.code == RspOK ? RSP_OK : RSP_ERR)
							 << response.errorCode << response.data.join(" ")
							 << "to unknown command";
			}
		}
		break;
	case RspError:
		// We don't retry connection in case of an error
		this->connectionStatus = ConnStatusDisconnected;
		if (this->lastCtiCommand != 0) {
			if (this->debug)
				qWarning() << "Received error response from server:" << response.errorCode
						   << "to command" << this->getCommandName(this->lastCtiCommand->command);

			emit this->serverResponse(response.errorCode, this->lastCtiCommand->command,
									  this->lastCtiCommand->parameters);
			this->resetLastCtiCommand();
		} else {
			if (this->debug)
				qDebug() << "Received unexpected response from server:"
						 << response.code << response.data.join(" ")
						 << "to unknown command";
		}
		break;
	}
}

//Slot which receives socket's connected signal
void AstCtiTcpClient::socketConnected()
{
	//We are connected, so connect timer is no longer necessary
	this->connectTimer->stop();

	//Get MAC Address (necessary for authentication with server)
	this->macAddress = "";
	QHostAddress localAddress = this->localSocket->localAddress();
	if (localAddress == QHostAddress::LocalHost) {
		macAddress = QStringLiteral("00:00:00:00:00:00");
	} else {
		foreach (QNetworkInterface iface, QNetworkInterface::allInterfaces()) {
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
		this->macAddress = QStringLiteral("00:00:00:00:00:00");
		if (this->debug)
			qWarning() << "Unable to determine local MAC address. Using default.";
	}

	this->connectionStatus = ConnStatusLoggingIn;
}

//Slot which receives socket's disconnected signal
void AstCtiTcpClient::socketDisconnected()
{
	//Reset compression level to zero (initial communication with server is always uncompressed)
	this->compressionLevel = 0;

	//Clear command queue
	this->commandQueue.clear();
	//Delete last command if it exists
	this->resetLastCtiCommand();

	if (this->connectionStatus != ConnStatusDisconnected) {
		if (this->debug)
			qDebug() << "Conection to server lost. I will keep trying to reconnect at"
					 << this->connectRetryInterval << "second intervals.";

		//Inform user
		emit this->connectionLost();

		//Retry connection
		this->connectSocket();
	}
}

//Slot which receives socket's stateChanged signal
void AstCtiTcpClient::socketStateChanged(const QAbstractSocket::SocketState socketState)
{
	Q_UNUSED(socketState)
//	if (this->debug) {
//		QString newState;
//		switch(socketState) {
//		case QAbstractSocket::UnconnectedState:
//			newState = QStringLiteral("UnconnectedState");
//			break;
//		case QAbstractSocket::HostLookupState:
//			newState = QStringLiteral("HostLookupState");
//			break;
//		case QAbstractSocket::ConnectingState:
//			newState = QStringLiteral("ConnectingState");
//			break;
//		case QAbstractSocket::ConnectedState:
//			newState = QStringLiteral("ConnectedState");
//			break;
//		case QAbstractSocket::BoundState:
//			newState = QStringLiteral("BoundState");
//			break;
//		case QAbstractSocket::ClosingState:
//			newState = QStringLiteral("ClosingState");
//			break;
//		case QAbstractSocket::ListeningState:
//			newState = QStringLiteral("ListeningState");
//			break;
//		}
//		qDebug() << "Socket state changed: " << newState;
//	}
}

//Slot which receives socket's error signal
void AstCtiTcpClient::socketError(const QAbstractSocket::SocketError error)
{
	if (error == QAbstractSocket::SocketTimeoutError) {
		if (this->connectionStatus == ConnStatusConnecting) {
			//Notify user only about first reconnect attempt
			if (this->reconnectNotify) {
				if (this->debug)
					qDebug() << "Unable to connect to server. I will keep trying to connect at"
							 << this->connectRetryInterval << "second intervals.";

				emit this->connectionLost();

				this->reconnectNotify = false;
			}
		}

		this->connectionStatus = ConnStatusDisconnected;

		this->localSocket->abort();

		//Restart connect timer so it reinitiates connection after a specified interval
		this->connectTimer->start(this->connectRetryInterval);
	} else {
		if (this->debug)
			qDebug() << "Socket Error:" << error << this->localSocket->errorString();
	}
}

//Slot which receives socket's readyRead signal
void AstCtiTcpClient::receiveData()
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
			if (this->compressionLevel > 0) {
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
			this->parseDataReceivedFromServer(receivedString);
		}
	}

	//Restart the idle timer
	this->idleTimer->start(this->keepAliveInterval);
}

//Called when connection has been idle for a specified period
void AstCtiTcpClient::idleTimerElapsed()
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
			this->sendCommand(CmdKeepAlive, QStringList());
	}
}

//Called when given connection timeout or connection retry interval has elapsed
//Appropriate action will be taken depending on connection state
//(see connectSocket() and socketError())
void AstCtiTcpClient::connectTimerElapsed()
{
	if (this->connectionStatus == ConnStatusConnecting)
		//Simulate socket timeout error
		this->socketError(QAbstractSocket::SocketTimeoutError);
	else
		//Retry connection
		this->connectSocket();
}

//Returns a string representation of CTI command
const QString AstCtiTcpClient::getCommandName(const AstCtiCommands command)
{
	//We use a variable to exploit NRVO
	QString commandName;

	switch (command) {
	case CmdLogin:
		commandName = QStringLiteral("LOGIN");
		break;
	case CmdKeepAlive:
		commandName = QStringLiteral("NOOP");
		break;
	case CmdChangePassword:
		commandName = QStringLiteral("CHPW");
		break;
	case CmdOriginate:
		commandName = QStringLiteral("ORIG");
		break;
	case CmdConference:
		commandName = QStringLiteral("CONF");
		break;
	case CmdStart:
		commandName = QStringLiteral("START");
		break;
	case CmdPause:
		commandName = QStringLiteral("PAUSE");
		break;
	case CmdQuit:
		commandName = QStringLiteral("QUIT");
		break;
	default:
		commandName = QStringLiteral("");
		break;
	}

	return commandName;
}
