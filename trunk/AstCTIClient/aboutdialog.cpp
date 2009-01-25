#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::AboutDialog)
{
    m_ui->setupUi(this);
    this->setUpInfoLabel();

}

AboutDialog::~AboutDialog()
{
    delete m_ui;
}

void AboutDialog::setUpInfoLabel()
{
    QString version = QLatin1String(APP_VERSION_LONG);
    version += QDate(2007, 25, 10).toString(Qt::SystemLocaleDate);

    const QString description = trUtf8(
        "<h3>AsteriskCTI Client %1</h3>"
        "Based on Qt %2<br/>"
        "<br/>"
        "Built on " __DATE__ " at " __TIME__ "<br />"
#ifdef APP_REVISION
        "From revision %5<br/>"
#endif
        "Copyright 2008-%3 %4. All rights reserved.<br/>"
        "<br/>"
        "The program is provided AS IS with NO WARRANTY OF ANY KIND, "
        "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A "
        "PARTICULAR PURPOSE.<br/>")
        .arg(version, QLatin1String(QT_VERSION_STR), QLatin1String(APP_YEAR), (QLatin1String(APP_AUTHOR))
#ifdef APP_REVISION
             , QString(APP_REVISION_STR).left(10)
#endif
             );
    this->m_ui->lblCopyright->setText(description);

    this->m_ui->lblCopyright->setWordWrap(true);
    this->m_ui->lblCopyright->setOpenExternalLinks(true);
    this->m_ui->lblCopyright->setTextInteractionFlags(Qt::TextBrowserInteraction);


}

void AboutDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AboutDialog::on_btnAboutQt_clicked()
{
    QMessageBox::aboutQt(this);
}

void AboutDialog::on_btnClose_clicked()
{
    this->close();
}

void AboutDialog::on_btnShowLicence_clicked()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("License");
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    QTextBrowser *licenseBrowser = new QTextBrowser(dialog);
    layout->addWidget(licenseBrowser);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox , SIGNAL(rejected()), dialog, SLOT(reject()));
    layout->addWidget(buttonBox);

    QString appPath = QCoreApplication::applicationDirPath();
    QString fileName = appPath + "/license.txt";
    QFile file(fileName);

    QString licenseText;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        licenseText = "File '" + fileName + "' could not be read.";
    else
        licenseText = file.readAll();

    licenseBrowser->setPlainText(licenseText);

    dialog->setMinimumSize(QSize(550, 690));
    dialog->exec();
    delete dialog;
}
