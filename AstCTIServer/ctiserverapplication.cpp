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
#include "ctiserverapplication.h"

CtiServerApplication::CtiServerApplication(int &argc, char **argv)
        : QCoreApplication(argc, argv)
{

    is_config_loading = false;
    can_start      = false;

    QArgumentList args(argc, argv);
    config.qDebug = args.getSwitch("--debug") | args.getSwitch("-d");
    QString configFile = args.getSwitchArg("-c", "settings.ini");

    if (config.qDebug) qDebug("Reading Configurations");

    if (!read_settings_file(configFile, &config))
    {
        return;
    }

    // Here we get from our configuration checker object the name
    // of the sqlite database containing the runtime configuration
    this->config_checker = new ConfigurationChecker(config.runtimeConfiguration);
    QFileInfo* lastDbFile = this->config_checker->load_first_configuration();
    if (lastDbFile == 0)
    {
        qCritical() << "Cannot read runtime configuration file";
        return;
    }
    qDebug() << "Reading first runtime configuration from file " << lastDbFile->absoluteFilePath();
    if (!build_sql_database(lastDbFile->absoluteFilePath()))
    {
        return;
    }


    // here we connect an event that will be triggered everytime
    // there's a newer runtime configuration database to read
    connect(this->config_checker, SIGNAL(new_configuration(QFileInfo*)), this, SLOT(reload_sql_database(QFileInfo*)));
    can_start = true;
}

void  CtiServerApplication::reload_sql_database(QFileInfo * databaseFile)
{
    qDebug() << "Configuration changes" << databaseFile->absoluteFilePath();

    build_sql_database(databaseFile->absoluteFilePath());

    /* CODE TESTING START: used for testing purpose only */
    qDebug("CODE TESTING START AT %s:%d",  __FILE__ , __LINE__);
    if (this->services != 0)
    {
        delete(this->services);
    }
    if (this->services == 0)
    {
        this->services = new QAstCTIServices();
    }

    QString sername = "ast-cti-demo";
    QAstCTIService* service = services->operator [](sername);
    if (service != 0)
    {
        qDebug() << "Service alive is " << service->get_service_name();
        qDebug() << "Number of operators on service is " << service->get_operators()->count();
        QAstCTIApplication* app = service->get_applications()->operator []("LIN");
        if (app != 0)
            qDebug() << "Application for lin is " << app->get_application_path();

        app = service->get_applications()->operator []("WIN");
        if (app != 0)
            qDebug() << "Application for win is " << app->get_application_path();


    }

    qDebug("CODE TESTING END AT %s:%d",  __FILE__ , __LINE__);
    /* CODE TESTING END*/

}

CtiServerApplication *CtiServerApplication::instance()
{

    return (static_cast<CtiServerApplication*>(QCoreApplication::instance()));
}

CtiServerApplication::~CtiServerApplication()
{
    if (this->config_checker != 0) delete(config_checker);
    if (config.qDebug) qDebug() << "CtiServerApplication closing";
    this->destroy_sql_database();
    if (config.qDebug) qDebug() << "SQLite database connection closed";
    if (config.qDebug) qDebug() << "Stopped";
}

CoreTcpServer *CtiServerApplication::build_new_core_tcpserver()
{
    if (!can_start) return NULL;

    // Let's build our main window
    this->core_tcp_server = new CoreTcpServer(&this->config);

    // Say we're listening on port..
    if (config.qDebug) qDebug() << "CoreTcpServer listening on port " <<  config.serverPort;

    // TODO : fix to listen also on specific addresses
    this->core_tcp_server->listen(QHostAddress::Any, this->config.serverPort );
    connect(this->core_tcp_server, SIGNAL(destroyed()), this, SLOT(quit()));

    return this->core_tcp_server;
}

bool CtiServerApplication::build_sql_database(const QString & databaseFile)
{
    if (config.db != 0)
    {
        this->destroy_sql_database();
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "sqlitedb");
    db.setDatabaseName(databaseFile);

    if (config.sqlite_user.length() > 0)
        db.setUserName(config.sqlite_user);

    if (config.sqlite_secret.length() > 0)
        db.setPassword(config.sqlite_secret);

    if(!db.open())
    {
        qCritical() << "Unable to open SQLite database" << config.sqlite_file << "\n";
        return false;
    }

    if (config.qDebug) qDebug() << "SQLite database successfully opened" << config.sqlite_file << "\n";

    if (config.qDebug) qDebug() << "SQLite database version " << this->read_database_version() << "\n";

    config.sqlite_file = databaseFile;
    config.db = &db;
    return true;

}

