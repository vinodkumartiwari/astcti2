/* Copyright (C) 2007-2010 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * Copyright (C) 2007-2010 Lumiss d.o.o.
 * http://www.lumiss.hr
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
 * AstCtiClient.  If you copy code from other releases into a copy of GNU
 * AstCtiClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCtiClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include "webview.h"
#include "browserwindow.h"
#include "ui_browserwindow.h"

BrowserWindow::BrowserWindow(const QString& userName, QUrl url, QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::BrowserWindow)
{
    ui->setupUi(this);

    webView = new WebView(this->ui->centralwidget);
	webView->setObjectName(QStringLiteral("webView"));
	webView->setUrl(QUrl(QStringLiteral("about:blank")));
    this->ui->verticalLayout->addWidget(webView);

	this->statusLabel = new QLabel(tr("<b>Not Connected</b>")) ;
    this->statusBar()->addWidget(this->statusLabel);

    this->currentHistoryItem = -1;
    historySetEnabled();

	connect(this->ui->prevButton, SIGNAL(clicked()),
			this, SLOT(prevButton_clicked()));
	connect(this->ui->nextButton, SIGNAL(clicked()),
			this, SLOT(nextButton_clicked()));
	connect(this->ui->reloadButton, SIGNAL(clicked()),
			this, SLOT(reloadButton_clicked()));
	connect(this->ui->stopButton, SIGNAL(clicked()),
			this, SLOT(stopButton_clicked()));
	connect(this->ui->goButton, SIGNAL(clicked()),
			this, SLOT(goButton_clicked()));

	connect(webView, SIGNAL(loadStarted()),
			this, SLOT(webView_loadStarted()));
	connect(webView, SIGNAL(loadProgress(int)),
			this, SLOT(webView_loadProgress(int)));
	connect(webView, SIGNAL(loadFinished(bool)),
			this, SLOT(webView_loadFinished(bool)));
	connect(webView, SIGNAL(titleChanged(QString)),
			this, SLOT(webView_titleChanged(QString)));
	connect(webView, SIGNAL(linkClicked(QUrl)),
			this, SLOT(webView_linkClicked(QUrl)));

    this->userName = userName;

    setUrl(url);
}

BrowserWindow::~BrowserWindow()
{
    delete ui;
}

void BrowserWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Return) {
        if (this->focusWidget() == this->ui->urlLineEdit) {
            goButton_clicked();
            e->accept();
        }
    }
}

void BrowserWindow::changeEvent(QEvent* e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void BrowserWindow::closeEvent(QCloseEvent* e)
{
    Q_UNUSED(e)

    writeSettings();
    emit windowClosing(this);
}

WebView* BrowserWindow::currentView() const
{
    return this->webView;
}

void BrowserWindow::setUrl(QUrl url)
{
    this->currentUrl = url;
    //Settings are stored per-site (and per-user)
    readSettings();

    this->webView->setUrl(url);
    ++this->currentHistoryItem;
    for (int i = this->history.size(); i > this->currentHistoryItem; i--) {
        this->history.removeLast();
    }
    this->history.append(url.toString());
    historySetEnabled();
}

void BrowserWindow::webView_loadStarted()
{
    QUrl url = this->webView->url();
    this->ui->urlLineEdit->setText(url.toString());
    this->ui->reloadButton->setEnabled(false);
    this->ui->stopButton->setEnabled(true);
	this->statusLabel->setText(tr("Loading..."));
}

void BrowserWindow::goButton_clicked()
{
    QString url = this->ui->urlLineEdit->text();
	if (!url.startsWith(QStringLiteral("http://")) && !url.startsWith(QStringLiteral("https://"))) {
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
    --this->currentHistoryItem;
    this->webView->setUrl(this->history[this->currentHistoryItem]);
    historySetEnabled();
}

void BrowserWindow::nextButton_clicked()
{
    ++this->currentHistoryItem;
    this->webView->setUrl(this->history[this->currentHistoryItem]);
    historySetEnabled();
}

void BrowserWindow::webView_loadFinished(bool)
{
    this->ui->reloadButton->setEnabled(true);
    this->ui->stopButton->setEnabled(false);
	this->statusLabel->setText(tr("Complete"));
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
   this->statusLabel->setText(tr("Progress: %1 %").arg(progress));
}

void BrowserWindow::webView_linkClicked(QUrl url)
{
    emit linkClicked(url);
}

void BrowserWindow::historySetEnabled(){
    this->ui->prevButton->setEnabled(this->currentHistoryItem > 0);
    this->ui->nextButton->setEnabled(this->currentHistoryItem < this->history.size() - 1);
}

void BrowserWindow::writeSettings()
{
    QSettings settings(APP_NAME);

    settings.beginGroup("BrowserWindow." + this->userName + "." + this->currentUrl.host());
	settings.setValue(QStringLiteral("geometry"), this->saveGeometry());
	settings.setValue(QStringLiteral("windowState"), this->saveState());
    settings.endGroup();
 }

void BrowserWindow::readSettings()
{
    QSettings settings(APP_NAME);

    settings.beginGroup("BrowserWindow." + this->userName + "." + this->currentUrl.host());
    //TODO
	//On X11, restoreGeometry cannot account for non-client area because window is not shown yet
	//The window will shift toward bottom right corner depending on the size of window decoration
    //Currently (Qt 4.6), there doesn't appear to be a workaround for this
    //A possible workaround would be to create borderless window and implement moving and sizing
	this->restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
	this->restoreState(settings.value(QStringLiteral("windowState")).toByteArray());
    settings.endGroup();
 }
