#include "browserdialog.h"
#include "ui_browserdialog.h"

BrowserDialog::BrowserDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::BrowserDialog)
{
    m_ui->setupUi(this);
}

BrowserDialog::~BrowserDialog()
{
    delete m_ui;
}

void BrowserDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void BrowserDialog::on_btnGo_clicked()
{
    this->m_ui->webView->setUrl(QUrl(this->m_ui->txtUrl->text()));
}