QString CtiServerApplication::read_database_version()
{
    QString result = "";
    QSqlDatabase db = QSqlDatabase::database("sqlitedb");
    QSqlQuery query(db);
    if (query.exec("SELECT VERSION FROM dbversion LIMIT 1"))
    {
        query.first();
        result = query.value(0).toString();
        query.finish();
    }
    query.clear();
    return result;
}

void CtiServerApplication::destroy_sql_database()
{

    if (QSqlDatabase::contains("sqlitedb"))
    {
        QSqlDatabase db = QSqlDatabase::database("sqlitedb");

        if (db.isValid())
        {
            if (db.isOpen()) db.close();
            QSqlDatabase::removeDatabase("sqlitedb");
        }
    }
    config.sqlite_file.clear();
    config.db = 0;
}

bool CtiServerApplication::read_settings_file(const QString configFile, QAstCTIConfiguration* config)
{
    QString logtxt;

    // check that the config file exists
    QFile cfile(configFile);
    if (!cfile.exists())
    {
        qCritical() << "MainServer Settings file " <<  configFile << "doesn't exists";
        return false;
    }
    // check that the config is readable
    if (!cfile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "MainServer Settings file " <<  configFile << "is not readable";
        return false;
    }
    cfile.close();

    // Instantiate a QSettings object for read/write from/to ini file
    QSettings settings(configFile, QSettings::IniFormat );

    // Just for the group named Server
    // write default values if keys are not set
    settings.beginGroup("Server");
    write_settings_file(&settings, "port",                 DEFAULT_SERVER_PORT);
    write_settings_file(&settings, "readTimeout",          DEFAULT_READ_TIMEOUT);
    write_settings_file(&settings, "compressionLevel",     DEFAULT_COMPRESSION_LEVEL);

    settings.endGroup();

    // write default values for Asterisk AMI
    settings.beginGroup("AsteriskAMI");
    write_settings_file(&settings, "host",            DEFAULT_AMI_HOSTIP);
    write_settings_file(&settings, "port",            DEFAULT_AMI_PORT);
    write_settings_file(&settings, "user",            DEFAULT_AMI_USER);
    write_settings_file(&settings, "secret",          DEFAULT_AMI_SECRET);
    write_settings_file(&settings, "connect_timeout", DEFAULT_AMI_CONNECT_TIMEOUT);
    settings.endGroup();

    settings.beginGroup("RuntimeDb");
    write_settings_file(&settings, "runtimeConfiguration", DEFAULT_RTCONFIG);
    write_settings_file(&settings, "user",            "");
    write_settings_file(&settings, "secret",          "");
    settings.endGroup();

    // read values from the keys and store them in a config object
    settings.beginGroup("Server");
    config->readTimeout          = settings.value("readTimeout").toInt();
    config->serverPort           = settings.value("port").toInt();
    config->compressionLevel     = settings.value("compressionLevel").toInt();
    settings.endGroup();

    settings.beginGroup("AsteriskAMI");
    config->ami_host             = settings.value("host").toString();
    config->ami_port             = settings.value("port").toInt();
    config->ami_user             = settings.value("user").toString();
    config->ami_secret           = settings.value("secret").toString();
    config->ami_connect_timeout  = settings.value("connect_timeout").toInt() * 1000;
    settings.endGroup();

    settings.beginGroup("RuntimeDb");
    config->runtimeConfiguration = settings.value("runtimeConfiguration").toString();
    config->sqlite_user          = settings.value("user").toString();
    config->sqlite_secret        = settings.value("secret").toString();
    settings.endGroup();
    return true;
}

void CtiServerApplication::write_settings_file( QSettings *settings, const QString & key, const QVariant & defvalue)
{
    if (!settings->contains(key))
    {
        settings->setValue(key,defvalue);
    }
}
