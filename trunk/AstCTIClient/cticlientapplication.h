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
 * AstCTIClient.  If you copy code from other releases into a copy of GNU
 * AstCTIClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTIClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#ifndef CTICLIENTAPPLICATION_H
#define CTICLIENTAPPLICATION_H

#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtCore/QUrl>
#include <QtCore/QPointer>

#include "argumentlist.h"
#include "cticonfig.h"
#include "compactwindow.h"
#include "serverconnection.h"

const QString defaultServerHost = "localhost";
const QString defaultServerPort = "5000";
const QString defaultConnectTimeout = "1500";
const QString defaultConnectRetryInterval = "2";
const int defaultKeepAliveInterval = 5000;

class BrowserWindow;
class CompactWindow;
//class MainWindow;
class LoginWindow;

class CtiClientApplication : public QApplication
{
    Q_OBJECT

public:
    CtiClientApplication(int &argc, char **argv);
    ~CtiClientApplication();
    static CtiClientApplication *instance();

    bool showLoginWindow();
    void createServerConnection();

    AstCTIConfiguration config; // Main configuration struct

    //MainWindow *newMainWindow();
    BrowserWindow *newBrowserWindow();

signals:
    void newMessage(const QString &message, QSystemTrayIcon::MessageIcon severity);
    void closeWindow(bool skipCheck);

public slots:
    void loginAccept(const QString &username, const QString &password);
    void loginReject();

    void logOff();

    void eventReceived(AstCTICall *astCTICall);
    void servicesReceived(QHash<QString, QString> *servicesList);
    void queuesReceived(QStringList *queuesList);
    void loggedIn(const QString &extension);
    void pauseAccepted();
    void pauseError(const QString &message);
    void connectionLost();
    void threadStopped(StopReason stopReason, const QString &message);

private:
    bool canStart;

    ServerConnection *servConn;
    QList< QPointer<BrowserWindow> > m_mainWindows;
    LoginWindow *m_loginWnd;
    QWidget *m_mainWnd;
};

#endif // CTICLIENTAPPLICATION_H
