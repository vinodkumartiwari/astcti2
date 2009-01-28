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


CtiClientApplication::CtiClientApplication(int &argc, char **argv)
             : QApplication(argc, argv)
{
    QString locale = QLocale::system().name();

    QTranslator translator;
    translator.load(QString("AstCTIClient_") + locale);
    installTranslator(&translator);

#if defined(Q_WS_MAC)
    QApplication::setQuitOnLastWindowClosed(false);
#else
        QApplication::setQuitOnLastWindowClosed(true);
#endif

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


MainWindow *CtiClientApplication::newMainWindow()
{
    // Let's build our main window
    this->m_mainWnd = new MainWindow();
    this->m_mainWnd->show();
    return this->m_mainWnd;
}


BrowserWindow *CtiClientApplication::newBrowserWindow()
{
    // All browsers will be child of our main window
    // if we close main window all browser' windows will close too
    BrowserWindow *browser = new BrowserWindow(this->m_mainWnd);
    m_mainWindows.prepend(browser);
    browser->show();
    return browser;

}
