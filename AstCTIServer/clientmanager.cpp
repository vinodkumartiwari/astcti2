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
