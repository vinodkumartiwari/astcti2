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

#include <QtWidgets>

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(const QString& serviceData, QWidget* parent) :
	QDialog(parent), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

	connect(this->ui->showLicenseButton, SIGNAL(clicked()),
			this, SLOT(showLicense()));
	connect(this->ui->aboutQtButton, SIGNAL(clicked()),
			this, SLOT(aboutQt()));
	connect(this->ui->closeButton, SIGNAL(clicked()),
			this, SLOT(close()));

	QString version = QLatin1String(APP_VERSION_LONG);

	const QString description = tr(
		"<h3>AsteriskCTI Client %1</h3>"
		"Based on Qt %2<br/>"
		"<br/>"
		"Built on " __DATE__ " at " __TIME__ "<br />"
#ifdef APP_REVISION
		"From revision %6<br/>"
#endif
		"Copyright 2008-%3 %4. All rights reserved.<br/>"
		"<br/>"
		"The program is provided AS IS with NO WARRANTY OF ANY KIND, "
		"INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A "
		"PARTICULAR PURPOSE.<br/><br/>"
		"You are a member of the following services:<br/><br/>%5")
		.arg(version, QLatin1String(QT_VERSION_STR),
			 QLatin1String(APP_YEAR), QLatin1String(APP_AUTHOR), serviceData
#ifdef APP_REVISION
			 , QString(APP_REVISION_STR).left(10)
#endif
			 );
	this->ui->infoLabel->setText(description);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::changeEvent(QEvent* e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AboutDialog::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void AboutDialog::showLicense()
{
    QDialog* dialog = new QDialog(this);

    Qt::WindowFlags flags = dialog->windowFlags();
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowContextHelpButtonHint;
    dialog->setWindowFlags(flags);

	dialog->setWindowTitle(tr("License"));
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    QTextBrowser* licenseBrowser = new QTextBrowser(dialog);
    layout->addWidget(licenseBrowser);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
	connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
    layout->addWidget(buttonBox);

    QString appPath = QCoreApplication::applicationDirPath();
	QString fileName = appPath.append("/LICENSE.txt");
    QFile file(fileName);

    QString licenseText;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		licenseText = tr("Could not read file: %1").arg(fileName);
    else
        licenseText = file.readAll();

    licenseBrowser->setPlainText(licenseText);

    dialog->setMinimumSize(QSize(640, 480));
    dialog->exec();
    delete dialog;
}
