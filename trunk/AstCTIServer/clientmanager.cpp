#include "clientmanager.h"


ClientManager::ClientManager(QAstCTIConfiguration *config, int socketDescriptor, QObject *parent) :
        QThread(parent), socketDescriptor(socketDescriptor)
{
    this->config = config;
    qDebug() << "Read Timeout is " << config->readTimeout;

}

void ClientManager::run()
{
    QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        qDebug() << tcpSocket.errorString();
        return;
    }

    QHostAddress remote_addr = tcpSocket.peerAddress();
    quint16 remote_port = tcpSocket.peerPort();

    qDebug() << "Incoming Connection from " << remote_addr.toString() << ":" << remote_port;

    const QString separator = "<eol>";
    this->sendData(&tcpSocket, QString("Welcome to AstCTIServer rel. ").append(ASTCTISRV_RELEASE));

    while(tcpSocket.waitForReadyRead(config->readTimeout))
    {
        QString strdata(tcpSocket.readAll());
        buffer.append(strdata.trimmed());
        if(buffer.contains(separator))
        {
            QString toSplit = "";
            QString remainingBuffer = "";
            int lastIndexOf = buffer.lastIndexOf(separator);
            if (lastIndexOf < buffer.size()-separator.size())
            {
                toSplit=buffer.left(lastIndexOf);
                remainingBuffer = buffer.mid(lastIndexOf+ separator.size());

            } else {
                toSplit=buffer;
                remainingBuffer = "";

            }
            buffer = remainingBuffer;

            QStringList list = toSplit.split(separator);
            QString itm;
            foreach(itm, list)
            {
                itm = itm.trimmed();
                if (itm.size() > 0)
                    qDebug() << "--> Received " << itm;
            }
        }

    }
    tcpSocket.close();

    qDebug() << "Connection from " << remote_addr.toString() << ":" << remote_port << " closed";
}

void ClientManager::sendData(QTcpSocket *tcpSocket, QString data)
{
    data = data.append("\n");
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << data;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpSocket->write(block);
}
