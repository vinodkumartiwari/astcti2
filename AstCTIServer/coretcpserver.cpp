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


#include "coretcpserver.h"
#include "ctiserverapplication.h"


#include <QtSql>
#include <QVector>

/*!
    Our CoreTcpServer derive from QTcpServer
*/
CoreTcpServer::CoreTcpServer(QAstCTIConfiguration *config, QObject *parent)
    : QTcpServer(parent)
{
    this->clients = new QHash <QString,ClientManager*>;

    this->isClosing = false;
    // Lets set local configuration struct
    this->config = config;
    if (config->qDebug) qDebug() << "In CoreTcpServer:CoreTcpServer()";
    // Basic init here

     /* CODE TESTING START */
    this->ct = new AMIClient(this->config, 0 );
    connect(this->ct, SIGNAL(ami_client_noretries()), this, SLOT(stop_the_server()));
    connect(this->ct, SIGNAL(cti_event(const AMIEvent, QAstCTICall*)), this, SLOT(receive_cti_event(const AMIEvent, QAstCTICall*)));
    this->ct->start();

    //5f4dcc3b5aa765d61d8327deb882cf99
    /* CODE TESTING END*/
}


CoreTcpServer::~CoreTcpServer()
{

    if (this->config->qDebug) qDebug() << "In CoreTcpServer::~CoreTcpServer()";
    if (this->clients != 0) delete(this->clients);
    if (this->ct != 0) delete(this->ct);
}

