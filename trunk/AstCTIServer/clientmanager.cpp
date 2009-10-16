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
                             socketDescriptor(socketDescriptor),
                             compressionLevel(-1),
                             waitBeforeQuit()

{
    // Lets copy our configuration struct
    this->config = config;
    this->activeOperator = 0;
    this->clientOperatingSystem = "";
    connect(parent, SIGNAL(sendDataFromServer(QString)), this, SLOT(sendDataToClient(QString)));

    if (config->qDebug) qDebug() << "In ClientManager::ClientManager";

    // We need to initialize our parser
    this->initParserCommands();

    this->inPause = false;

}

ClientManager::~ClientManager()
{
    if (config->qDebug) qDebug() << "ClientManager terminated";
}

/*!
    Initialize the commandList QHash
*/
void ClientManager::initParserCommands()
{
    if (config->qDebug) qDebug() << "In ClientManager::initParserCommands()";
    commandsList["QUIT"]       = CmdQuit;
    commandsList["CMPR"]       = CmdCompression;
    commandsList["NOOP"]       = CmdNoOp;
    commandsList["USER"]       = CmdUser;
    commandsList["PASS"]       = CmdPass;
    commandsList["OSTYPE"]     = CmdOsType;
    commandsList["SERVICES"]   = CmdServices;
    commandsList["QUEUES"]     = CmdQueues;
    commandsList["PAUSE"]      = CmdPause;
    commandsList["ORIG"]       = CmdOrig;
    commandsList["STOP"]       = CmdStop;
    commandsList["MAC"]        = CmdMac;

}

