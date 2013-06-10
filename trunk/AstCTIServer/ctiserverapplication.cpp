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
#include "db.h"

CtiServerApplication::CtiServerApplication(const QString &appId, int &argc, char **argv)
        : QtSingleCoreApplication(appId, argc, argv), coreTcpServer(0)
{
    this->services = 0;
    this->ctiOperators = 0;
    this->configChecker = 0;
    isConfigLoading = false;
    canStart = false;
    if (!this->isRunning()) {
		QArgumentList args(argc, argv);
        config.debug = args.getSwitch("--debug") | args.getSwitch("-d");
        QString configFile = args.getSwitchArg("-c", "settings.ini");

		if (config.debug)
			qDebug("Reading Configuration");

        // Read default settings for database connection from file
		readSettingsFromFile(configFile);
		if (config.debug)
			qDebug() << "Reading first runtime configuration from database";
		if (!DB::buildConnection(config.sqlHost, config.sqlPort, config.sqlUserName,
								 config.sqlPassWord, config.sqlDatabase)) {
            return;
        }

		QString dbVersion = this->readDatabaseVersion();
		if (dbVersion.startsWith("Error")) {
			return;
		}
		if (config.debug) {
			qDebug() << "Database connection successfully created";
			qDebug() << "Database version" << dbVersion;
		}

		readSettingsFromDatabase();

		// This will build all services list
		// actions should always be loaded before services
		this->actions = new QAstCTIActions(this);
		this->services = new QAstCTIServices(this->actions, this);
		this->ctiOperators = new QAstCTIOperators(this);

		// ConfigurationChecker will check if database configuration will change
		this->configChecker = new ConfigurationChecker(config.debug);
		connect(this->configChecker, SIGNAL(newConfiguration()),
				this, SLOT(reloadSettings()));

		QThread *configThread = new QThread(this);
		this->configChecker->moveToThread(configThread);
		connect(configThread, SIGNAL(started()),
				this->configChecker, SLOT(run()));
		connect(configThread, SIGNAL(finished()),
				this->configChecker, SLOT(deleteLater()));
		configThread->start();

        canStart = true;
    }
}

CtiServerApplication::~CtiServerApplication()
{
    if (!this->isRunning()) {
		if (config.debug)
			qDebug() << "In CtiServerApplication::~CtiServerApplication()";

		this->configChecker->stop();
		// configChecker will be deleted automatically because the thread's
		// finished() signal is connected to configChecker's deleteLater() slot
		QThread *configThread = this->configChecker->thread();
		configThread->quit();
		configThread->wait();
		delete configThread;

		delete this->coreTcpServer;
		delete this->ctiOperators;
		delete this->services;
		delete this->actions;

		if (config.debug)
			qDebug() << "CtiServerApplication closing";

		DB::destroyConnection();

		if (config.debug) {
			qDebug() << "Database connection closed";
			qDebug() << "Stopped";
		}

        // Remove msg handler to avoid access to this QApplication
        qInstallMsgHandler(0);
    }
}

void  CtiServerApplication::reloadSettings()
{
    // TODO : About race condition, you could add all the data (services, actions...) to the
    // TODO : main configuration struct. Then, when database is changed, you could read the
    // TODO : data from within the configurationchecker thread and put it in a temporary
    // TODO : config struct. Once you are sure that you read all the data, you can pass the
    // TODO : temporary struct to the main thread within newConfiguration signal, and make
    // TODO : it active (and delete the old one, of course).

	// TODO : This mutex currently has no purpose
	configMutex.lock();

	delete this->actions;
	this->actions = new QAstCTIActions(this);

	delete this->services;
	this->services = new QAstCTIServices(this->actions, this);

	delete this->ctiOperators;
	this->ctiOperators = new QAstCTIOperators(this);

	configMutex.unlock();
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
	if (!canStart)
		return 0;

    this->coreTcpServer = new CoreTcpServer(&this->config, this);
//	connect(this->coreTcpServer, SIGNAL(destroyed()),
//			this, SLOT(quit()));
    return this->coreTcpServer;
}

