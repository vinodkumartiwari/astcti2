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

QAsteriskCTILogger::QAsteriskCTILogger(QObject* parent) :
        QObject (parent)
{
}

void QAsteriskCTILogger::write_to_log(QtMsgType level, const QString& logData)
{
    this->build_log_directory_if_not_exists();
    QDir baseDirectory = this->logging_directory;

    QString todayFilename = QString("log_").append(QDate::currentDate().toString("yyyyMMdd").append(".log"));

    QFile logFile(baseDirectory.absolutePath().append("/").append(todayFilename));
    if (!logFile.open(QIODevice::Append | QIODevice::Text ))
    {
        qDebug() << "Cannot open log_file" << todayFilename;
        return;
    }

    QString logString;
    QByteArray dataArray;
    QBuffer dataBuffer( &dataArray );
    dataBuffer.open( QIODevice::WriteOnly );
    QTextStream text_stream( &dataBuffer );
    text_stream << QTime::currentTime().toString("hh:mm:ss")  << " (" << get_level_description_from_id(level) << "): " << logData << "\n\r";
    dataBuffer.close();

    logFile.write(dataArray);

    logFile.close();
}

bool QAsteriskCTILogger::build_log_directory_if_not_exists()
{
    bool bIsOk = true;
    QString appPath = QCoreApplication::applicationDirPath();
    appPath.append("/logs/");
    QDir logdir(appPath);
    if (!logdir.exists())
    {
        bIsOk = logdir.mkdir(appPath);
    }
    this->logging_directory = logdir;
    return bIsOk;
}

QString QAsteriskCTILogger::get_level_description_from_id(const QtMsgType level)
{
    QString retLevel = "NONE";
    switch (level)
    {
        case QtDebugMsg:
            retLevel = "DEBUG";
            break;
        case QtWarningMsg:
            retLevel = "WARNING";
            break;
        case QtCriticalMsg:
            retLevel = "CRITICAL";
            break;
        case QtFatalMsg:
            retLevel = "FATAL";
    }
    return retLevel;
}