void ClientManager::run()
{
    // Our separator
    // We'll parse incoming data each time we receive an \n. By This way,
    // when we receive from the network incomplete data, we can append them
    // in receive buffer and parse them when they're completed by a successive
    // \n command.
    const QString separator = "\n";

    QString ctiUserName;
    int retries = 3;
    bool authenticated = false;

    if (config->qDebug) qDebug() << "In ClientManager::run";

    // We need to build the socket to manage client connection
    QTcpSocket              tcpSocket;

    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Error in socket:" << tcpSocket.errorString();
        return;
    }
    this->localSocket = &tcpSocket;

    // Let's grab some informations about remote endpoint
    QHostAddress    remoteAddr = tcpSocket.peerAddress();  // Remote ip addr
    quint16         remotePort = tcpSocket.peerPort();     // Remote port

    this->localIdentifier = QString("client-%1-%2").arg(remoteAddr.toString()).arg(remotePort);
    emit this->addClient(this->localIdentifier, this);

    if (config->qDebug) qDebug() << "Incoming Connection from " << remoteAddr.toString() << ":" << remotePort;

    // Let's say Welcome to our client!
    this->sendDataToClient("Welcome to AstCTIServer");

    if (config->qDebug) qDebug() << "Read Timeout is " << config->readTimeout;

    // Read timeout will let us wait for data for some time.
    // When the timeout elapse, the socket will be automatically
    // closed...
    while(tcpSocket.waitForReadyRead(config->readTimeout)) {
        QString strdata;
        QByteArray sockData = tcpSocket.readAll();

        if (this->compressionLevel > 0) {
            QByteArray toUncompress = qUncompress(sockData);
            if (toUncompress.isEmpty()) {
                qCritical() << "Unable to uncompress data in ClientManager::run() for client " << this->localIdentifier << "Try to use uncompressed data.";
                strdata = QString(sockData);
            } else {
                strdata = QString(toUncompress);
            }
        }
        else {
            // Read all data from the network
            strdata = QString(sockData);
        }
        // Append data to the buffer.
        buffer.append(strdata);
        // Now check if we've some separator in the buffered data.
        if(buffer.contains(separator)) {
            QString toSplit         = ""; // here we'll store data to be parsed
            QString remainingBuffer = ""; // here we'll store data that will remain in the buffer
            // If we have text after the last separator, then
            // we need to leave it in the buffer and parse all the rest
            int lastIndexOf = buffer.lastIndexOf(separator);
            if (lastIndexOf < buffer.size()-separator.size()) {
                toSplit         = buffer.left(lastIndexOf);
                remainingBuffer = buffer.mid(lastIndexOf+ separator.size());

            }
            // otherwise we parse the full buffer
            else {
                toSplit         = buffer;
                remainingBuffer = "";
            }
            buffer = remainingBuffer;

            // Let's split our string by separator
            QStringList list = toSplit.split(separator);
            QString itm;
            QString parm;
            // Iterate the list
            foreach(itm, list) {
                itm = itm.trimmed();
                if (itm.size() > 0) {
                    // TODO : command parser
                    if (config->qDebug) qDebug() << "ClientManager::run data received:" << itm;
                    QAstCTICommand  cmd = this->parseCommand(itm);

                    switch(commandsList[cmd.command]) {
                    case CmdQuit:
                        this->sendDataToClient("900 BYE");
                        tcpSocket.close();
                        break;
                    case CmdNoOp:
                        this->sendDataToClient("100 OK");
                        break;
                    case CmdUser:
                        if (cmd.parameters.count() < 1) {
                            this->sendDataToClient("101 KO No username given");
                            break;
                        } else {
                            ctiUserName = cmd.parameters.at(0);

                            // Avoid duplicate logins
                            QAstCTIOperator *theOperator = CtiServerApplication::instance()->getOperatorByUsername(ctiUserName);
                            if (theOperator != 0) {
                                if (!theOperator->getIsLoggedIn()) {
                                    this->sendDataToClient("100 OK Send Password Now");
                                } else {
                                    ctiUserName = "";
                                    this->sendDataToClient("101 KO User is still logged in");
                                }
                            } else {
                                ctiUserName = "";
                                this->sendDataToClient("101 KO User doesn't exists");
                            }
                        }
                        break;
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
                    case CmdPass:
                        // TODO : emit only if user done a successfull authentication
                        // this->activeOperator

                        if (ctiUserName == "")  {
                            this->sendDataToClient("101 KO Send User First");
                        } else {
                            if (cmd.parameters.count() < 1) {
                                this->sendDataToClient("101 KO No password given");
                                break;
                            }

                            {
                                QAstCTIOperator *theOperator = CtiServerApplication::instance()->getOperatorByUsername(ctiUserName);
                                if (theOperator == 0) {
                                    this->sendDataToClient("101 KO Operator Not Found");
                                    ctiUserName = "";
                                    retries--;
                                } else {
                                    if (!theOperator->checkPassword(cmd.parameters.at(0))) {
                                        this->sendDataToClient("101 KO Wrong password");
                                        ctiUserName = "";
                                        retries--;
                                    } else {
                                        this->inPause = theOperator->getBeginInPause();
                                        theOperator->setLoggedIn(true);
                                        this->activeOperator = theOperator;
                                        authenticated = true;
                                        this->sendDataToClient("102 OK Authentication successful");
                                    }
                                }
                            }

                            if (retries == 0) {
                                tcpSocket.close();
                            }
                        }
                        break;
                    case CmdCompression:
                        this->sendDataToClient( QString("103 %1").arg(this->config->compressionLevel) );
                        this->compressionLevel = this->config->compressionLevel;
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
                            this->sendDataToClient("101 KO No mac given");
                            break;
                        } else {
                            // TODO: Add better support to pause event
                            // we need to connect some signals from coretcpserver
                            // to get back a response for a pause request.
                            if (this->inPause) {
                                emit this->ctiPauseOut(this);
                                this->inPause = false;
                            } else {
                                emit this->ctiPauseIn(this);
                                this->inPause = true;
                            }
                            this->sendDataToClient("100 OK");
                        }
                        break;
                    case CmdStop:
                        if (!authenticated) {
                            this->sendDataToClient("101 KO Not authenticated");
                            break;
                        }
                        emit this->stopRequest(this->localIdentifier, this);
                        tcpSocket.close();
                        qDebug() << "Emitted signal STOP";
                        break;
                    case CmdNotDefined:
                        this->sendDataToClient("101 KO Invalid Command");
                        break;
                    } // end switch
                } // end if (itm.size())
            } // foreach
        } // end buffer.contains(separator)
    } // end while(tcpSocket.waitForReadyRead)

    // Timeout elapsed: close the socket
    if ( (tcpSocket.state() != QAbstractSocket::UnconnectedState)
        | (tcpSocket.state() != QAbstractSocket::ClosingState) ) {
        tcpSocket.close();
    }

    if (config->qDebug) qDebug() << "Connection from" << remoteAddr.toString() << ":" << remotePort << "closed";

    if (this->localIdentifier.startsWith("exten-")) {
        // We should do a logoff right now..
        emit this->ctiLogoff(this);

        waitBeforeQuit.acquire();
       /* while(!exitFromSleep) {
            // We'll wait for a successfull logout before quit
            msleep(500);
        }*/
    }


    if (this->activeOperator != 0) {
        this->activeOperator->setLoggedIn(false);
    }
    // Emit a signal when disconnection is in progress
    emit this->removeClient(this->localIdentifier);

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

bool ClientManager::isInPause()
{
    return this->inPause;
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
    if (!this->localSocket->state() == QAbstractSocket::ConnectedState ) return;

    if (config->qDebug) qDebug() << "In ClientManager::sendDataToClient(" << data << ")";

    QByteArray block;

    // TODO : We can use a qCompress/qUncompress function to compress data over the network
    QDataStream out(&block, QIODevice::WriteOnly);

    // We output simple ascii strings (no utf8)
    out << data.toAscii() << "\n";
    if (this->compressionLevel > 0) {
        QByteArray compressed = qCompress(block, this->compressionLevel);
        this->localSocket->write(compressed);
    } else {
        this->localSocket->write(block);
    }

    // Write the internal buffer to the socket without blocking
    // We need QAbstractSocket to start sending buffered data
    // immediately
    this->localSocket->flush();
}


