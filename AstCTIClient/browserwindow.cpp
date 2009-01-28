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
#include "webview.h"
#include "browserwindow.h"
#include "ui_browserwindow.h"

BrowserWindow::BrowserWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::BrowserWindowClass)
{
    m_ui->setupUi(this);
    //this->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    this->addExtraWidgets();
}

BrowserWindow::~BrowserWindow()
{
    delete m_ui;
}

WebView *BrowserWindow::currentView() const
{
    return this->webView;
}

void BrowserWindow::addExtraWidgets()
{

    webView = new WebView(this->m_ui->centralwidget);
    webView->setObjectName(QString::fromUtf8("webView"));
    webView->setUrl(QUrl("about:blank"));
    this->m_ui->verticalLayout->addWidget(webView);

    lblCurrentStatus = new QLabel(trUtf8("<b>Not Connected</b>")) ;
    this->statusBar()->addWidget(lblCurrentStatus);

    connect(webView, SIGNAL(loadStarted()), this, SLOT(webView_loadStarted()));
    connect(webView, SIGNAL(loadProgress(int)), this, SLOT(webView_loadProgress(int)));
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(webView_loadFinished(bool)));
    connect(webView, SIGNAL(titleChanged(QString)), this, SLOT(webView_titleChanged(QString)));
    connect(webView, SIGNAL(linkClicked(QUrl)), this, SLOT(webView_linkClicked(QUrl)));

}
void BrowserWindow::setUrl(QUrl url)
{
    this->m_ui->txtUrl->setText(url.toString() );
}

void BrowserWindow::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void BrowserWindow::webView_loadStarted()
{
    QUrl url = this->webView->url();
    this->m_ui->txtUrl->setText(url.toString());
    this->m_ui->txtUrl->setEnabled(false);
    this->m_ui->btnGo->setText(trUtf8("&Stop"));
    this->lblCurrentStatus->setText(trUtf8("Loading"));




}

void BrowserWindow::on_btnGo_clicked()
{
    if (this->m_ui->btnGo->text()==trUtf8("&Stop"))
    {
        this->webView->stop();
    }
    else
    {
        QString url = this->m_ui->txtUrl->text();
        if (!url.startsWith("http://"))
        {
            url = "http://" + url;
            this->m_ui->txtUrl->setText(url);

        }
        this->webView->setUrl(url);
    }
}

void BrowserWindow::webView_loadFinished(bool)
{
    this->m_ui->btnGo->setText(trUtf8("&Go"));
    this->m_ui->txtUrl->setEnabled(true);
    this->lblCurrentStatus->setText(trUtf8("Complete"));
}

void BrowserWindow::webView_statusBarMessage(QString text)
{
     this->lblCurrentStatus->setText(text);
}

void BrowserWindow::webView_titleChanged(QString title)
{
    this->setWindowTitle(title);
}

void BrowserWindow::webView_loadProgress(int progress)
{

   this->lblCurrentStatus->setText(trUtf8("Progress: %1 %").arg(progress));
}

void BrowserWindow::webView_linkClicked(QUrl url)
{

    BrowserWindow *sec = new BrowserWindow();
    sec->move(x() + 40, y() + 40);
    sec->show();
    sec->setUrl(url);
}
