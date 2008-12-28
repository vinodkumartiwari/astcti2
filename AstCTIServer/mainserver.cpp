#include "main.h"
#include "mainserver.h"
#include "clientmanager.h"

#include <stdlib.h>

//! [0]
MainServer::MainServer(QAstCTIConfiguration *config, QObject *parent)
    : QTcpServer(parent)
{
    this->config = config;
    qDebug("MainServer initialization");
    // Basic init here

}

void MainServer::incomingConnection(int socketDescriptor)
{
    ClientManager *thread = new ClientManager(this->config, socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
