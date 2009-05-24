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
#include "ctiserverapplication.h"


AMIClient::AMIClient(QAstCTIConfiguration *config, QObject *parent)
        : QThread(parent)

{

    this->config = config;
    this->active_calls  = new QHash<QString, QAstCTICall*>;
    this->commands_stack = new QStack<AsteriskCommand*>;
    this->quit = false;
    this->local_socket = 0;
    this->ami_client_status = AMI_STATUS_NOT_DEFINED;


}

AMIClient::~AMIClient()
{
    if (config->qDebug) qDebug() << "In AMIClient::~AMIClient()";
    delete(this->local_socket);
    delete(this->active_calls);
    delete(this->commands_stack);
}

void AMIClient::start_ami_thread()
{
    this->start();
}



void AMIClient::build_the_socket()
{
    // Builds the socket
    this->local_socket = new QTcpSocket();

    // Initialize how many times we need to restart
    if (this->config->ami_reconnect_retries > 0)
    {
        this->retries = this->config->ami_reconnect_retries;
    }    
}

void AMIClient::run()
{
    // We need to build here the socket to avoid warnings from QObject like
    // "Cannot create children for a parent that is in a different thread.
    if (this->local_socket == 0)
    {
        // Build the socket only once
        build_the_socket();
    }
    forever
    {
        this->local_socket->connectToHost(config->ami_host , config->ami_port);

        if (!this->local_socket->waitForConnected(config->ami_connect_timeout))
        {
            this->retries --;
            if (this->retries == 0)
            {
                qDebug() << "AMI Client will now stop definitively";
                emit ami_client_noretries();
                break;
            }
            this->ami_client_status = AMI_STATUS_NOT_DEFINED;

            qDebug() << "AMI Client will retry reconnect within " << this->config->ami_connect_timeout << " seconds";
            this->sleep(this->config->ami_connect_timeout);

        }
        else
        {
            // Reset retries
            this->retries = config->ami_reconnect_retries;
            forever
            {
                //http://qt.gitorious.org/qt/miniak/blobs/6a994abef70012e2c0aa3d70253ef4b9985b2f20/src/corelib/kernel/qeventdispatcher_win.cpp
                while(this->local_socket->waitForReadyRead(500))
                {

                    QByteArray sockData = local_socket->readAll();
                    QString data_received = QString(sockData);
                    if (data_received.trimmed().length() > 0)
                    {
                        // emit this->receive_data_from_asterisk(data_received);
                        parse_data_received_from_asterisk(data_received);
                    }
                }
                if (this->local_socket->state() != QAbstractSocket::ConnectedState)
                {
                    break;
                }
            }
        }
    }
}

bool AMIClient::is_connected()
{
    return (this->local_socket != 0) & ( this->local_socket->state() == QAbstractSocket::ConnectedState);
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

    // qDebug() << "pdrfm:" << message;
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
    }

}

void AMIClient::execute_actions()
{
    // do a copy of the local buffer and...
    QString local_buffer = this->data_buffer;
    // ... then clean it!
    this->data_buffer.clear();

    // now check our state. The parser can be invoked only
    // after we're authenticated
    switch(this->ami_client_status)
    {
        case AMI_STATUS_LOGGING_IN:
            if (local_buffer.contains("Message: Authentication accepted"))
            {
                qDebug() << "We're authenticated by AMI Server";
                this->ami_client_status = AMI_STATUS_LOGGED_IN;
            }
            else
            {
                this->ami_client_status = AMI_STATUS_NOT_DEFINED;
            }
            break;

        case AMI_STATUS_LOGGED_IN:
            {
                // here we can begin evaluate AMI events
                QStringList events = local_buffer.split("\r\n\r\n");

                // we need to check if the buffer contains more than
                // one event and parse them all in the right order
                QStringList::const_iterator events_iterator;
                for (events_iterator = events.constBegin();
                    events_iterator != events.constEnd();
                    ++events_iterator)
                {
                    QString event_buffer  = (*events_iterator).toLocal8Bit().constData();
                    if (event_buffer.length() > 0)
                    {
                        QHash<QString, QString>* evt = this->hash_from_message(event_buffer);
                        // we should have at least one Event key in the hash!
                        if (evt->contains("Event"))
                        {
                            this->evaluate_event(evt);
                        }
                        else if (evt->contains("Response"))
                        {
                            this->evaluate_response(evt);
                        }
                    }
                }
            }
            break;

        default:
            return;

    }
}


QHash<QString, QString>* AMIClient::hash_from_message(QString data)
{
    QHash<QString, QString>* hash = new QHash<QString, QString>;
    QStringList lines = data.split('\n');
    QStringList::const_iterator lines_iterator;
    for(lines_iterator = lines.constBegin();
        lines_iterator != lines.constEnd();
        ++lines_iterator)
    {
        QString line = (*lines_iterator).toLocal8Bit().constData();
        if (line.contains(':'))
        {
            QStringList key_value = line.split(':');
            if (key_value.length() == 2)
            {
                QString key =key_value.at(0);
                QString value = key_value.at(1);
                hash->insert(key.trimmed() , value.trimmed());
            }
        }
    }
    return hash;
}

