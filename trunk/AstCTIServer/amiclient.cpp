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

#include "amiclient.h"

AMIClient::AMIClient(QAstCTIConfiguration *config, QObject *parent)
        : QThread(parent)
{
    this->config = config;
    this->quit = false;
    this->ami_client_status = AMI_STATUS_NOT_DEFINED;
    this->local_socket = new QTcpSocket();

    if (this->config->ami_reconnect_retries > 0)
    {
        this->retries = this->config->ami_reconnect_retries;
    }
    connect(this, SIGNAL(thread_stopped()) , this, SLOT(restart_ami_thread()));

    connect(this, SIGNAL(error(int,QString)), this, SLOT(log_socket_error(int,QString)));
    connect(this, SIGNAL(receive_data_from_asterisk(QString)), this, SLOT(parse_data_received_from_asterisk(QString)));
    connect(this->local_socket, SIGNAL(disconnected()), this, SLOT(stop_ami_thread()) );

}

AMIClient::~AMIClient()
{
    if (config->qDebug) qDebug() << "In AMIClient::~AMIClient()";
    delete(this->local_socket);
}

void AMIClient::start_ami_thread()
{
    this->run();
}

void AMIClient::restart_ami_thread()
{
    if (this->config->ami_reconnect_retries == 0)
    {

        qDebug() << "AMI Client will retry reconnect within " << this->config->ami_connect_timeout << " seconds";
        QTimer::singleShot(this->config->ami_connect_timeout*1000 , this, SLOT(start_ami_thread()));

    }
    else
    {
        this->retries --;
        if (this->retries > 0)
        {
            qDebug() << "AMI Client will retry reconnect within " << this->config->ami_connect_timeout << " seconds";
            QTimer::singleShot(this->config->ami_connect_timeout*1000, this, SLOT(start_ami_thread()));
        }
        else
        {
            qDebug() << "AMI Client will now stop definitively";
            emit ami_client_noretries();
        }
    }
}

void AMIClient::log_socket_error(int socketError, const QString& message)
{
    qDebug() << "Received error" << socketError << "from socket:" << message;
    restart_ami_thread();

}

void AMIClient::run()
{
    this->quit = false;
    this->emit_stopped_signal_on_quit = true;
    this->local_socket->connectToHost(config->ami_host , config->ami_port);

    if (!this->local_socket->waitForConnected(config->ami_connect_timeout))
    {
        emit error(this->local_socket->error(), this->local_socket->errorString());
        return;
    }
    while(!quit)
    {
        // TODO: main loop for receinving data..
        while(this->local_socket->waitForReadyRead(500))
        {

            QByteArray sockData = local_socket->readAll();
            QString data_received = QString(sockData);
            emit this->receive_data_from_asterisk(data_received);
        }
    }
    this->local_socket->close();
    this->ami_client_status = AMI_STATUS_NOT_DEFINED;
    if (this->emit_stopped_signal_on_quit)
    {
        emit this->thread_stopped();
    }
}

void AMIClient::stop_ami_thread()
{
    this->stop_ami_thread(true);
}
void AMIClient::stop_ami_thread(bool emit_stopped_signal)
{
    this->emit_stopped_signal_on_quit = emit_stopped_signal;
    this->quit = true;
}



void AMIClient::perform_login()
{
    this->ami_client_status = AMI_STATUS_LOGGING_IN;
    QString login_data = QString("Action: Login\r\nUsername: %1\r\nSecret: %2\r\n\r\n")
                            .arg(this->config->ami_user)
                            .arg(this->config->ami_secret);
    if (config->qDebug) qDebug() << login_data;
    this->send_data_to_asterisk(login_data);

}

void AMIClient::parse_data_received_from_asterisk(const QString& message)
{

    qDebug() << "pdrfm:" << message;

    if (this->ami_client_status != AMI_STATUS_NOT_DEFINED)
    {
        this->data_buffer+=message;
        if (this->data_buffer.contains("\r\n\r\n"))
        {
            // here we can execute actions on buffer
            this->execute_actions();
        }
    }
    else
    {
        if (message.contains( "Asterisk Call Manager", Qt::CaseInsensitive  ))
        {
            this->perform_login();
        }
        if (this->ami_client_status == AMI_STATUS_LOGGING_IN)
        {
            if (message.contains("Message: Authentication accepted"))
            {
                qDebug() << "We're authenticated by AMI Server";
                this->ami_client_status = AMI_STATUS_LOGGED_IN;
            }
        }
    }

}

void AMIClient::execute_actions()
{
    // do a copy of the local buffer and...
    QString local_buffer = this->data_buffer;
    // ... then clean it!
    this->data_buffer.clear();

    switch(this->ami_client_status)
    {
        case AMI_STATUS_LOGGING_IN:
            if (local_buffer.contains("Message: Authentication accepted"))
            {
                qDebug() << "We're authenticated by AMI Server";
                this->ami_client_status = AMI_STATUS_LOGGED_IN;
            }
            break;
        default:
            return;

    }

}

void AMIClient::send_data_to_asterisk(const QString& data)
{
    if (config->qDebug) qDebug() << "In AMIClient::send_data_to_asterisk(" << data << ")";

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // We output simple ascii strings (no utf8)    
    out.writeRawData(data.toAscii(), data.length());

    qDebug() << block;

    this->local_socket->write(block);
}


