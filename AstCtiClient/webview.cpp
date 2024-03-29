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

#include <QClipboard>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStyle>
#include <QtNetwork/QNetworkRequest>
#include <QtWebKitWidgets/QWebHitTestResult>
#include <QtNetwork/QNetworkReply>
#include <QtUiTools/QUiLoader>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QBuffer>

/******************************************************************************
** WebPage
*/

WebPage::WebPage(QObject* parent)
    : QWebPage(parent)
{
//    connect(this, SIGNAL(unsupportedContent(QNetworkReply* )),
//            this, SLOT(handleUnsupportedContent(QNetworkReply* )));
}

bool WebPage::acceptNavigationRequest(QWebFrame* const frame, const QNetworkRequest &request,
									  const NavigationType type)
{
    if (frame == mainFrame()) {
        m_loadingUrl = request.url();
        emit loadingUrl(m_loadingUrl);
    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
}
//
//QWebPage* WebPage::createWindow(QWebPage::WebWindowType type)
//{
//    Q_UNUSED(type);
//    /*switch(type)
//    {
//        case QWebPage::WebBrowserWindow:
//
//            break;
//        case QWebPage::WebModalDialog:
//            break;
//    }*/
//    BrowserWindow* w = new BrowserWindow( qobject_cast<QWidget*>(this->parent()) );
//    w->show();
//    return w->currentView()->webPage();
//}

#if !defined(QT_NO_UITOOLS)
QObject* WebPage::createPlugin(const QString& classId, const QUrl& url,
							   const QStringList& paramNames, const QStringList& paramValues)
{
    Q_UNUSED(url);
    Q_UNUSED(paramNames);
    Q_UNUSED(paramValues);
    QUiLoader loader;
    return loader.createWidget(classId, view());
}
#endif // !defined(QT_NO_UITOOLS)

void WebPage::handleUnsupportedContent(QNetworkReply* reply)
{
	QFile file(QStringLiteral(":/res/notfound.html"));
    bool isOpened = file.open(QIODevice::ReadOnly);
    Q_ASSERT(isOpened);
	QString title = tr("Error loading page: %1").arg(reply->url().toString());
    QString html = QString(QLatin1String(file.readAll()))
                        .arg(title)
                        .arg(reply->errorString())
                        .arg(reply->url().toString());

    QBuffer imageBuffer;
    imageBuffer.open(QBuffer::ReadWrite);
    QIcon icon = view()->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, view());
    QPixmap pixmap = icon.pixmap(QSize(32,32));
	if (pixmap.save(&imageBuffer, "PNG")) {
		html.replace(QStringLiteral("IMAGE_BINARY_DATA_HERE"),
                     QString(QLatin1String(imageBuffer.buffer().toBase64())));
    }

    QList<QWebFrame*> frames;
    frames.append(mainFrame());
    while (!frames.isEmpty()) {
        QWebFrame* frame = frames.takeFirst();
        if (frame->url() == reply->url()) {
            frame->setHtml(html, reply->url());
            return;
        }
        QList<QWebFrame* > children = frame->childFrames();
        foreach(QWebFrame* frame, children)
            frames.append(frame);
    }
    if (m_loadingUrl == reply->url()) {
        mainFrame()->setHtml(html, reply->url());
    }
}

/******************************************************************************
** WebView
*/

WebView::WebView(QWidget* parent)
	: QWebView(parent), m_progress(0), m_page(new WebPage(this))
{
	connect(m_page, SIGNAL(statusBarMessage(const QString&)),
			this, SLOT(setStatusBarText(const QString&)));
	connect(this, SIGNAL(loadProgress(int)),
			this, SLOT(setProgress(int)));
	connect(this, SIGNAL(loadFinished(bool)),
			this, SLOT(loadFinished()));
	connect(m_page, SIGNAL(loadingUrl(const QUrl&)),
			this, SIGNAL(urlChanged(const QUrl &)));
//	connect(m_page, SIGNAL(downloadRequested(const QNetworkRequest &)),
//			this, SLOT(downloadRequested(const QNetworkRequest &)));
	connect(m_page, SIGNAL(unsupportedContent(QNetworkReply* )),
			m_page, SLOT(handleUnsupportedContent(QNetworkReply* )));
	m_page->setForwardUnsupportedContent(true);
	m_page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

	this->setPage(m_page);
}

WebPage* const WebView::webPage() const
{
	return this->m_page;
}

void WebView::setProgress(const int progress)
{
    m_progress = progress;
}

void WebView::loadFinished()
{
	if (m_progress != 100)
		qWarning() << "Recieved finished signal while progress is still:"
				   << progress() << "Url:" << url();
    m_progress = 0;
}

void WebView::loadUrl(const QUrl &url)
{
    m_initialUrl = url;
	this->load(url);
}

const QString& WebView::lastStatusBarText() const
{
    return m_statusBarText;
}

const int WebView::progress() const
{
	return this->m_progress;
}

const QUrl WebView::url() const
{
    QUrl url = QWebView::url();
	if (url.isEmpty())
		url = m_initialUrl;

	return url;
}

void WebView::setStatusBarText(const QString& message)
{
    m_statusBarText = message;
}