void AMIClient::evaluate_event(QHash<QString, QString>* event)
{
    QString event_name = event->value("Event");
    if (config->qDebug) qDebug() << "Received event " << event_name;

    if (event_name == "Shutdown")
    {
        // for the moment we can ignore shutdown event
        // because the socket will be closed from the asterisk
        // server...
        this->active_calls->clear();

        // TODO: Add destionation and call
        emit this->cti_event(AMI_EVENT_SHUTDOWN, 0);
    }
    else if( event_name == "Newchannel" )
    {
        // Build here a new asterisk call object and add it to hashtable
        QAstCTICall* new_call = new QAstCTICall(this->parent());
        QString unique_id = event->value("Uniqueid");
        new_call->set_channel(event->value("Channel"));
        new_call->set_callerid_num(event->value("CallerIDNum"));
        new_call->set_callerid_name(event->value("CallerIDName"));
        new_call->set_state(event->value("ChannelStateDesc"));
        new_call->set_uniqueid(unique_id);

        this->active_calls->insert(unique_id, new_call);

        // TODO: Add destionation and call
        emit this->cti_event(AMI_EVENT_NEW_CHANNEL, new_call);

        // qDebug() << "Newchannel" << unique_id << "in state: " << event->value("ChannelStateDesc");
    }
    else if( event_name == "Hangup" )
    {
        QString unique_id = event->value("Uniqueid");
        if (this->active_calls->contains(unique_id) )
        {
            QAstCTICall* new_call = this->active_calls->operator [](unique_id);
            if (new_call != 0)
            {
                // TODO: Add destionation and call
                emit this->cti_event(AMI_EVENT_HANGUP, new_call);
            }
            this->active_calls->remove(unique_id);
        }
        // Delete here the call from the hashtable
        qDebug() << "Hangup: " << event->value("Uniqueid");
    }
    else if (event_name == "Newexten")
    {
        QString context = event->value("Context");
        QString unique_id = event->value("Uniqueid");
        if (this->active_calls->contains(unique_id) )
        {
            QAstCTICall* new_call = this->active_calls->operator [](unique_id);
            if (new_call != 0)
            {
                new_call->set_context(context);
                qDebug() << "Call context for " << unique_id << "is now" << context;

                // TODO: Add destionation and call
                emit this->cti_event(AMI_EVENT_NEW_EXTEN, new_call);
            }
        }

    }
    else if (event_name == "VarSet")
    {
        QString unique_id = event->value("Uniqueid");
        if (this->active_calls->contains(unique_id) )
        {
            QAstCTICall* new_call = this->active_calls->operator [](unique_id);
            if (new_call != 0)
            {
                // let's retrieve call context
                QString context = new_call->get_context();
                QString variable= event->value("Variable");
                if (context != "")
                {
                    QAstCTIServices* services = CtiServerApplication::instance()->get_services();
                    QAstCTIService* service = services->operator [](context);
                    if (service != 0)
                    {
                        // Check if the variable we're getting exists and is relevant in our configuration
                        if (service->get_variables()->contains( variable ) )
                        {
                            // Add the new variable to the current call
                            new_call->add_variable(variable, event->value("Value"));
                            qDebug() << "Reading variable " << variable << "with value" << event->value("Value");
                            // TODO: Add destionation and call

                            emit this->cti_event(AMI_EVENT_VAR_SET, new_call);
                        }
                        else
                        {
                            qDebug() << "Variable " << variable << "doesn't exists in context" << context;
                        }
                    }
                }
            }
        }
    }
    else if (event_name == "Join")
    {
        qDebug() << "Channel" << event->value("Channel") << "is joining queue" << event->value("Queue");
        // TODO: Add destionation and call
        emit this->cti_event(AMI_EVENT_JOIN,0);
    }
    else if (event_name == "Bridge")
    {
        // we come from a queue
        /*
        Event: Bridge
        Privilege: call,all
        Bridgestate: Link
        Bridgetype: core
        Channel1: SIP/200-0974a250 <- source channel
        Channel2: SIP/201-097525f8 <- dest channel
        Uniqueid1: bruno-1240133625.2 <- source uid
        Uniqueid2: bruno-1240133628.3 <- dest uid
        CallerID1: 200
        CallerID2: 200
        */

        QString unique_id = event->value("Uniqueid1");
        if (this->active_calls->contains(unique_id) )
        {
            QAstCTICall* new_call = this->active_calls->operator [](unique_id);
            if (new_call != 0)
            {

                // let's retrieve call context
                QString context = new_call->get_context();
                QString variable= event->value("Variable");
                if (context != "")
                {
                    QAstCTIServices* services = CtiServerApplication::instance()->get_services();
                    QAstCTIService* service = services->operator [](context);
                    if (service != 0)
                    {
                        // Here we add reference to service applications.
                        // Before the call is passed to the clientmanager
                        // will be selected the right application using client's
                        // operating system.
                        new_call->set_applications(service->get_applications());

                        // Let's say where the call is directed. The destination
                        // will let us know the client to signal.
                        new_call->set_dest_uniqueid(event->value("Uniqueid2"));
                        new_call->set_dest_channel(event->value("Channel2"));

                        emit this->cti_event(AMI_EVENT_BRIDGE,new_call);
                    }
                }
            }
        }
    }
    delete(event);
}

void AMIClient::evaluate_response(QHash<QString, QString>* response)
{
    QString response_str = response->value("Response");
    QString response_msg = response->value("Message");

    if (this->commands_stack->count() > 0)
    {
        AsteriskCommand* cmd = this->commands_stack->pop();
        if (cmd != 0)
        {
            QString channel = cmd->channel;
            QString command_name = cmd->command_name;
            qDebug() << "Evaluated response " << response_str << ". For channel:" << channel << ".Cause:"<< response_msg;
            delete(cmd);
            emit this->cti_response(command_name, response_str, response_msg, channel);

        }
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
    this->local_socket->flush();
}

void AMIClient::send_command_to_asterisk(const QString& command_name, const QString& data, const QString& channel)
{
    AsteriskCommand* cmd = new AsteriskCommand();
    cmd->command_name = command_name;
    cmd->command = data;
    cmd->channel = channel;
    this->commands_stack->push(cmd);
    this->send_data_to_asterisk(data);
}

