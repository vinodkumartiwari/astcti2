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

/*!
    ClientManager Constructor
*/
ClientManager::ClientManager(QAstCTIConfiguration* config,
                             int socketDescriptor,
                             QObject* parent) :
                             QThread(parent),
                             socketDescriptor(socketDescriptor)
{
    // Lets copy our configuration struct
    this->config = config;


    connect(parent, SIGNAL(send_data_from_server(QString)), this, SLOT(send_data_to_client(QString)));

    if (config->qDebug) qDebug() << "In ClientManager::ClientManager";

    // We need to initialize our parser
    this->init_parser_commands();


}

ClientManager::~ClientManager()
{
    qDebug() << "ClientManager terminated";
}

/*!
    Initialize the commandList QHash
*/
void ClientManager::init_parser_commands()
{
    if (config->qDebug) qDebug() << "In ClientManager::init_parser_commands()";
    commands_list["QUIT"]=CMD_QUIT;
    commands_list["NOOP"]=CMD_NOOP;
    commands_list["USER"]=CMD_USER;
    commands_list["PASS"]=CMD_PASS;
    commands_list["ORIG"]=CMD_ORIG;
    commands_list["STOP"]=CMD_STOP;

}

void ClientManager::stop()
{
    this->local_socket->disconnectFromHost();

}

void ClientManager::run()
{
    // Our separator
    // We'll parse incoming data each time we receive an \n. By This way,
    // when we receive from the network incomplete data, we can append them
    // in receive buffer and parse them when they're completed by a successive
    // \n command.
    const QString separator = "\n";

    if (config->qDebug) qDebug() << "In ClientManager::run";

    // We need to build the socket to manage client connection
    QTcpSocket              tcpSocket;

    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        qDebug() << "Error in socket:" << tcpSocket.errorString();
        return;
    }
    this->local_socket = &tcpSocket;

    // Let's grab some informations about remote endpoint
    QHostAddress    remote_addr = tcpSocket.peerAddress();  // Remote ip addr
    quint16         remote_port = tcpSocket.peerPort();     // Remote port

    this->local_identifier = QString("client-%1-%2").arg(remote_addr.toString()).arg(remote_port);
    emit this->add_client(this->local_identifier, this);

    if (config->qDebug) qDebug() << "Incoming Connection from " << remote_addr.toString() << ":" << remote_port;

    // Let's say Welcome to our client!

    this->send_data_to_client("100 Welcome to AstCTIServer");

    if (config->qDebug) qDebug() << "Read Timeout is " << config->readTimeout;

    // Read timeout will let us wait for data for some time.
    // When the timeout elapse, the socket will be automatically
    // closed...
    while(tcpSocket.waitForReadyRead(config->readTimeout))
    {
        QString strdata;
        QByteArray sockData = tcpSocket.readAll();

        if (config->compressionLevel > 0)
        {
            strdata = QString(qUncompress(sockData));
        }
        else
        {
            // Read all data from the network
            strdata = QString(sockData);
        }
        // Append data to the buffer.
        buffer.append(strdata);
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
            QString parm;
            // Iterate the list
            foreach(itm, list)
            {
                itm = itm.trimmed();
                if (itm.size() > 0)
                {
                    // TODO : command parser
                    if (config->qDebug) qDebug() << "ClientManager::run data received:" << itm;
                    QAstCTICommand  cmd = this->parse_command(itm);

                    switch(commands_list[cmd.command])
                    {
                        case CMD_QUIT:
                            this->send_data_to_client("900 BYE");
                            tcpSocket.close();
                            break;
                        case CMD_NOOP:
                            this->send_data_to_client("100 OK");
                            break;
                        case CMD_USER:
                            if (cmd.parameters.count() < 1)
                            {
                                this->send_data_to_client("101 KO No username given");
                                break;
                            }
                            else
                            {
                                foreach(parm, cmd.parameters)
                                {
                                    if (config->qDebug) qDebug() << "ClientManager::run params:" << parm;
                                }
                            }
                            break;
                        case CMD_PASS:
                            // TODO : emit only if user done a successfull authentication

                            emit this->change_client(this->local_identifier, QString("exten-SIP/200"));
                            this->local_identifier = QString("exten-SIP/200");
                            this->send_data_to_client("100 OK Authentication successful");
                            break;
                        case CMD_ORIG:
                            foreach(parm, cmd.parameters)
                            {
                                emit this->notify_server(parm);
                            }
                            this->send_data_to_client("100 OK");
                            break;
                        case CMD_STOP:
                            emit this->stop_request(this->local_identifier, this);
                            qDebug() << "Emitted signal STOP";
                            break;
                        case CMD_NOT_DEFINED:
                             this->send_data_to_client("101 KO Invalid Command");
                            break;
                    } // end switch
                } // end if (itm.size())
            } // foreach
        } // end buffer.contains(separator)
    } // end while(tcpSocket.waitForReadyRead)

    // Timeout elapsed: close the socket
    if ( (tcpSocket.state() != QAbstractSocket::UnconnectedState)
        | (tcpSocket.state() != QAbstractSocket::ClosingState) )
        tcpSocket.close();

    if (config->qDebug) qDebug() << "Connection from" << remote_addr.toString() << ":" << remote_port << "closed";

    // Emit a signal when disconnection is in progress
    // TODO : emit only if user done a successfull authentication
    emit this->remove_client(this->local_identifier);

}

/*!
    Parse a string command and returns an QAstCTICommand struct with
    command and parameters as QStringList;
*/
QAstCTICommand ClientManager::parse_command(const QString& command)
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
void ClientManager::send_data_to_client(const QString& data)
{
    if (config->qDebug) qDebug() << "In ClientManager::send_data_to_client(" << data << ")";

    QByteArray block;

    // TODO : We can use a qCompress/qUncompress function to compress data over the network
    QDataStream out(&block, QIODevice::WriteOnly);

    // We output simple ascii strings (no utf8)
    out << data.toAscii() << "\n";
    if (config->compressionLevel > 0)
    {
        QByteArray compressed = qCompress(block, config->compressionLevel);
        this->local_socket->write(compressed);
    }
    else
        this->local_socket->write(block);

}

/*!
    Compare a plain string "password" with a pre-generated md5 QString "checkPassword"
*/
bool ClientManager::check_password(const QString& password, const QString& checkPassword)
{

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(QByteArray(password.toUtf8()));
    QByteArray result = md5.result();

    return (QString(result).compare(checkPassword) == 0);
}
