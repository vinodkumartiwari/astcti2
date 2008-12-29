#include "logger.h"

QAsteriskCTILogger::QAsteriskCTILogger(QObject *parent) : QObject (parent)
{
}

void QAsteriskCTILogger::writeToLog(QAsteriskCTILoggerLevel level, const QString &logdata)
{
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
    QTextStream ts( &logstring );
    ts << QTime::currentTime().toString("hh:mm:ss")  << ":" << logdata;
    buf.close();

    logFile.write(dataArray);

    logFile.close();
}

bool QAsteriskCTILogger::buildLogDirectory()
{
    QDir logdir;
    if (!logdir.exists("logs/"))
    {
        this->logDirectory = logdir;
        return logdir.mkdir("logs/");
    }



    return false;
}