QString CtiServerApplication::readDatabaseVersion()
{
    bool ok;
    QVariant result = DB::readScalar("SELECT VERSION FROM dbversion LIMIT 1", &ok);
    if (!ok)
        result = "Error reading database version";

    return result.toString();
}

void CtiServerApplication::readSettingsFromFile(const QString path)
{
	if (config.debug)
		qDebug() << "Reading settings from file" << path;

	// Check that the config file exists
	QFile cfile(path);
	if (cfile.exists() && cfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cfile.close();

        // Instantiate a QSettings object for read/write from/to ini file
		QSettings settings(path, QSettings::IniFormat);

        settings.beginGroup("RuntimeDb");
		config.sqlHost     = settings.value("host",     defaultSqlHost ).toString();
		config.sqlUserName = settings.value("user",     defaultSqlUser).toString();
		config.sqlPassWord = settings.value("password", defaultSqlPassWord).toString();
		config.sqlPort     = settings.value("port",     defaultSqlPort).toInt();
		config.sqlDatabase = settings.value("database", defaultSqlDatabase).toString();
        settings.endGroup();
    } else {
		config.sqlHost     = defaultSqlHost;
		config.sqlUserName = defaultSqlUser;
		config.sqlPassWord = defaultSqlPassWord;
		config.sqlPort     = defaultSqlPort;
		config.sqlDatabase = defaultSqlDatabase;
    }
}

void CtiServerApplication::readSettingsFromDatabase()
{
	if (config.debug)
		qDebug() << "Reading settings from database";

	config.amiHost =
			readSetting("ami_host",                     defaultAmiHost).toString();
	config.amiPort =
			readSetting("ami_port",                     defaultAmiPort).toInt();
	config.amiUser =
			readSetting("ami_user",                     defaultAmiUser).toString();
	config.amiSecret =
			readSetting("ami_secret",                   defaultAmiSecret).toString();
	config.amiConnectTimeout =
			readSetting("ami_connect_timeout",          defaultAmiConnectTimeout).toInt();
	config.amiReadTimeout =
			readSetting("ami_read_timeout",             defaultAmiReadTimeout).toInt();
	config.amiConnectRetryAfter =
			readSetting("ami_connect_retry_after",      defaultAmiConnectRetryAfter).toInt();
	config.ctiServerPort =
			readSetting("cti_server_port",              defaultCtiServerPort).toInt();
	config.ctiConnectTimeout =
			readSetting("cti_connect_timeout",          defaultCtiConnectTimeout).toInt();
	config.ctiReadTimeout =
			readSetting("cti_read_timeout",             defaultCtiReadTimeout).toInt();
	config.ctiSocketCompressionLevel =
			readSetting("cti_socket_compression_level", defaultCtiSocketCompressionLevel).toInt();
}

QVariant CtiServerApplication::readSetting(const QString &name, const QVariant &defaultValue)
{
	QVariantList params;
	params.append(name);
    bool ok;
	QVariant value = DB::readScalar("SELECT val FROM server_settings WHERE name=?", params, &ok);
    if (ok)
		return value;
    else
        //TODO: We return the default value in case of an error.
        //TODO: Perhaps execution should be stopped in that case
        return defaultValue;
}

QAstCTIOperator *CtiServerApplication::getOperatorByUsername(const QString& username)
{
	if (this->ctiOperators != 0)
		return this->ctiOperators->operator [](username);

	return 0;
}

bool CtiServerApplication::containsUser(const QString &username)
{
    return this->coreTcpServer->containsUser(username);
}

void CtiServerApplication::addUser(const QString &username)
{
    this->coreTcpServer->addUser(username);
}

void CtiServerApplication::removeUser(const QString &username)
{
    this->coreTcpServer->removeUser(username);
}
