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

#include <QFile>
#include <QFileInfo>
#include <QSettings>

#include "QsLog.h"
#include "QsLogDest.h"
#include "ctiserverapplication.h"
#include "db.h"

CtiServerApplication::CtiServerApplication(const QString &appId, int &argc, char **argv)
		: QtSingleCoreApplication(appId, argc, argv)
{
	this->canStart = false;

	if (!this->isRunning()) {
		this->coreTcpServer = 0;
		this->configChecker = 0;

		ArgumentList args(argc, argv);
		int verbosity = args.getSwitchMulti("v");
		QString configFileName = args.getSwitchArg("c", "settings.ini");
		QString logFileName = args.getSwitchArg("l", "");
		if (logFileName.isEmpty())
			if (args.getSwitch("l")) {
				//The switch can be specified without argument,
				//in which case default filename is used
				logFileName = QCoreApplication::applicationDirPath();
				logFileName.append("/logs/astctiserver.log");
			}

		// Init the logging mechanism
		QsLogging::Logger &logger = QsLogging::Logger::instance();

		QsLogging::Level loggingLevel;
		switch (verbosity) {
		case 0:
			loggingLevel = QsLogging::FatalLevel;
			break;
		case 1:
			loggingLevel = QsLogging::ErrorLevel;
			break;
		case 2:
			loggingLevel = QsLogging::WarnLevel;
			break;
		case 3:
			loggingLevel = QsLogging::InfoLevel;
			break;
		case 4:
			loggingLevel = QsLogging::DebugLevel;
			break;
		default:
			loggingLevel = QsLogging::TraceLevel;
			break;
		}
		logger.setLoggingLevel(loggingLevel);

		//Debug destination is always active
		logger.addDestination(QsLogging::DestinationFactory::MakeDebugOutputDestination());
		if (!logFileName.isEmpty()) {
			QFileInfo logFileInfo(logFileName);
			QDir logDir = logFileInfo.absoluteDir();
			if (logDir.exists() || logDir.mkdir(logDir.absolutePath()))
				logger.addDestination(
						QsLogging::DestinationFactory::MakeFileDestination(logFileName));
			else
				QLOG_ERROR() << "Unable to create log directory" << logDir.absolutePath();
		}

		// Read settings for database connection from file and create connection
		if (!createDatabaseConnection(configFileName))
			return;

		this->canStart = true;
    }
}

CtiServerApplication::~CtiServerApplication()
{
    if (!this->isRunning()) {
		QLOG_INFO() << "CtiServerApplication closing";

		//CoreTcpServer object will be deleted automatically as this object is its parent

		if (this->configChecker != 0) {
			this->configChecker->stop();
			// configChecker will be deleted automatically because the thread's
			// finished() signal is connected to configChecker's deleteLater() slot
			QThread *configThread = this->configChecker->thread();
			configThread->quit();
			configThread->wait();
			delete configThread;
		}

		DB::destroyConnection();
		QLOG_INFO() << "Database connection closed";

		QLOG_INFO() << "Stopped";
    }
}

bool CtiServerApplication::getCanStart() {
	return this->canStart;
}

bool CtiServerApplication::start()
{
	// ConfigurationChecker will check if database configuration will change
	this->configChecker = new ConfigurationChecker();
	connect(this->configChecker, SIGNAL(newConfiguration(AstCtiConfiguration*)),
			this, SLOT(reloadSettings(AstCtiConfiguration*)));

	QThread *configThread = new QThread(this);
	this->configChecker->moveToThread(configThread);
	connect(configThread, SIGNAL(started()),
			this->configChecker, SLOT(run()));
	connect(configThread, SIGNAL(finished()),
			this->configChecker, SLOT(deleteLater()));
	configThread->start();

	//Configuration checker thread will read configuration from database
	//And fire newConfiguration signal, which we will catch in reloadSettings slot

	return this->exec();
}

void  CtiServerApplication::reloadSettings(AstCtiConfiguration *newConfig)
{
	if (this->coreTcpServer == 0) {
		//Configuration has not been read before

		if (newConfig == 0) {
			//Reading configuration has failed, exit the application
			this->exit(exitCodeFailure);
			return;
		}

		//This will create CoreTcpServer object which will initiate AMI connection
		//and start acccepting client connections
		if (!this->buildCoreTcpServer(newConfig)) {
			this->exit(exitCodeFailure);
		}
	} else {
		if (newConfig == 0) {
			//Reading configuration has failed, we keep the current configuration
			//Config checker will try to read configuration again
			return;
		}

		//CoreTcpServer will apply new configuration
		this->coreTcpServer->setConfig(newConfig);
	}
}

CtiServerApplication *CtiServerApplication::instance()
{
    return (static_cast<CtiServerApplication*>(QCoreApplication::instance()));
}

bool CtiServerApplication::buildCoreTcpServer(AstCtiConfiguration *config)
{
	this->coreTcpServer = new CoreTcpServer(config, this);
//	connect(this->coreTcpServer, SIGNAL(destroyed()),
//			this, SLOT(quit()));
	return true;
}

QString CtiServerApplication::readDatabaseVersion()
{
    bool ok;
    QVariant result = DB::readScalar("SELECT VERSION FROM dbversion LIMIT 1", &ok);
    if (!ok)
        result = "Error reading database version";

    return result.toString();
}

bool CtiServerApplication::createDatabaseConnection(const QString &iniFilePath)
{
	QLOG_INFO() << "Reading settings from file" << iniFilePath;

	QString sqlHost;
	QString sqlUserName;
	QString sqlPassWord;
	quint16 sqlPort;
	QString sqlDatabase;

	QFile configFile(iniFilePath);
	// Make sure we are in application's directory if relative path was given
	QDir::setCurrent(CtiServerApplication::instance()->applicationDirPath());
	// Check that the config file exists
	if (configFile.exists() && configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		configFile.close();

        // Instantiate a QSettings object for read/write from/to ini file
		QSettings settings(iniFilePath, QSettings::IniFormat);

        settings.beginGroup("RuntimeDb");
		sqlHost     = settings.value("host",     defaultSqlHost ).toString();
		sqlUserName = settings.value("user",     defaultSqlUser).toString();
		sqlPassWord = settings.value("password", defaultSqlPassWord).toString();
		sqlPort     = settings.value("port",     defaultSqlPort).toInt();
		sqlDatabase = settings.value("database", defaultSqlDatabase).toString();
        settings.endGroup();
    } else {
		sqlHost     = defaultSqlHost;
		sqlUserName = defaultSqlUser;
		sqlPassWord = defaultSqlPassWord;
		sqlPort     = defaultSqlPort;
		sqlDatabase = defaultSqlDatabase;
    }

	if (!DB::buildConnection(sqlHost, sqlPort, sqlUserName, sqlPassWord, sqlDatabase)) {
		return false;
	}

	QString dbVersion = this->readDatabaseVersion();
	if (dbVersion.startsWith("Error")) {
		return false;
	}

	QLOG_INFO() << "Database connection successfully created";
	QLOG_INFO() << "Database version" << dbVersion;

	return true;
}
