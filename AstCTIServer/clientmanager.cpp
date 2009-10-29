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

#include "clientmanager.h"
#include "coretcpserver.h"
#include "ctiserverapplication.h"
#include "qastctiseat.h"

/*!
    ClientManager Constructor
*/
ClientManager::ClientManager(QAstCTIConfiguration *config,
                             int socketDescriptor,
                             QObject *parent) :
                             QThread(parent),
                             waitBeforeQuit()




{
    // Initialize private variables
    this->state                 = StateNotLoggedIn;
    this->socketDescriptor      = socketDescriptor;
    this->config                = config;
    this->activeOperator        = 0;
    this->activeSeat            = 0;
    this->clientOperatingSystem = "";
    this->ctiUserName           = "";
    this->blockSize             = 0;
    this->compressionLevel      = -1;
    this->authenticated         = false;
    this->ctiLogoffOnClose      = true;
    this->retries               = 3;
    if (config->qDebug) qDebug() << "In ClientManager::ClientManager";

    // We need to initialize our parser
    this->initParserCommands();

    connect(parent, SIGNAL(sendDataFromServer(QString)), this, SLOT(sendDataToClient(QString)));
    connect(this, SIGNAL(dataReceived(QString)), this, SLOT(parseDataReceived(QString)));
}

ClientManager::~ClientManager()
{
    if (config->qDebug) qDebug() << "ClientManager terminated";    
    exit(0);
}

/*!
    Initialize the commandList QHash
*/
void ClientManager::initParserCommands()
{
    if (config->qDebug) qDebug() << "In ClientManager::initParserCommands()";
    this->commandsList.insert("QUIT",       CmdQuit);
    this->commandsList.insert("CMPR",       CmdCompression);
    this->commandsList.insert("NOOP",       CmdNoOp);
    this->commandsList.insert("USER",       CmdUser);
    this->commandsList.insert("PASS",       CmdPass);
    this->commandsList.insert("CHPW",       CmdChangePassword);
    this->commandsList.insert("KEEP",       CmdKeep);
    this->commandsList.insert("OSTYPE",     CmdOsType);
    this->commandsList.insert("IDEN",       CmdIden);
    this->commandsList.insert("SERVICES",   CmdServices);
    this->commandsList.insert("QUEUES",     CmdQueues);
    this->commandsList.insert("PAUSE",      CmdPause);
    this->commandsList.insert("ORIG",       CmdOrig);
    this->commandsList.insert("STOP",       CmdStop);
    this->commandsList.insert("MAC",        CmdMac);

}

void ClientManager::socketDisconnected() {
    if ( (this->ctiLogoffOnClose) &&
         (this->localIdentifier.startsWith("exten-")) ) {
        // We should do a logoff right now..
        emit this->ctiLogoff(this);

        waitBeforeQuit.acquire();
    }
    CtiServerApplication::instance()->removeUser(ctiUserName);
    // Emit a signal when disconnection is in progress
    emit this->removeClient(this->localIdentifier);
    exit(0);

}

