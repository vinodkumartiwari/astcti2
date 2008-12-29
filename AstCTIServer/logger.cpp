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

#include "logger.h"

QAsteriskCTILogger::QAsteriskCTILogger(QObject *parent) : QObject (parent)
{
}

void QAsteriskCTILogger::writeToLog(QAsteriskCTILoggerLevel level, const QString &logdata)
{
    this->buildLogDirectory();
    QDir baseDir = this->logDirectory;

    QString todayFileName = QString("log_").append(QDate::currentDate().toString("yyyyMMdd").append(".log"));

    QFile logFile(baseDir.absolutePath().append("/").append(todayFileName));
    if (!logFile.open(QIODevice::Append | QIODevice::Text ))
    {
        qDebug() << "Cannot open logfile" << todayFileName;
        return;
    }
    QString logstring;


    QByteArray dataArray;
    QBuffer buf( &dataArray );
    buf.open( QIODevice::WriteOnly );
    QTextStream ts( &buf );
    ts << QTime::currentTime().toString("hh:mm:ss")  << " (" << levelStringFromLevelId(level) << "): " << logdata << "\n\r";
    buf.close();

    logFile.write(dataArray);

    logFile.close();
}

bool QAsteriskCTILogger::buildLogDirectory()
{
    bool bIsOk = true;
    QString appPath = QCoreApplication::applicationDirPath();
    appPath.append("/logs/");
    QDir logdir(appPath);
    if (!logdir.exists())
    {
        bIsOk = logdir.mkdir(appPath);
    }
    this->logDirectory = logdir;
    return bIsOk;
}

QString QAsteriskCTILogger::levelStringFromLevelId(const QAsteriskCTILoggerLevel level)
{
    switch(level)
    {
        case LOG_NOTICE:
            return "NOTICE";
            break;
        case LOG_INFO:
            return "INFO";
            break;
        case LOG_WARNING:
            return "WARNING";
            break;
        case LOG_ERROR:
            return "ERROR";
            break;
        case LOG_FATAL:
            return "FATAL";
            break;
        case LOG_ALL:
            return "ALL";
            break;
    }
    return "NONE";
}
