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
    this->services = 0;
    this->ctiOperators = 0;
    this->configChecker = 0;
    isConfigLoading = false;
    canStart = false;

    QArgumentList args(argc, argv);
    config.qDebug = args.getSwitch("--debug") | args.getSwitch("-d");
    QString configFile = args.getSwitchArg("-c", "settings.ini");

    if (config.qDebug) qDebug("Reading Configurations");

    if (!readSettingsFile(configFile, &config)) {
        return;
    }

    // ConfigurationChecker will check if database configuration
    // will change
    this->configChecker = new ConfigurationChecker(this);

    qDebug() << "Reading first runtime configuration from database";
    if (!buildSqlDatabase()) {
        return;
    }

    // This will build all services list
    this->services = new QAstCTIServices(this);
    this->ctiOperators = new QAstCTIOperators(this);


    // here we connect an event that will be triggered everytime
    // there's a newer runtime configuration database to read
    connect(this->configChecker, SIGNAL(newConfiguration()), this, SLOT(reloadSqlDatabase()));

    this->configChecker->startConfigurationCheckerThread();

    canStart = true;

}

CtiServerApplication::~CtiServerApplication()
{
    if (config.qDebug) qDebug() << "In CtiServerApplication::~CtiServerApplication()";
    if (this->configChecker != 0) delete(this->configChecker);
    if (this->coreTcpServer != 0) delete(this->coreTcpServer);
    if (config.qDebug) qDebug() << "CtiServerApplication closing";
    this->destroySqlDatabase();
    if (config.qDebug) qDebug() << "MySQL database connection closed";
    if (config.qDebug) qDebug() << "Stopped";

    // Remove msg handler to avoid access to this QApplication
    qInstallMsgHandler(0);
}

void  CtiServerApplication::reloadSqlDatabase()
{
    qDebug() << "Configuration has changed on " << this->configChecker->getLastModified();

    buildSqlDatabase();

    /* CODE TESTING START: used for testing purpose only */
    qDebug("CODE TESTING START AT %s:%d",  __FILE__ , __LINE__);

    if (this->services != 0) {
        delete(this->services);
        this->services = 0;
    }
    if (this->services == 0) {
        this->services = new QAstCTIServices(this);
    }

    if (this->ctiOperators != 0) {
        delete(this->ctiOperators);
        this->ctiOperators= 0;
    }

    if (this->ctiOperators == 0) {
        this->ctiOperators = new QAstCTIOperators(this);
    }

    QString sername = "ast-cti-demo";
    QAstCTIService* service = services->operator [](sername);
    if (service != 0) {
        qDebug() << "Service alive is " << service->getServiceName();
        qDebug() << "Number of operators on service is " << service->getOperators()->count();
        QAstCTIApplication* app = service->getApplications()->operator []("LIN");
        if (app != 0)
            qDebug() << "Application for lin is " << app->getApplicationPath();

        app = service->getApplications()->operator []("WIN");
        if (app != 0)
            qDebug() << "Application for win is " << app->getApplicationPath();
    }

    qDebug("CODE TESTING END AT %s:%d",  __FILE__ , __LINE__);
    /* CODE TESTING END*/

}

CtiServerApplication *CtiServerApplication::instance()
{
    return (static_cast<CtiServerApplication*>(QCoreApplication::instance()));
}

QAstCTIServices *CtiServerApplication::getServices()
{
    return this->services;
}

CoreTcpServer *CtiServerApplication::buildNewCoreTcpServer()
{
    if (!canStart) return 0;

    // Let's build our main window
    this->coreTcpServer = new CoreTcpServer(&this->config);

    // Say we're listening on port..
    if (config.qDebug) qDebug() << "CoreTcpServer listening on port " <<  config.serverPort;

    // TODO : fix to listen also on specific addresses
    this->coreTcpServer->listen(QHostAddress::Any, this->config.serverPort );
    connect(this->coreTcpServer, SIGNAL(destroyed()), this, SLOT(quit()));

    return this->coreTcpServer;
}

bool CtiServerApplication::buildSqlDatabase()
{
    QSqlDatabase db  = QSqlDatabase::database("mysqldb");
    if (db.isValid()) {
        this->destroySqlDatabase();
    }

    db = QSqlDatabase::addDatabase("QMYSQL", "mysqldb");
    db.setHostName(config.sqlHost);
    db.setPort(config.sqlPort);
    db.setUserName(config.sqlUserName);
    db.setPassword(config.sqlPassWord);
    db.setDatabaseName(config.sqlDatabase);


    if(!db.open()) {
        qCritical() << "Unable to open MySQL database: " << db.lastError() << "\n";

        return false;
    }

    if (config.qDebug) qDebug() << "MySQL database successfully opened\n";

    if (config.qDebug) qDebug() << "MySQL database version " << this->readDatabaseVersion() << "\n";

    return true;

}