/*!
    Called by QTcpServer when a new connection is avaiable.
*/
void CoreTcpServer::incomingConnection(int socketDescriptor)
{
    if (config->qDebug) qDebug() << "In CoreTcpServer::incomingConnection(" << socketDescriptor << ")";

    ClientManager* thread = new ClientManager(this->config, socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    // Inform us about client authentications / disconnections
    connect(thread, SIGNAL(add_client(const QString,ClientManager *)) , this, SLOT(add_client(const QString,ClientManager*)));
    connect(thread, SIGNAL(change_client(const QString,const QString)), this, SLOT(change_client(const QString,const QString)));
    connect(thread, SIGNAL(remove_client(const QString)), this, SLOT(remove_client(const QString)));
    connect(thread, SIGNAL(notify_server(const QString)), this, SLOT(notify_client(const QString)));
    connect(thread, SIGNAL(stop_request(QString,ClientManager*)), this, SLOT(stop_the_server()));

    // Connect signals to inform us about pause and login / logout
    connect(thread, SIGNAL(cti_login(ClientManager*)), this, SLOT(cti_client_login(ClientManager*)));
    connect(thread, SIGNAL(cti_logoff(ClientManager*)), this, SLOT(cti_client_logoff(ClientManager*)));
    connect(thread, SIGNAL(cti_pause_in(ClientManager*)) , this, SLOT(cti_client_pause_in(ClientManager*)));
    connect(thread, SIGNAL(cti_pause_out(ClientManager*)) , this, SLOT(cti_client_pause_out(ClientManager*)));
    /*
        Action: QueueAdd    -> cti_login();
        Queue: astcti-queue
        Interface: SIP/201
        Penalty: 1
        Paused: false

        Action: QueueRemove -> cti_logoff();
        Queue: astcti-queue
        Interface: SIP/201


        Action: QueuePause -> cti_pause_on();
        Interface: SIP/201
        Paused: true


        Action: QueuePause -> cti_pause_off();
        Interface: SIP/201
        Paused: false
    */

    thread->start();
}

void CoreTcpServer::add_client(const QString &exten, ClientManager *cl)
{
    mutexClientList.lock();
    if (!clients->contains(exten))
    {
        if (config->qDebug) qDebug() << "Adding exten" << exten;
        clients->insert(exten, cl);
    }
    else
    {
        if (config->qDebug) qDebug() << "Cannot add exten" << exten << "because is already in list";
    }
    if (config->qDebug) qDebug() << "Number of clients:" << clients->count();
    mutexClientList.unlock();
}

void CoreTcpServer::remove_client(const QString &exten)
{
    if (config->qDebug) qDebug() << "Removing exten" << exten;
    mutexClientList.lock();
    if (clients->contains(exten))
    {
        clients->remove(exten);
    }
    else
    {
        if (config->qDebug) qDebug() << "Cannot find client:" << exten;
    }
    int clientCount = clients->count();
    if (config->qDebug) qDebug() << "Number of clients:" << clientCount;
    if (clientCount == 0)
    {
        this->clients->squeeze();
    }

    mutexClientList.unlock();

    if ((this->isClosing) & (clients->count() == 0) )
    {

        if (config->qDebug) qDebug() << "Clients are now 0 and we were asked to close";
        this->close();

    }
}
void CoreTcpServer::change_client(const QString &oldexten,const QString &newexten)
{
    if (config->qDebug) qDebug() << "Changing client exten from" << oldexten << "to" << newexten;
    mutexClientList.lock();
    if (clients->contains(oldexten))
    {
        ClientManager *cl = clients->take(oldexten);
        clients->insert(newexten, cl);
    }

    mutexClientList.unlock();
}

bool CoreTcpServer::contain_client(const QString &exten)
{

    mutexClientList.lock();
    bool retval = clients->contains(exten);
    mutexClientList.unlock();
    return retval;
}

void CoreTcpServer::notify_client(const QString &data)
{
    emit send_data_from_server(data);
}

void CoreTcpServer::stop_the_server()
{
    this->stop_the_server(false);
}

void CoreTcpServer::stop_the_server(bool close_the_socket)
{
    this->isClosing = true;
    qDebug() << "Received STOP signal";
    emit(this->server_is_closing());

    if (close_the_socket)
    {
        this->close();
    }
    
    CtiServerApplication::instance()->exit(0);
}

void CoreTcpServer::receive_cti_event(const AMIEvent& eventid, QAstCTICall* the_call)
{
    QString log_msg("");

    if (the_call != 0)
    {
        QString call_channel = the_call->get_parsed_dest_channel();

        log_msg = "for call" + the_call->get_uniqueid();
        if (call_channel.length() > 0)
        {
            QString identifier = QString("exten-%1").arg(call_channel);

            if (eventid==AMI_EVENT_BRIDGE)
            {
                mutexClientList.lock();
                if (clients->contains(identifier))
                {

                    ClientManager* client = clients->value(identifier);
                    if (client != 0)
                    {
                        // Here we add informatsion about CTI application to start
                        the_call->set_operating_system( client->get_client_operating_system() );

                        client->send_data_to_client(the_call->to_xml());
                    }
                    else
                        qDebug() << ">> receive_cti_event << Client is null";
                }
                else
                {
                    qDebug() << ">> receive_cti_event << Identifier" << identifier << "not found in client list";
                }
                mutexClientList.unlock();
            }
        }
        else
        {
            qDebug() << ">> receive_cti_event << Call Channel is empty";
        }


    }
    qDebug() << "Received CTI Event" << eventid << log_msg;
}

void CoreTcpServer::cti_client_login(ClientManager* cl)
{
    QAstCTIOperator* the_operator = cl->get_active_operator();
    if (the_operator == 0) return;
    QAstCTISeat* the_seat = the_operator->get_seat();
    if (the_seat == 0) return;
    QHash<QString,int>*  services_list = the_operator->get_list_of_services();
    if (services_list == 0) return;

    QHash<QString, int>::const_iterator i = services_list->constBegin();
    while (i != services_list->constEnd()) {
        QString service_name = i.key();

        // Let's give the pointer a restricted range of life
        {
            // Here we get the service object from CtiServerApplication instance
            QAstCTIService* the_service = 0;
            the_service = CtiServerApplication::instance()->get_services()->operator [](service_name);

            // Check if we got an existent service.. we should even!
            if (the_service != 0)
            {
                QString begin_in_pause = (the_operator->get_begin_in_pause() ? "true" : "false");
                // Get the right interface for the operator fromit's seat
                QString interface = the_seat->get_seat_exten();
                int penalty = i.value();
                // We can send a login if AMIClient is connected
                // and the service is a queue where we can login
                if (this->ct->is_connected() & (the_service->get_service_is_queue()) )
                {
                    QString cmd = QString("Action: QueueAdd\r\nQueue: %1\r\nInterface: %2\r\nPenalty: %3\r\nPaused: %4\r\n\r\n")
                                  .arg(the_service->get_service_queue_name() ).arg(interface).arg(penalty).arg(begin_in_pause);

                    this->ct->send_command_to_asterisk(cmd, interface);

                }
            }
        }
        ++i;
    }
 }




void CoreTcpServer::cti_client_logoff(ClientManager* cl)
{
    QAstCTIOperator* the_operator = cl->get_active_operator();
    if (the_operator == 0) return;
    QAstCTISeat* the_seat = the_operator->get_seat();
    if (the_seat == 0) return;
    QHash<QString,int>*  services_list = the_operator->get_list_of_services();
    if (services_list == 0) return;

    QHash<QString, int>::const_iterator i = services_list->constBegin();
    while (i != services_list->constEnd()) {
        QString service_name = i.key();

        // Let's give the pointer a restricted range of life
        {
            // Here we get the service object from CtiServerApplication instance
            QAstCTIService* the_service = 0;
            the_service = CtiServerApplication::instance()->get_services()->operator [](service_name);

            // Check if we got an existent service.. we should even!
            if (the_service != 0)
            {
                QString begin_in_pause = (the_operator->get_begin_in_pause() ? "true" : "false");
                // Get the right interface for the operator fromit's seat
                QString interface = the_seat->get_seat_exten();

                // We can send a login if AMIClient is connected
                // and the service is a queue where we can login
                if (this->ct->is_connected() & (the_service->get_service_is_queue()) )
                {
                    QString cmd = QString("Action: QueueRemove\r\nQueue: %1\r\nInterface: %2\r\n\r\n")
                                  .arg(the_service->get_service_queue_name() ).arg(interface);

                    this->ct->send_command_to_asterisk(cmd, interface);

                }
            }
        }
        ++i;
    }
}

void CoreTcpServer::cti_client_pause_in(ClientManager* cl)
{
    Q_UNUSED(cl);
}

void CoreTcpServer::cti_client_pause_out(ClientManager* cl)
{
    Q_UNUSED(cl);
}
