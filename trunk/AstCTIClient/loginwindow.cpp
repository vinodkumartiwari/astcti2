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

#include <QAbstractButton>

#include "loginwindow.h"
#include "compactwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::LoginWindow)
{
    m_ui->setupUi(this);

    m_ui->dialogButtonBox->buttons().at(0)->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/ok.png"))));
    m_ui->dialogButtonBox->buttons().at(1)->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/cancel.png"))));

    connect(this->m_ui->dialogButtonBox, SIGNAL(accepted()), this, SLOT(accepting()));
    connect(this->m_ui->dialogButtonBox, SIGNAL(rejected()), this, SIGNAL(rejected()));
}

LoginWindow::~LoginWindow()
{
    delete m_ui;
}

void LoginWindow::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void LoginWindow::showMessage(const QString message, bool connectionLost)
{
    this->m_ui->dialogButtonBox->buttons().at(0)->setEnabled(!connectionLost);
    this->m_ui->messageLabel->setText(message);
}

void LoginWindow::accepting()
{
    if (this->m_ui->usernameLineEdit->text().isEmpty()) {
        this->showMessage("Username is required to continue. Please enter your username.", false);
        this->m_ui->usernameLineEdit->setFocus();
    } else {
        this->hide();
        emit this->accepted(this->m_ui->usernameLineEdit->text(), this->m_ui->passwordLineEdit->text());
    }
}