QString CtiServerApplication::readDatabaseVersion()
{
    QString result = "";
    QSqlDatabase db = QSqlDatabase::database("mysqldb");
    if (!db.isOpen()) {
        db.open();
    }
    QSqlQuery query(db);
    if (query.exec("SELECT VERSION FROM dbversion LIMIT 1")) {
        query.first();
        result = query.value(0).toString();
        query.finish();
    }
    query.clear();
    return result;
}

void CtiServerApplication::destroySqlDatabase()
{

    if (QSqlDatabase::contains("mysqldb")) {
        QSqlDatabase db = QSqlDatabase::database("mysqldb");

        if (db.isValid()) {
            if (db.isOpen()) db.close();
            QSqlDatabase::removeDatabase("mysqldb");
        }
    }
    // config.db = 0;
}

bool CtiServerApplication::readSettingsFile(const QString configFile, QAstCTIConfiguration *config)
{
    QString logtxt;

    // check that the config file exists
    QFile cfile(configFile);
    if (!cfile.exists()) {
        qCritical() << "MainServer Settings file " <<  configFile << "doesn't exists";
        return false;
    }
    // check that the config is readable
    if (!cfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "MainServer Settings file " <<  configFile << "is not readable";
        return false;
    }
    cfile.close();

    // Instantiate a QSettings object for read/write from/to ini file
    QSettings settings(configFile, QSettings::IniFormat );

    // Just for the group named Server
    // write default values if keys are not set
    settings.beginGroup("Server");
    writeSettingsFile(&settings, "port",                 DefaultServerPort);
    writeSettingsFile(&settings, "readTimeout",          DefaultReadTimeout);
    writeSettingsFile(&settings, "compressionLevel",     DefaultCompressionLevel);

    settings.endGroup();

    // write default values for Asterisk AMI
    settings.beginGroup("AsteriskAMI");
    writeSettingsFile(&settings, "host",            DefaultAmiHost);
    writeSettingsFile(&settings, "port",            DefaultAmiPort);
    writeSettingsFile(&settings, "user",            DefaultAmiUser);
    writeSettingsFile(&settings, "secret",          DefaultAmiSecret);
    writeSettingsFile(&settings, "connect_timeout", DefaultAmiConnectTimeout);
    writeSettingsFile(&settings, "connect_retries", DefaultAmiConnectRetries);

    settings.endGroup();

    settings.beginGroup("RuntimeDb");
    writeSettingsFile(&settings, "host",            DefaultSqlHost);
    writeSettingsFile(&settings, "user",            DefaultSqlUser);
    writeSettingsFile(&settings, "password",        DefaultSqlPassWord);
    writeSettingsFile(&settings, "port",            DefaultSqlPort);
    writeSettingsFile(&settings, "database",        DefaultSqlDatabase);

    settings.endGroup();

    // read values from the keys and store them in a config object
    settings.beginGroup("Server");
    config->readTimeout          = settings.value("readTimeout").toInt();
    config->serverPort           = settings.value("port").toInt();
    config->compressionLevel     = settings.value("compressionLevel").toInt();
    settings.endGroup();

    settings.beginGroup("AsteriskAMI");
    config->amiHost                 = settings.value("host").toString();
    config->amiPort                 = settings.value("port").toInt();
    config->amiUser                 = settings.value("user").toString();
    config->amiSecret               = settings.value("secret").toString();
    config->amiConnectTimeout       = settings.value("connect_timeout").toInt();
    config->amiReconnectRetries     = settings.value("connect_retries").toInt();
    settings.endGroup();

    settings.beginGroup("RuntimeDb");
    config->sqlHost             = settings.value("host").toString();
    config->sqlUserName         = settings.value("user").toString();
    config->sqlPassWord         = settings.value("password").toString();
    config->sqlPort             = settings.value("port").toInt();
    config->sqlDatabase         = settings.value("database").toString();


    settings.endGroup();
    return true;
}

void CtiServerApplication::writeSettingsFile( QSettings *settings, const QString &key, const QVariant &defValue)
{
    if (!settings->contains(key)) {
        settings->setValue(key,defValue);
    }
}

QAstCTIOperator *CtiServerApplication::getOperatorByUsername(const QString& username)
{
    if (this->ctiOperators != 0) {
        QAstCTIOperator *theOperator = this->ctiOperators->operator [](username);
        return theOperator;
    }
    return 0;
}
