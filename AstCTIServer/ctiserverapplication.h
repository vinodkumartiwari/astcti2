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

#ifndef CTISERVERAPPLICATION_H
#define CTISERVERAPPLICATION_H

#include <QCoreApplication>
#include <QFile>
#include <QtDebug>
#include <QSettings>
#include <QtCore>
#include <QtCore/QCoreApplication>
#include <QFileInfo>
#include <QtSql>

#include "coretcpserver.h"
#include "cticonfig.h"
#include "logger.h"
#include "argumentlist.h"
#include "configurationchecker.h"
#include "qastctiapplication.h"
#include "qastctiservicesoperators.h"
#include "qastctiservice.h"
#include "qastctiservices.h"

#define DEFAULT_SERVER_PORT         5000
#define DEFAULT_READ_TIMEOUT        15000
#define DEFAULT_COMPRESSION_LEVEL   0
#define DEFAULT_RTCONFIG            "rtconfig/"

#define DEFAULT_AMI_HOSTIP          "localhost"
#define DEFAULT_AMI_PORT            5038
#define DEFAULT_AMI_USER            "manager"
#define DEFAULT_AMI_SECRET          "password"
#define DEFAULT_AMI_CONNECT_TIMEOUT 5

#define EXIT_CODE_SUCCESS           0
#define EXIT_CODE_NO_INI_CONFIG     1
#define EXIT_CODE_NO_SQLITE_CONFIG  2

class CtiServerApplication : public QCoreApplication
{
    Q_OBJECT

public:
    CtiServerApplication(int &argc, char **argv);
    ~CtiServerApplication();
    static CtiServerApplication* instance();

    QAstCTIConfiguration    config; // Main configuration struct

public slots:
    CoreTcpServer*          build_new_core_tcpserver();
    void                    reload_sql_database(QFileInfo * databaseFile);

private:
    bool                    can_start;
    CoreTcpServer*          core_tcp_server;
    ConfigurationChecker*   config_checker;
    bool                    is_config_loading;
    QAstCTIServices*        services;

    bool                    build_sql_database(const QString & databaseFile);
    void                    destroy_sql_database();
    QString                 read_database_version();

    bool                    read_settings_file(const QString configFile, QAstCTIConfiguration* config);
    void                    write_settings_file(QSettings* settings, const QString & key, const  QVariant & defvalue);


};

#endif // CTISERVERAPPLICATION_H
