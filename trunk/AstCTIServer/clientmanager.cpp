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
ClientManager::ClientManager(QAstCTIConfiguration* config,
                             int socketDescriptor,
                             QObject* parent) :
                             QThread(parent),
                             socketDescriptor(socketDescriptor)

{
    // Lets copy our configuration struct
    this->config = config;
    this->active_operator = 0;
    this->client_operating_system = "";
    connect(parent, SIGNAL(send_data_from_server(QString)), this, SLOT(send_data_to_client(QString)));

    if (config->qDebug) qDebug() << "In ClientManager::ClientManager";

    // We need to initialize our parser
    this->init_parser_commands();

    this->in_pause = false;

}

ClientManager::~ClientManager()
{
    if (config->qDebug) qDebug() << "ClientManager terminated";
}

/*!
    Initialize the commandList QHash
*/
void ClientManager::init_parser_commands()
{
    if (config->qDebug) qDebug() << "In ClientManager::init_parser_commands()";
    commands_list["QUIT"]       = CMD_QUIT;
    commands_list["NOOP"]       = CMD_NOOP;
    commands_list["USER"]       = CMD_USER;
    commands_list["PASS"]       = CMD_PASS;
    commands_list["OSTYPE"]     = CMD_OSTYPE;
    commands_list["SERVICES"]   = CMD_SERVICES;
    commands_list["QUEUES"]     = CMD_QUEUES;
    commands_list["PAUSE"]      = CMD_PAUSE;
    commands_list["ORIG"]       = CMD_ORIG;
    commands_list["STOP"]       = CMD_STOP;
    commands_list["MAC"]        = CMD_MAC;

}

void ClientManager::run()
{
    // Our separator
    // We'll parse incoming data each time we receive an \n. By This way,
    // when we receive from the network incomplete data, we can append them
    // in receive buffer and parse them when they're completed by a successive
    // \n command.
    const QString separator = "\n";

    QString cti_username;
    int retries = 3;
    bool authenticated = false;

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
    this->send_data_to_client("Welcome to AstCTIServer");

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
                                cti_username = cmd.parameters.at(0);
                                this->send_data_to_client("100 OK Send Password Now");
                            }
                            break;
                        case CMD_MAC:
                            if (!authenticated)
                            {
                                this->send_data_to_client("101 KO Not authenticated");
                                break;
                            }

                            if (cmd.parameters.count() < 1)
                            {
                                this->send_data_to_client("101 KO No mac given");
                                break;
                            }
                            else
                            {
                                QString mac = cmd.parameters.at(0).toLower();
                                QAstCTISeat seat(mac, 0);
                                if (seat.load_from_mac())
                                {
                                    if (this->active_operator != 0)
                                    {
                                        this->active_operator->set_last_seat(seat.get_id_seat());
                                        this->active_operator->save();
                                        QString new_identifier = QString("exten-%1").arg(seat.get_seat_exten());
                                        emit this->change_client(this->local_identifier, new_identifier);
                                        this->local_identifier = new_identifier;

                                        this->send_data_to_client("100 OK");
                                    }
                                }
                            }
                            break;
                        case CMD_PASS:
                            // TODO : emit only if user done a successfull authentication
                            // this->active_operator

                            if (cti_username == "")
                            {
                                this->send_data_to_client("101 KO Send User First");
                            }
                            else
                            {
                                if (cmd.parameters.count() < 1)
                                {
                                    this->send_data_to_client("101 KO No password given");
                                    break;
                                }

                                {
                                    QAstCTIOperator* the_operator = CtiServerApplication::instance()->get_operator_by_username(cti_username);
                                    if (the_operator == 0)
                                    {
                                        this->send_data_to_client("101 KO Operator Not Found");
                                        cti_username = "";
                                        retries--;
                                    }
                                    else
                                    {

                                        if (!the_operator->check_password(cmd.parameters.at(0)))
                                        {
                                            this->send_data_to_client("101 KO Wrong password");
                                            cti_username = "";
                                            retries--;
                                        }
                                        else
                                        {
                                            this->in_pause = the_operator->get_begin_in_pause();
                                            this->active_operator = the_operator;
                                            authenticated = true;
                                            this->send_data_to_client("102 OK Authentication successful");
                                            emit this->cti_login();
                                        }
                                    }
                                }

                                if (retries == 0)
                                {
                                    tcpSocket.close();
                                }
                            }
                            break;
                        case CMD_OSTYPE:
                            if (!authenticated)
                            {
                                this->client_operating_system = "";
                                this->send_data_to_client("101 KO Not authenticated");
                                break;
                            }
                            if (cmd.parameters.count() < 1)
                            {
                                this->client_operating_system = "";
                                this->send_data_to_client("101 KO Operating system is not set");
                                break;
                            }
                            else
                            {
                                this->client_operating_system = cmd.parameters.at(0).toUpper();
                                this->send_data_to_client("100 OK Operating System Is Set");
                            }
                            break;

                        case CMD_ORIG:
                            if (!authenticated)
                            {
                                this->send_data_to_client("101 KO Not authenticated");
                                break;
                            }
                            foreach(parm, cmd.parameters)
                            {
                                emit this->notify_server(parm);
                            }
                            this->send_data_to_client("100 OK");
                            break;
                        case CMD_SERVICES:
                            if (!authenticated)
                            {
                                this->send_data_to_client("101 KO Not authenticated");
                                break;
                            }

                            break;
                        case CMD_PAUSE:
                            if (!authenticated)
                            {
                                this->send_data_to_client("101 KO Not authenticated");
                                break;
                            }
                            if (cmd.parameters.count() < 1)
                            {
                                this->send_data_to_client("101 KO No mac given");
                                break;
                            }
                            else
                            {
                                // TODO: Add better support to pause event
                                // we need to connect some signals from coretcpserver
                                // to get back a response for a pause request.
                                if (this->in_pause)
                                {
                                    emit this->cti_pause_out();
                                    this->in_pause = false;
                                }
                                else
                                {
                                    emit this->cti_pause_in();
                                    this->in_pause = true;
                                }
                                this->send_data_to_client("100 OK");

                            }
                            break;
                        case CMD_STOP:
                            if (!authenticated)
                            {
                                this->send_data_to_client("101 KO Not authenticated");
                                break;
                            }
                            emit this->stop_request(this->local_identifier, this);
                            tcpSocket.close();
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

    // We should do a logoff right now..
    emit this->cti_logoff();

    // Emit a signal when disconnection is in progress
    // TODO : emit only if user done a successfull authentication
    emit this->remove_client(this->local_identifier);

}

QAstCTIOperator* ClientManager::get_active_operator()
{
    return this->active_operator;
}

QString ClientManager::get_client_operating_system()
{
    return this->client_operating_system;
}

bool ClientManager::is_in_pause()
{
    return this->in_pause;
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

    // Write the internal buffer to the socket without blocking
    // We need QAbstractSocket to start sending buffered data
    // immediately
    this->local_socket->flush();
}


