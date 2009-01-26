#include "browserwindow.h"
#include "ui_browserwindow.h"

BrowserWindow::BrowserWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::BrowserWindowClass)
{
    m_ui->setupUi(this);
    this->addExtraWidgets();
}

BrowserWindow::~BrowserWindow()
{
    delete m_ui;
}

void BrowserWindow::addExtraWidgets()
{
    lblCurrentStatus = new QLabel(trUtf8("<b>Not Connected</b>")) ;
    this->statusBar()->addWidget(lblCurrentStatus);

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

void BrowserWindow::on_webView_loadStarted()
{
    this->m_ui->btnGo->setText(trUtf8("&Stop"));
    this->lblCurrentStatus->setText(trUtf8("Loading"));
    this->m_ui->txtUrl->setEnabled(false);



}

void BrowserWindow::on_btnGo_clicked()
{
    if (this->m_ui->btnGo->text()==trUtf8("&Stop"))
    {
        this->m_ui->webView->stop();
    }
    else
    {
        QString url = this->m_ui->txtUrl->text();
        if (!url.startsWith("http://"))
        {
            url = "http://" + url;
            this->m_ui->txtUrl->setText(url);

        }
        this->m_ui->webView->setUrl(url);
    }
}

void BrowserWindow::on_webView_loadFinished(bool)
{
    this->m_ui->btnGo->setText(trUtf8("&Go"));
    this->m_ui->txtUrl->setEnabled(true);
    this->lblCurrentStatus->setText(trUtf8("Complete"));
}

void BrowserWindow::on_webView_statusBarMessage(QString text)
{
     this->lblCurrentStatus->setText(text);
}

void BrowserWindow::on_webView_titleChanged(QString title)
{
    this->setWindowTitle(title);
}

void BrowserWindow::on_webView_loadProgress(int progress)
{

   this->lblCurrentStatus->setText(trUtf8("Progress: %1 %").arg(progress));
}
