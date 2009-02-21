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


    m_canStart = false;

    QArgumentList args(argc, argv);
    config.qDebug = args.getSwitch("--debug") | args.getSwitch("-d");
    QString configFile = args.getSwitchArg("-c", "settings.ini");


    if (config.qDebug) qDebug("Reading Configurations");

    if (!readSettingsFile(configFile, &config))
    {
        return;
    }

    if (!buildSqlDatabase(&config))
    {
        return;
    }
    m_canStart = true;
}

CtiServerApplication *CtiServerApplication::instance()
{
    return (static_cast<CtiServerApplication *>(QCoreApplication::instance()));
}

CtiServerApplication::~CtiServerApplication()
{
    if (config.qDebug) qDebug() << "MainServer closing";
    this->destroySqlDatabase(&this->config);
    if (config.qDebug) qDebug() << "SQLite database connection closed";
}

MainServer *CtiServerApplication::newMainServer()
{
    if (!m_canStart) return NULL;

    // Let's build our main window
    this->m_mainServer = new MainServer(&this->config);

    // Say we're listening on port..
    if (config.qDebug) qDebug() << "MainServer listening on port " <<  config.serverPort;

    // TODO : fix to listen also on specific addresses
    this->m_mainServer->listen(QHostAddress::Any, this->config.serverPort );
    connect(this->m_mainServer, SIGNAL(destroyed()), this, SLOT(quit()));

    return this->m_mainServer;
}

bool CtiServerApplication::buildSqlDatabase(QAstCTIConfiguration *config)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "sqlitedb");
    db.setDatabaseName(config->sqlite_file);

    if (config->sqlite_user.length() > 0)
        db.setUserName(config->sqlite_user);

    if (config->sqlite_secret.length() > 0)
        db.setPassword(config->sqlite_secret);

    if(!db.open())
    {
        qCritical() << "Unable to open SQLite database" << config->sqlite_file << "\n";
        return false;
    }

    if (config->qDebug) qDebug() << "SQLite database successfully opened" << config->sqlite_file << "\n";

    if (config->qDebug) qDebug() << "SQLite database version " << this->databaseVersion() << "\n";

    config->db = &db;
    return true;

}

QString CtiServerApplication::databaseVersion()
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

void CtiServerApplication::destroySqlDatabase(QAstCTIConfiguration *config)
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
    config->db = NULL;
}

bool CtiServerApplication::readSettingsFile(const QString configFile, QAstCTIConfiguration *config)
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
    writeSetting(&settings, "port",             DEFAULT_SERVER_PORT);
    writeSetting(&settings, "readTimeout",      DEFAULT_READ_TIMEOUT);
    writeSetting(&settings, "compressionLevel", DEFAULT_COMPRESSION_LEVEL);
    settings.endGroup();

    // write default values for Asterisk AMI
    settings.beginGroup("AsteriskAMI");
    writeSetting(&settings, "host",            DEFAULT_AMI_HOSTIP);
    writeSetting(&settings, "port",            DEFAULT_AMI_PORT);
    writeSetting(&settings, "user",            DEFAULT_AMI_USER);
    writeSetting(&settings, "secret",          DEFAULT_AMI_SECRET);
    writeSetting(&settings, "connect_timeout", DEFAULT_AMI_CONNECT_TIMEOUT);
    settings.endGroup();

    settings.beginGroup("RuntimeDb");
    writeSetting(&settings, "file",            "asteriskcti.db3");
    writeSetting(&settings, "user",            "");
    writeSetting(&settings, "secret",          "");
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
    config->sqlite_file          = settings.value("file").toString();
    config->sqlite_user          = settings.value("user").toString();
    config->sqlite_secret        = settings.value("secret").toString();
    settings.endGroup();
    return true;
}

void CtiServerApplication::writeSetting( QSettings *settings, const QString & key, const QVariant & defvalue)
{
    if (!settings->contains(key))
    {
        settings->setValue(key,defvalue);
    }
}
