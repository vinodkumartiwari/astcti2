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

#include "cticlientapplication.h"

#include "mainwindow.h"
#include "browserwindow.h"
#include "loginwindow.h"

CtiClientApplication::CtiClientApplication(int &argc, char **argv)
             : QApplication(argc, argv)
{
    canStart = false;

    QString locale = QLocale::system().name();

    QTranslator translator;
    translator.load(QString("AstCTIClient_") + locale);
    installTranslator(&translator);

#if defined(Q_WS_MAC)
    QApplication::setQuitOnLastWindowClosed(false);
#else
    QApplication::setQuitOnLastWindowClosed(true);
#endif

    QArgumentList args(argc, argv);
    config.qDebug = args.getSwitch("--debug") | args.getSwitch("-d");

    config.serverHost = args.getSwitchArg("-h", defaultServerHost);
    if (config.serverHost.isEmpty()) {
        qFatal("No value given for Server Host (-h). If you want to use default host, don't include the command line switch.");
        return;
    }
    bool ok;
    config.serverPort = args.getSwitchArg("-p", defaultServerPort).toUShort(&ok);
    if (!ok) {
        qFatal("Wrong value given for Server Port (-p). Expected number from 0 to 65535.");
        return;
    }
    config.connectTimeout = args.getSwitchArg("-t", defaultConnectTimeout).toInt(&ok);
    if (!ok || config.connectTimeout < -1) {
        qFatal("Wrong value given for Connect Timeout (-t). Expected number from -1 to 2147483647.");
        return;
    }
    config.connectRetryInterval = args.getSwitchArg("-r", defaultConnectRetryInterval).toUInt(&ok);
    if (!ok) {
        qFatal("Wrong value given for Connect Retry Interval (-r). Expected number from 0 to 4294967295.");
        return;
    }

    config.compressionLevel = 0; //No compression initially. Compression level will be received from server.
    config.keepAliveInterval = defaultKeepAliveInterval; //Set to default initially. Interval will be received from server.

    this->m_loginWnd = 0;
    this->m_mainWnd = 0;
    this->servConn = 0;

    canStart = true;
}

CtiClientApplication::~CtiClientApplication()
{
    // delete all browser windows
    qDeleteAll(m_mainWindows);
}

CtiClientApplication *CtiClientApplication::instance()
{
    return (static_cast<CtiClientApplication *>(QCoreApplication::instance()));
}

void CtiClientApplication::createServerConnection()
{
    this->servConn = new ServerConnection(&this->config);

    qRegisterMetaType<AstCTICall>("AstCTICall");
    qRegisterMetaType< QHash<QString,QString> >("QHash<QString,QString>");
    qRegisterMetaType<StopReason>("StopReason");

    connect(this->servConn, SIGNAL(eventReceived(AstCTICall*)), this, SLOT(eventReceived(AstCTICall*)), Qt::QueuedConnection);
    connect(this->servConn, SIGNAL(servicesReceived(QHash<QString,QString>*)), this, SLOT(servicesReceived(QHash<QString,QString>*)), Qt::QueuedConnection);
    connect(this->servConn, SIGNAL(queuesReceived(QStringList*)), this, SLOT(queuesReceived(QStringList*)), Qt::QueuedConnection);
    connect(this->servConn, SIGNAL(loggedIn(QString)), this, SLOT(loggedIn(QString)), Qt::QueuedConnection);
    connect(this->servConn, SIGNAL(pauseAccepted()), this, SLOT(pauseAccepted()), Qt::QueuedConnection);
    connect(this->servConn, SIGNAL(pauseError(QString)), this, SLOT(pauseError(QString)), Qt::QueuedConnection);
    connect(this->servConn, SIGNAL(connectionLost()), this, SLOT(connectionLost()), Qt::QueuedConnection);
    connect(this->servConn, SIGNAL(threadStopped(StopReason,QString)), this, SLOT(threadStopped(StopReason,QString)), Qt::QueuedConnection);

    this->servConn->start();
}

