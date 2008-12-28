#ifndef MAINSERVER_H
#define MAINSERVER_H

#include <QObject>
#include <QtNetwork>
#include <QSettings>
#include "cticonfig.h"
class MainServer : public QTcpServer
{
    Q_OBJECT

private:
    QSettings settings;
    QAstCTIConfiguration *config;

public:
    MainServer(QAstCTIConfiguration *config, QObject *parent=0);


protected:
    void incomingConnection(int socketDescriptor);


};


#endif // MAINSERVER_H
