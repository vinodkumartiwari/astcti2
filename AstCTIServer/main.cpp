#include "main.h"
#include "mainserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QSettings settings("settings.ini", QSettings::IniFormat );

    settings.beginGroup("Server");
    writeSetting(&settings, "port", DEFAULT_SERVER_PORT);
    writeSetting(&settings, "readTimeout" , DEFAULT_READ_TIMEOUT);
    settings.endGroup();

    QAsteriskCTILogger logger;
    QAstCTIConfiguration config;

    qDebug("Creating MainServer");
    logger.writeToLog(LOG_INFO, "Creating MainServer");

    settings.beginGroup("Server");

    config.readTimeout = settings.value("readTimeout").toInt();
    config.serverPort = settings.value("port").toInt();

    settings.endGroup();


    qDebug() << "MainServer listening on port " <<  config.serverPort;
    MainServer server(&config);
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