void CtiClientApplication::loginAccept(const QString &username, const QString &password)
{
    this->config.user = username;
    this->config.pass = password;

    if (this->servConn == 0) {
        createServerConnection();
    }
}

void CtiClientApplication::loginReject()
{
    this->exit(0);
}

void CtiClientApplication::logOff()
{
    this->servConn->sendCommandToServer(CmdQuit);
}

void CtiClientApplication::eventReceived(AstCTICall *astCTICall)
{
   //TODO
   delete(astCTICall);
}

void CtiClientApplication::servicesReceived(QHash<QString, QString> *servicesList)
{
}

void CtiClientApplication::queuesReceived(QStringList *queuesList)
{
}

void CtiClientApplication::loggedIn(const QString &extension)
{
    if (this->m_mainWnd == 0) {
        CompactWindow *w = new CompactWindow();

        connect(this, SIGNAL(newMessage(QString,QSystemTrayIcon::MessageIcon)), w, SLOT(showMessage(QString,QSystemTrayIcon::MessageIcon)));
        connect(this, SIGNAL(closeWindow(bool)), w, SLOT(quit(bool)));

        connect(w, SIGNAL(logOff()), this, SLOT(logOff()));

        this->m_mainWnd = dynamic_cast<QWidget*>(w);
        w->show();
    }

    if (this->m_loginWnd->isVisible()) {
        this->m_loginWnd->hide();
    }

    emit this->newMessage("Connection to server OK.", QSystemTrayIcon::Information);
    //TODO: Extension
}

void CtiClientApplication::pauseAccepted()
{
}

void CtiClientApplication::pauseError(const QString &message)
{
}

void CtiClientApplication::connectionLost()
{
    if (this->m_mainWnd != 0) {
        emit this->newMessage("Connection to server lost. Trying to reconnect...", QSystemTrayIcon::Critical);
    } else {
        this->m_loginWnd->showMessage("AsteriskCTI client is unable to connect to AsteriskCTI server. "
                                      "Please check that the correct parameter is passed from the command line.\n\n"
                                      "AsteriskCTI client will keep trying to connect.", true);
        this->m_loginWnd->show();
    }
}

void CtiClientApplication::threadStopped(StopReason stopReason, const QString &message)
{
    delete this->servConn;
    this->servConn = 0;

    QString msg;

    switch (stopReason) {
    case StopUserDisconnect:
        return;
    case StopInternalError:
        msg = "Internal error occured:\n" + message;
        break;
    case StopServerError:
        msg = "Server error occured:\n" + message;
        break;
    case StopInvalidCredentials:
        msg = "Your credentials were not accepted:\n" + message;
        break;
    }

    if (this->m_mainWnd != 0) {
        QMessageBox msgBox;
        msgBox.setText(msg);
        msgBox.exec();
        emit this->closeWindow(true);
        this->m_mainWnd = 0;
    } else {
        this->m_loginWnd->showMessage(msg, false);
        this->m_loginWnd->show();
    }
}

bool CtiClientApplication::showLoginWindow()
{
    if (!canStart) return false;

    if (this->m_loginWnd == 0) {
        this->m_loginWnd = new LoginWindow();

        connect(this->m_loginWnd, SIGNAL(accepted(QString, QString)), this, SLOT(loginAccept(QString, QString)));
        connect(this->m_loginWnd, SIGNAL(rejected()), this, SLOT(loginReject()));
    }

    this->m_loginWnd->show();

    return true;
}

//MainWindow *CtiClientApplication::newMainWindow()
//{
//    // Let's build our main window
//    this->m_mainWnd = new MainWindow();
//    this->m_mainWnd->show();
//    return this->m_mainWnd;
//}

BrowserWindow *CtiClientApplication::newBrowserWindow()
{
    // All browsers will be child of our main window
    // if we close main window all browser' windows will close too
    BrowserWindow *browser = new BrowserWindow(this->m_mainWnd);
    m_mainWindows.prepend(browser);
    browser->show();
    return browser;
}
