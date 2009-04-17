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

#ifndef AMICLIENT_H
#define AMICLIENT_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include <QStringList>
#include <QDebug>
#include <QSettings>
#include <QHash>


#include "main.h"
#include "qastctiservice.h"
#include "qastctiservices.h"
#include "qastcticall.h"

enum AMIClientStatus {
    AMI_STATUS_NOT_DEFINED,
    AMI_STATUS_LOGGING_IN,
    AMI_STATUS_LOGGED_IN,
    AMI_STATUS_REQEUSTING_EXTENSIONS,
    AMI_STATUS_REQUESTING_SIP,
    AMI_STATUS_REQUESTING_QUEUES,
    AMI_STATUS_ENDLIST
};

enum AMIEvent {
    AMI_EVENT_UNDEFINED,
    AMI_EVENT_SHUTDOWN,
    AMI_EVENT_NEW_CHANNEL,
    AMI_EVENT_HANGUP,
    AMI_EVENT_NEW_EXTEN,
    AMI_EVENT_VAR_SET,
    AMI_EVENT_JOIN
};

class AMIClient : public  QThread
{
    Q_OBJECT

public:
    AMIClient(QAstCTIConfiguration* config, QObject* parent);
    ~AMIClient();
    void run();
public slots:
    void                            send_data_to_asterisk(const QString& data);
    void                            stop_ami_thread();
    void                            stop_ami_thread(bool emit_stopped_signal);

signals:
    void                            error(int socketError, const QString& message);
    void                            receive_data_from_asterisk(const QString& data);
    void                            thread_stopped();
    void                            ami_client_noretries();
    // TODO: complete the signal declaration
    void                            cti_event(const AMIEvent& eventid, QAstCTICall* the_call);

private:
    QAstCTIConfiguration*           config;
    int                             socket_descriptor;
    bool                            quit;
    bool                            emit_stopped_signal_on_quit;
    QString                         data_buffer;
    AMIClientStatus                 ami_client_status;
    int                             retries;
    QHash<QString, QAstCTICall*>*   active_calls;
private slots:
    void                            build_the_socket();
    void                            parse_data_received_from_asterisk(const QString& message);
    void                            restart_ami_thread();
    void                            start_ami_thread();
    void                            perform_login();
    void                            execute_actions();
    void                            log_socket_error(int socketError, const QString& message);
    QHash<QString, QString>*        hash_from_message(QString data);
    void                            evaluate_event(QHash<QString, QString>* event);
protected:
    QTcpSocket*                     local_socket;
};

#endif // AMICLIENT_H
