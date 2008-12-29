#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QtCore>


enum QAsteriskCTILoggerLevel {
    LOG_NOTICE = 1,
    LOG_INFO = 2,
    LOG_WARNING = 4,
    LOG_ERROR = 8,
    LOG_FATAL = 16,
    LOG_ALL = LOG_NOTICE & LOG_INFO & LOG_WARNING & LOG_ERROR & LOG_FATAL
};


class QAsteriskCTILogger : public QObject
{
    Q_OBJECT

public:
    QAsteriskCTILogger(QObject *parent=0);
    void writeToLog(const QAsteriskCTILoggerLevel level=LOG_NOTICE, const QString &logdata="");

private:
    bool buildLogDirectory();
    QDir logDirectory;
};

#endif // LOGGER_H