void ClientManager::run()
{
    if (config->qDebug) qDebug() << "In ClientManager::run";

    // We need to build the socket to manage client connection
    QTcpSocket              tcpSocket;
    this->localSocket = &tcpSocket;

    if (!this->localSocket->setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Error in socket:" << this->localSocket->errorString();
        return;
    }

    connect(this->localSocket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(this->localSocket, SIGNAL(readyRead()), this, SLOT(sockedDataReceived()));
    // Let's grab some informations about remote endpoint
    QHostAddress    remoteAddr = tcpSocket.peerAddress();  // Remote ip addr
    quint16         remotePort = tcpSocket.peerPort();     // Remote port

    this->localIdentifier = QString("client-%1-%2").arg(remoteAddr.toString()).arg(remotePort);
    emit this->addClient(this->localIdentifier, this);

    if (config->qDebug) qDebug() << "Incoming Connection from " << remoteAddr.toString() << ":" << remotePort;

    // Let's say Welcome to our client!
    this->sendDataToClient("100 OK Welcome to AstCTIServer");
    // Start the event loop;
    exec();
}

void ClientManager::sockedDataReceived()
{
    QDataStream in(this->localSocket);
    in.setVersion ( QDataStream::Qt_4_5 );
    while ( !in.atEnd() )
    {
        if ( this->localSocket->bytesAvailable() < ( int ) sizeof ( quint16 ) ) {
            return;
        }
        if (this->blockSize == 0) {
            in >> this->blockSize;
        }
        if ( this->localSocket->bytesAvailable() < blockSize ) {
            return;
        }

        QByteArray receivedData;
        QString receivedString;

        if (this->compressionLevel > 0) {
            in >> receivedData;
            QByteArray uncompressed = qUncompress(receivedData);
            if (!uncompressed.isEmpty()) {
                receivedString = QString::fromUtf8(uncompressed);
            }
        } else {
            in >> receivedString;
        }
        this->blockSize = 0;
        emit this->dataReceived(receivedString);
    }
}

void ClientManager::parseDataReceived(const QString &data)
{
    if (config->qDebug) qDebug() << "Received signal dataReceived in parseDataReceived: " << data;

    QString parm;
    QAstCTICommand  cmd = this->parseCommand(data);

    switch(commandsList[cmd.command]) {
    case CmdQuit:
        this->sendDataToClient("900 BYE");
        this->localSocket->close();
        break;
    case CmdNoOp:
        this->sendDataToClient("100 OK");
        break;
    case CmdUser:
        if (cmd.parameters.count() < 1) {
            this->sendDataToClient("101 KO No username given");
            break;
        } else {
            this->ctiUserName = cmd.parameters.at(0);

            // Avoid duplicate logins
            QAstCTIOperator *theOperator = CtiServerApplication::instance()->getOperatorByUsername(this->ctiUserName);
            if (theOperator != 0) {
                if (!CtiServerApplication::instance()->containsUser(this->ctiUserName) ) {
                    this->sendDataToClient("100 OK Send Password Now");
                } else {
                    this->ctiUserName = "";
                    this->sendDataToClient("101 KO User is still logged in");
                }
            } else {
                this->ctiUserName = "";
                this->sendDataToClient("101 KO User doesn't exists");
            }
        }
        break;
    case CmdPass:
        if (this->ctiUserName == "")  {
            this->sendDataToClient("101 KO Send User First");
        } else {
            if (cmd.parameters.count() < 1) {
                this->sendDataToClient("101 KO No password given");
                break;
            }

            {
                QAstCTIOperator *theOperator = CtiServerApplication::instance()->getOperatorByUsername(this->ctiUserName);
                if (theOperator == 0) {
                    this->sendDataToClient("101 KO Operator Not Found");
                    this->ctiUserName = "";
                    this->retries--;
                } else {
                    if (!theOperator->checkPassword(cmd.parameters.at(0))) {
                        this->sendDataToClient("101 KO Wrong password");
                        this->ctiUserName = "";
                        this->retries--;
                    } else {
                        this->state = theOperator->getBeginInPause() ? StatePaused : StateLoggedIn;
                        CtiServerApplication::instance()->addUser(this->ctiUserName);
                        this->activeOperator = theOperator;
                        this->authenticated = true;
                        this->sendDataToClient("102 OK Authentication successful");
                    }
                }
            }
            if (retries == 0) {
                this->localSocket->close();
            }
        }
        break;
    case CmdChangePassword:
        if (!authenticated) {
            this->sendDataToClient("101 KO Not authenticated");
            break;
        } else {
            if (cmd.parameters.count() < 1) {
                this->sendDataToClient("101 KO Specify new password");
                break;
            }

            if (this->activeOperator != 0) {
                QString newPassword = cmd.parameters.at(0);
                if (this->activeOperator->changePassword(newPassword)) {
                    this->sendDataToClient("100 OK Password changed successfully");
                } else {
                    this->sendDataToClient("101 KO Password not changed. Error occurred");
                }
            } else {
                this->sendDataToClient("101 KO Not authenticated");
            }

        }
    case CmdMac:
        if (!authenticated) {
            this->sendDataToClient("101 KO Not authenticated");
            break;
        }

        if (cmd.parameters.count() < 1) {
            this->sendDataToClient("101 KO No mac given");
            break;
        } else {
            QString mac = cmd.parameters.at(0).toLower();
            QAstCTISeat seat(mac, 0);
            if (seat.loadFromMac()) {
                this->activeSeat = &seat;
                if (this->activeOperator != 0) {
                    this->activeOperator->setLastSeatId(seat.getIdSeat());
                    this->activeOperator->save();
                    QString newIdentifier = QString("exten-%1").arg(seat.getSeatExten());
                    emit this->changeClient(this->localIdentifier, newIdentifier);
                    this->localIdentifier = newIdentifier;

                    // We can do a successful cti login only after the seat
                    // is known
                    emit this->ctiLogin(this);
                    this->sendDataToClient("100 OK");
                }
            }
        }
        break;
    case CmdKeep:
        this->sendDataToClient(QString("104 OK %1").arg(config->ctiReadTimeout));
        break;
    case CmdIden:
        if (!authenticated) {
            this->sendDataToClient("101 KO Not authenticated");
            break;
        }
        if (this->activeSeat != 0) {
            this->sendDataToClient(QString("100 OK %1").arg(this->localIdentifier));
        } else {
            this->sendDataToClient("101 KO Seat not yet set");
        }
        break;

    case CmdCompression:
        this->sendDataToClient( QString("103 %1").arg(this->config->ctiSocketCompressionLevel) );
        this->compressionLevel = this->config->ctiSocketCompressionLevel;
        break;
    case CmdOsType:
        if (!authenticated) {
            this->clientOperatingSystem = "";
            this->sendDataToClient("101 KO Not authenticated");
            break;
        }
        if (cmd.parameters.count() < 1) {
            this->clientOperatingSystem = "";
            this->sendDataToClient("101 KO Operating system is not set");
            break;
        } else {
            this->clientOperatingSystem = cmd.parameters.at(0).toUpper();
            this->sendDataToClient("100 OK Operating System Is Set");
        }
        break;

    case CmdOrig:
        if (!authenticated) {
            this->sendDataToClient("101 KO Not authenticated");
            break;
        }
        foreach(parm, cmd.parameters) {
            emit this->notifyServer(parm);
        }
        this->sendDataToClient("100 OK");
        break;
    case CmdServices:
        if (!authenticated) {
            this->sendDataToClient("101 KO Not authenticated");
            break;
        }

        break;
    case CmdPause:
        if (!authenticated) {
            this->sendDataToClient("101 KO Not authenticated");
            break;
        }
        if (cmd.parameters.count() < 1) {
            this->sendDataToClient("101 KO No action requested");
            break;
        } else {
            this->sendDataToClient("301 PAUSE PENDING");
            if (this->state == StatePaused) {
                this->state = StatePauseOutReuqested;
                emit this->ctiPauseOut(this);
            } else {
                this->state = StatePauseInRequested;
                emit this->ctiPauseIn(this);
            }
        }
        break;    
    case CmdNotDefined:
        this->sendDataToClient("101 KO Invalid Command");
        break;
    } // end switch
}

void ClientManager::pauseInResult(const QString &identifier, const bool &result, const QString &reason)
{
    if (identifier == this->localIdentifier) {
        if (result) {
            this->state = StatePaused;
            this->sendDataToClient("300 OK");
        } else {
            this->state = StateLoggedIn;
            this->sendDataToClient(QString("302 KO ").append(reason));
        }
    }
}

void ClientManager::pauseOutResult(const QString &identifier, const bool &result, const QString &reason)
{
    if (identifier == this->localIdentifier) {
        if (result) {
            this->state = StateLoggedIn;
            this->sendDataToClient("300 OK");
        } else {
            this->state = StatePaused;
            this->sendDataToClient(QString("302 KO ").append(reason));
        }
    }
}

void ClientManager::ctiResponse(const QString &identifier, const int actionId,
                                const QString &commandName, const QString &responseString,
                                const QString &responseMessage)
{
    if (identifier == this->localIdentifier) {
        QString strMessage = QString("500 %1,%2,%3,%4")
                                       .arg(actionId)
                                       .arg(commandName)
                                       .arg(responseString)
                                       .arg(responseMessage);
        this->sendDataToClient(strMessage);
    }
}

void ClientManager::disconnectRequest() {
    this->ctiLogoffOnClose = false;
    this->localSocket->close();
}

void ClientManager::unlockAfterSuccessfullLogoff() {
    // this->exitFromSleep = true;
    waitBeforeQuit.release();
}

QAstCTIOperator *ClientManager::getActiveOperator()
{
    return this->activeOperator;
}

QString ClientManager::getClientOperatingSystem()
{
    return this->clientOperatingSystem;
}

QString ClientManager::getLocalIdentifier()
{
    return this->localIdentifier;
}

QAstCTIClientState ClientManager::getState()
{
    return this->state;
}

/*!
    Parse a string command and returns an QAstCTICommand struct with
    command and parameters as QStringList;
*/
QAstCTICommand ClientManager::parseCommand(const QString &command)
{
    QStringList data = command.split(" ");
    QAstCTICommand newCommand;
    newCommand.command = QString(data.at(0)).toUpper();
    data.removeFirst();
    newCommand.parameters = data;
    return newCommand;
}

/*!
    Sends string data over the specified QTcpSocket
*/
void ClientManager::sendDataToClient(const QString &data)
{
    if (this->localSocket == 0) return;
    if (!this->localSocket->state() == QAbstractSocket::ConnectedState ) return;

    if (config->qDebug) qDebug() << "In ClientManager::sendDataToClient(" << data << ")";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out << (quint16)0;
    if (this->compressionLevel > 0) {
        out << qCompress(data.toUtf8(), this->compressionLevel);
    } else {
        out << data;
    }
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    this->localSocket->write(block);

}


