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

BrowserWindow::BrowserWindow(const QString &userName, QUrl url, QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::BrowserWindow)
{
    m_ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);

    webView = new WebView(this->m_ui->centralwidget);
    webView->setObjectName(QString::fromUtf8("webView"));
    webView->setUrl(QUrl("about:blank"));
    this->m_ui->verticalLayout->addWidget(webView);

    this->statusLabel = new QLabel(trUtf8("<b>Not Connected</b>")) ;
    this->statusBar()->addWidget(this->statusLabel);

    this->currentHistoryItem = -1;
    historySetEnabled();

    connect(this->m_ui->prevButton, SIGNAL(clicked()), this, SLOT(prevButton_clicked()));
    connect(this->m_ui->nextButton, SIGNAL(clicked()), this, SLOT(nextButton_clicked()));
    connect(this->m_ui->reloadButton, SIGNAL(clicked()), this, SLOT(reloadButton_clicked()));
    connect(this->m_ui->stopButton, SIGNAL(clicked()), this, SLOT(stopButton_clicked()));
    connect(this->m_ui->goButton, SIGNAL(clicked()), this, SLOT(goButton_clicked()));

    connect(webView, SIGNAL(loadStarted()), this, SLOT(webView_loadStarted()));
    connect(webView, SIGNAL(loadProgress(int)), this, SLOT(webView_loadProgress(int)));
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(webView_loadFinished(bool)));
    connect(webView, SIGNAL(titleChanged(QString)), this, SLOT(webView_titleChanged(QString)));
    connect(webView, SIGNAL(linkClicked(QUrl)), this, SLOT(webView_linkClicked(QUrl)));

    this->userName = userName;

    setUrl(url);
}

BrowserWindow::~BrowserWindow()
{
    delete m_ui;
}

void BrowserWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return) {
        if (this->focusWidget() == this->m_ui->urlLineEdit) {
            goButton_clicked();
            e->accept();
        }
    }
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

void BrowserWindow::closeEvent(QCloseEvent *e)
{
    writeSettings();
    emit windowClosing(this);
}

WebView *BrowserWindow::currentView() const
{
    return this->webView;
}

void BrowserWindow::setUrl(QUrl url)
{
    this->currentUrl = url;
    //Settings are stored per-site (and per-user)
    readSettings();

    this->webView->setUrl(url);
    this->currentHistoryItem++;
    for (int i = this->history.size(); i > this->currentHistoryItem; i--) {
        this->history.removeLast();
    }
    this->history.append(url.toString());
    historySetEnabled();
}

void BrowserWindow::webView_loadStarted()
{
    QUrl url = this->webView->url();
    this->m_ui->urlLineEdit->setText(url.toString());
    this->m_ui->reloadButton->setEnabled(false);
    this->m_ui->stopButton->setEnabled(true);
    this->statusLabel->setText(trUtf8("Loading..."));
}

void BrowserWindow::goButton_clicked()
{
    QString url = this->m_ui->urlLineEdit->text();
    if (!url.startsWith("http://") && !url.startsWith("https://"))  {
        url = "http://" + url;
    }
    setUrl(QUrl(url));
}

void BrowserWindow::stopButton_clicked()
{
    this->webView->stop();
}

void BrowserWindow::reloadButton_clicked()
{
    this->webView->reload();
}

void BrowserWindow::prevButton_clicked()
{
    this->currentHistoryItem--;
    this->webView->setUrl(this->history[this->currentHistoryItem]);
    historySetEnabled();
}

void BrowserWindow::nextButton_clicked()
{
    this->currentHistoryItem++;
    this->webView->setUrl(this->history[this->currentHistoryItem]);
    historySetEnabled();
}

void BrowserWindow::webView_loadFinished(bool)
{
    this->m_ui->reloadButton->setEnabled(true);
    this->m_ui->stopButton->setEnabled(false);
    this->statusLabel->setText(trUtf8("Complete"));
}

void BrowserWindow::webView_statusBarMessage(QString text)
{
     this->statusLabel->setText(text);
}

void BrowserWindow::webView_titleChanged(QString title)
{
    this->setWindowTitle(title);
}

void BrowserWindow::webView_loadProgress(int progress)
{
   this->statusLabel->setText(trUtf8("Progress: %1 %").arg(progress));
}

void BrowserWindow::webView_linkClicked(QUrl url)
{
    emit linkClicked(url);
}

void BrowserWindow::historySetEnabled(){
    this->m_ui->prevButton->setEnabled(this->currentHistoryItem > 0);
    this->m_ui->nextButton->setEnabled(this->currentHistoryItem < this->history.size() - 1);
}

void BrowserWindow::writeSettings()
{
    QSettings settings(appName);

    settings.beginGroup("BrowserWindow." + this->userName + "." + this->currentUrl.host());
    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("windowState", this->saveState());
    settings.endGroup();
 }

void BrowserWindow::readSettings()
{
    QSettings settings(appName);

    settings.beginGroup("BrowserWindow." + this->userName + "." + this->currentUrl.host());
    //TODO
    //On X11, restoreGeometry cannot account for non-client area because the window is not shown yet
    //So, the window will shift toward bottom right corner depending on the size of window decoration
    //Currently (Qt 4.6), there doesn't appear to be a workaround for this
    this->restoreGeometry(settings.value("geometry").toByteArray());
    this->restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();
 }
