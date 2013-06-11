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
#include <QDebug>
#include <QSettings>

#include "ctiserverapplication.h"
#include "db.h"

CtiServerApplication::CtiServerApplication(const QString &appId, int &argc, char **argv)
		: QtSingleCoreApplication(appId, argc, argv)
{
	this->canStart = false;

	if (!this->isRunning()) {
		this->config = 0;

		QArgumentList args(argc, argv);
		this->debug = args.getSwitch("--debug") | args.getSwitch("-d");
        QString configFile = args.getSwitchArg("-c", "settings.ini");

		// Read settings for database connection from file and create connection
		if (!createDatabaseConnection(configFile))
			return;

		// ConfigurationChecker will check if database configuration will change
		this->configChecker = new ConfigurationChecker(this->debug);
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

		this->canStart = true;
    }
}

CtiServerApplication::~CtiServerApplication()
{
    if (!this->isRunning()) {
		if (this->debug)
			qDebug() << "CtiServerApplication closing";

		//CoreTcpServer object will be deleted automatically as this object is its parent

		this->configChecker->stop();
		// configChecker will be deleted automatically because the thread's
		// finished() signal is connected to configChecker's deleteLater() slot
		QThread *configThread = this->configChecker->thread();
		configThread->quit();
		configThread->wait();
		delete configThread;

		DB::destroyConnection();

		if (this->debug) {
			qDebug() << "Database connection closed";
			qDebug() << "Stopped";
		}

		delete this->config;

        // Remove msg handler to avoid access to this QApplication
        qInstallMsgHandler(0);
    }
}

bool CtiServerApplication::getCanStart() {
	return this->canStart;
}

void  CtiServerApplication::reloadSettings(AstCtiConfiguration *newConfig)
{
	if (this->config == 0) {
		//Configuration has been read for the first time
		this->config = newConfig;
		//This will create CoreTcpServer object which will initiate AMI connection
		//and start acccepting client connections
		if (!this->buildCoreTcpServer()) {
			this->exit(exitCodeFailure);
		}
	} else {
		// TODO : Compare old and new configurations and act accordingly
		// TODO : If AMI parameters are changed, AMI connection should be dropped and reestablished
		// TODO : If CTI server address or port are changed, all clients should be disconnected
		// TODO : If operators or seats are changed, some clients may have to be disconnected

		AstCtiConfiguration *tempConfig = this->config;
		this->config = newConfig;
		delete tempConfig;
	}
}

CtiServerApplication *CtiServerApplication::instance()
{
    return (static_cast<CtiServerApplication*>(QCoreApplication::instance()));
}

bool CtiServerApplication::buildCoreTcpServer()
{
	this->coreTcpServer = new CoreTcpServer(this->debug, this->config, this);
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
	if (this->debug)
		qDebug() << "Reading settings from file" << iniFilePath;

	QString sqlHost;
	QString sqlUserName;
	QString sqlPassWord;
	quint16 sqlPort;
	QString sqlDatabase;

	// Check that the config file exists
	QFile cfile(iniFilePath);
	if (cfile.exists() && cfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cfile.close();

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

	DB::debug = this->debug;
	if (!DB::buildConnection(sqlHost, sqlPort, sqlUserName, sqlPassWord, sqlDatabase)) {
		return false;
	}

	QString dbVersion = this->readDatabaseVersion();
	if (dbVersion.startsWith("Error")) {
		return false;
	}

	if (this->debug) {
		qDebug() << "Database connection successfully created";
		qDebug() << "Database version" << dbVersion;
	}
	return true;
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
