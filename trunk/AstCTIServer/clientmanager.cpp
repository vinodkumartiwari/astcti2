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


ClientManager::ClientManager(QAstCTIConfiguration *config,
                             int socketDescriptor,
                             QObject *parent) :
                QThread(parent),
                socketDescriptor(socketDescriptor)
{
    // Lets copy our configuration struct
    this->config = config;

    if (config->qDebug) qDebug() << "In ClientManager::ClientManager";


}

void ClientManager::run()
{
    // Our separator
    // We'll parse incoming data each time we receive an <eol>. By This way,
    // when we receive from the network incomplete data, we can append them
    // in receive buffer and parse them when they're completed by a successive
    // <eol> command.
    const QString separator = "<eol>";

    if (config->qDebug) qDebug() << "In ClientManager::run";

    // We need to build the socket to manage client connection
    QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Error in socket:" << tcpSocket.errorString();
        return;
    }

    // Let's grab some informations about remote endpoint
    QHostAddress    remote_addr = tcpSocket.peerAddress();  // Remote ip addr
    quint16         remote_port = tcpSocket.peerPort();     // Remote port

    if (config->qDebug) qDebug() << "Incoming Connection from " << remote_addr.toString() << ":" << remote_port;

    // Let's say Welcome to our client!
    this->sendData(&tcpSocket, QString("100 Welcome to AstCTIServer rel. ").append(ASTCTISRV_RELEASE));

    if (config->qDebug) qDebug() << "Read Timeout is " << config->readTimeout;

    // Read timeout will let us wait for data for some time.
    // When the timeout elapse, the socket will be automatically
    // closed...
    while(tcpSocket.waitForReadyRead(config->readTimeout))
    {
        // Read all data from the network
        QString strdata(tcpSocket.readAll());
        // Append trimmed data to the buffer.
        // We need trimmed data only ;)
        buffer.append(strdata.trimmed());

        // Now check if we've some separator in the buffered data.
        if(buffer.contains(separator))
        {
            QString toSplit         = ""; // here we'll store data to be parsed
            QString remainingBuffer = ""; // here we'll store data that will remain in the buffer
            // If we have text after the last separator, then
            // we need to leave it in the buffer and parse all the rest
            int lastIndexOf = buffer.lastIndexOf(separator);
            if (lastIndexOf < buffer.size()-separator.size())
            {
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
            // Iterate the list
            foreach(itm, list)
            {
                itm = itm.trimmed();
                if (itm.size() > 0)
                {
                    // TODO : command parser
                    if (config->qDebug) qDebug() << "ClientManager::run data received:" << itm;
                    QAstCTICommand  cmd = this->parseCommand(itm);
                    if (cmd.command.compare("QUIT")==0)
                    {
                        tcpSocket.close();
                        return;
                    }

                    if (cmd.command.compare("NOOP")==0)
                    {
                        this->sendData(&tcpSocket, "100 OK");
                        break;
                    }

                    if (cmd.command.compare("USER")==0)
                    {
                        if (cmd.parameters.count() < 1)
                        {
                            this->sendData(&tcpSocket, "101 KO No username given");
                            break;
                        }
                        else
                        {
                            foreach(QString parm, cmd.parameters)
                            {
                                if (config->qDebug) qDebug() << "ClientManager::run params:" << parm;
                            }
                        }
                    }
                }
            }
        }

    }
    // Timeout elapsed: close the socket
    tcpSocket.close();

    if (config->qDebug) qDebug() << "Connection from" << remote_addr.toString() << ":" << remote_port << "closed";
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
void ClientManager::sendData(QTcpSocket *tcpSocket, QString data)
{
    if (config->qDebug) qDebug() << "In ClientManager::sendData(" << data << ")";
    data = data.append("\n");
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << data;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpSocket->write(block);
}
