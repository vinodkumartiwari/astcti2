#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include <QStringList>
#include <QDebug>
#include <QSettings>
#include "cticonfig.h"

class ClientManager : public QThread
{
public:
    ClientManager(QAstCTIConfiguration *config, int socketDescriptor, QObject *parent);
    void run();
    void sendData(QTcpSocket *tcpSocket, QString data);


private:
    QAstCTIConfiguration *config;
    int socketDescriptor;
    QString buffer;
};

#endif // CLIENTMANAGER_H
