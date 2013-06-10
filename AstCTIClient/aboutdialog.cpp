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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

    this->setUpInfoLabel();
}

AboutDialog::~AboutDialog()
{
    delete ui;
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
		.arg(version, QLatin1String(QT_VERSION_STR),
			 QLatin1String(APP_YEAR), (QLatin1String(APP_AUTHOR))
#ifdef APP_REVISION
             , QString(APP_REVISION_STR).left(10)
#endif
             );
    this->ui->lblCopyright->setText(description);

    this->ui->lblCopyright->setWordWrap(true);
    this->ui->lblCopyright->setOpenExternalLinks(true);
    this->ui->lblCopyright->setTextInteractionFlags(Qt::TextBrowserInteraction);
}

void AboutDialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
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

void AboutDialog::on_btnShowLicense_clicked()
{
    QDialog *dialog = new QDialog(this);

    Qt::WindowFlags flags = dialog->windowFlags();
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowContextHelpButtonHint;
    dialog->setWindowFlags(flags);

    dialog->setWindowTitle(trUtf8("License"));
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
        licenseText = trUtf8("Could not read file: ") + fileName;
    else
        licenseText = file.readAll();

    licenseBrowser->setPlainText(licenseText);

    dialog->setMinimumSize(QSize(640, 480));
    dialog->exec();
    delete dialog;
}
