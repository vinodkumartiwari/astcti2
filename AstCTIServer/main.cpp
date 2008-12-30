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

#include "main.h"
#include "mainserver.h"

int main(int argc, char *argv[])
{
    // QCoreApplication needs to be initialized here
    QCoreApplication a(argc, argv);

    QString                 logtxt; // A string for logging purposes
    QAsteriskCTILogger      logger; // Our logger object
    QAstCTIConfiguration    config; // Main configuration struct

    // Please, no debug on console if not requested
    config.qDebug = false;

    // Check command line if some arguments are set
    if (QCoreApplication::arguments().count() > 1)
    {
        // Check if debug on command line is enabled
        config.qDebug = (QCoreApplication::arguments().contains("-debug"));
    }

    if (config.qDebug) qDebug("Creating MainServer");
    logtxt.clear(); QTextStream(&logtxt) << "Creating MainServer";
    logger.writeToLog(LOG_INFO, logtxt);

    if (config.qDebug) qDebug("Reading Configurations");
    logtxt.clear(); QTextStream(&logtxt) << "Creating MainServer";
    logger.writeToLog(LOG_INFO, "Reading Configurations");

    // Instantiate a QSettings object for read/write from/to ini file
    QSettings settings("settings.ini", QSettings::IniFormat );

    // Just for the group named Server
    // write default values if keys are not set
    settings.beginGroup("Server");
    writeSetting(&settings, "port",             DEFAULT_SERVER_PORT);
    writeSetting(&settings, "readTimeout",      DEFAULT_READ_TIMEOUT);
    writeSetting(&settings, "compressionLevel", DEFAULT_COMPRESSION_LEVEL);
    settings.endGroup();

    // read values from the keys and store them in a config object
    settings.beginGroup("Server");
    config.readTimeout      = settings.value("readTimeout").toInt();
    config.serverPort       = settings.value("port").toInt();
    config.compressionLevel = settings.value("compressionLevel").toInt();
    settings.endGroup();


    logtxt.clear(); QTextStream(&logtxt) << "MainServer version " << ASTCTISRV_RELEASE;
    logger.writeToLog(LOG_INFO, logtxt);

    // Say we're listening on port..
    if (config.qDebug) qDebug() << "MainServer listening on port " <<  config.serverPort;
    logtxt.clear(); QTextStream(&logtxt) << "MainServer listening on port " << config.serverPort;
    logger.writeToLog(LOG_INFO, logtxt );

    // Here we start the server
    MainServer server(&config);

    // TODO : fix to listen also on specific addresses
    server.listen(QHostAddress::Any,  config.serverPort );

    return a.exec();
}

void writeSetting( QSettings *settings, const QString & key, const QVariant & defvalue)
{
    if (!settings->contains(key))
    {
        settings->setValue(key,defvalue);
    }
}
