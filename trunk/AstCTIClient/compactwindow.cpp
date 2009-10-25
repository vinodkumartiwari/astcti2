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

#include <QMouseEvent>

#include "compactwindow.h"
#include "ui_compactwindow.h"

#include "cticlientapplication.h"

CompactWindow::CompactWindow(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::CompactWindow)
{
    m_ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    this->m_ui->moveLabel->installEventFilter(this);
    this->m_ui->messageLabel->installEventFilter(this);
    this->m_ui->sizeLabel->installEventFilter(this);

    this->canClose = false;

    this->createTrayIcon();
    this->connectSlots();
}

CompactWindow::~CompactWindow()
{
    delete m_ui;
}


void CompactWindow::connectSlots()
{
    connect(m_ui->callButton, SIGNAL(clicked()), this, SLOT(placeCall()));
    connect(m_ui->aboutButton, SIGNAL(clicked()), this, SLOT(about()));
    connect(m_ui->pauseButton, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));
    connect(m_ui->minimizeButton, SIGNAL(clicked()), this, SLOT(minimizeToTray()));
    connect(m_ui->quitButton, SIGNAL(clicked(bool)), this, SLOT(quit(bool)));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void CompactWindow::createTrayIcon()
{
     QIcon icon;
     icon.addPixmap(QPixmap(QString::fromUtf8(":/res/res/asteriskcti16x16.png")), QIcon::Normal, QIcon::Off);

     trayIcon = new QSystemTrayIcon(icon, this);
     trayIcon->setToolTip("AsteriskCTI client");
     trayIcon->show();
}

void CompactWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        this->isVisible() ? this->hide() : this->show();
        break;
    default:
        ;
    }
}

void CompactWindow::showMessage(const QString &message, QSystemTrayIcon::MessageIcon severity)
{
    this->m_ui->messageLabel->setText(message);
    switch (severity) {
    case QSystemTrayIcon::Critical:
        this->m_ui->messageLabel->setProperty("severity", "Critical");
        break;
    case QSystemTrayIcon::Warning:
        this->m_ui->messageLabel->setProperty("severity", "Warning");
        break;
    default:
        this->m_ui->messageLabel->setProperty("severity", "Information");
        break;
    }

    if (!this->isVisible())
        trayIcon->showMessage("AsteriskCTI", message, severity, 5000);
}

void CompactWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        event->accept();
    }
}

void CompactWindow::closeEvent(QCloseEvent *event)
{
    if (this->canClose)
        event->accept();
    else
        event->ignore();
}

void CompactWindow::changeEvent(QEvent *e)
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

bool CompactWindow::eventFilter(QObject *object, QEvent *event)
{
    QEvent::Type type = event->type();

    if (type != QEvent::MouseButtonPress && type != QEvent::MouseButtonRelease && type != QEvent::MouseMove)
        return false;

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);

    if (!mouseEvent || mouseEvent->modifiers() != Qt::NoModifier)
        return false;

    Qt::MouseButton button = mouseEvent->button();

    bool accepted = false;

    if (type == QEvent::MouseButtonPress && button == Qt::LeftButton) {
        if (object == this->m_ui->sizeLabel) {
            this->offset = mouseEvent->globalPos() - (this->pos() + QPoint(this->width(), 0));
        } else {
            this->offset = mouseEvent->globalPos() - this->pos();
        }
        event->accept();
        accepted = true;
    }

    if (type == QEvent::MouseButtonRelease) {
        this->offset = QPoint(0, 0);
    }

    if (type == QEvent::MouseMove && this->offset != QPoint(0, 0)) {
        if (object == this->m_ui->sizeLabel) {
            int newWidth = mouseEvent->globalPos().x() - this->offset.x() - this->pos().x();
            if (newWidth >= this->minimumWidth()) {
                this->resize(newWidth, this->height());
            }
        } else {
            this->move(mouseEvent->globalPos() - this->offset);
        }
        accepted = true;
    }

    return accepted;
}

void CompactWindow::placeCall()
{
    //TODO
}

void CompactWindow::about()
{
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void CompactWindow::minimizeToTray()
{
    this->hide();
}

void CompactWindow::pause(bool paused)
{
    //TODO
}

void CompactWindow::quit(bool skipCheck)
{
    if (!skipCheck) {
        QMessageBox msgBox(this);

        msgBox.setText( trUtf8("Confirm application close"));
        msgBox.setInformativeText( trUtf8("Are you sure you want to exit?"));
        QPushButton *yesBtn = msgBox.addButton(trUtf8("&Yes"),QMessageBox::YesRole);
        yesBtn->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/ok.png"))));
        QPushButton *noBtn = msgBox.addButton(trUtf8("&No"),QMessageBox::NoRole);
        noBtn->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/cancel.png"))));

        msgBox.setDefaultButton(noBtn);
        msgBox.setIcon(QMessageBox::Question);

        msgBox.exec();
        if (msgBox.clickedButton() == yesBtn) {
            this->canClose = true;
        }
    } else {
        this->canClose = true;
    }

    if (this->canClose) {
        emit this->logOff();

        this->close();
    }
}
