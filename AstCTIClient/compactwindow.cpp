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

#include <QMouseEvent>
#include <QMessageBox>
#include <QSettings>

#include "compactwindow.h"
#include "ui_compactwindow.h"
#include "managerwindow.h"

CompactWindow::CompactWindow(const QString &userName) :
    QWidget(),
    ui(new Ui::CompactWindow)
{
    ui->setupUi(this);

    this->setWindowFlags((this->windowFlags() | Qt::Window
                                              | Qt::FramelessWindowHint
                                              | Qt::WindowStaysOnTopHint
                                              | Qt::CustomizeWindowHint)
                                              & ~Qt::WindowSystemMenuHint
                                              & ~Qt::WindowTitleHint
                                              & ~Qt::WindowMinMaxButtonsHint
                                              & ~Qt::WindowCloseButtonHint);

    this->ui->moveLabel->installEventFilter(this);
    this->ui->statusLabel->installEventFilter(this);
    this->ui->sizeLabel->installEventFilter(this);

    this->canClose = false;

    this->createTrayIcon();
    this->connectSlots();

    this->userName = userName;

    this->dragOrigin = QPoint(-1, -1);

    readSettings();
}

CompactWindow::~CompactWindow()
{
    delete ui;
}

void CompactWindow::connectSlots()
{
    connect(ui->callButton, SIGNAL(clicked()), this, SLOT(placeCall()));
    connect(ui->aboutButton, SIGNAL(clicked()), this, SLOT(about()));
    connect(ui->pauseButton, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));
    connect(ui->minimizeButton, SIGNAL(clicked()), this, SLOT(minimizeToTray()));
    connect(ui->passwordButton, SIGNAL(clicked()), this, SIGNAL(changePassword()));
    connect(ui->quitButton, SIGNAL(clicked(bool)), this, SLOT(quit(bool)));
    connect(this->trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void CompactWindow::createTrayIcon()
{
     QIcon icon;
     icon.addPixmap(QPixmap(QString::fromUtf8(":/res/res/asteriskcti16x16.png")), QIcon::Normal, QIcon::Off);

     this->trayIcon = new QSystemTrayIcon(icon, this);
     this->trayIcon->setToolTip(QString(APP_NAME) + " client " + QString(APP_VERSION_LONG));
     this->trayIcon->show();
}

void CompactWindow::enableControls(bool enable)
{
    ui->callComboBox->setEditable(enable);
    ui->callButton->setEnabled(enable);
    ui->passwordButton->setEnabled(enable);
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

void CompactWindow::setStatus(bool status)
{
    if (status) {
        enableControls(true);
        this->ui->statusLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/res/greenled.png")));
        this->ui->statusLabel->setToolTip(statusMessageOK);
        //Hide the message
        if (this->trayIcon->supportsMessages())
            this->trayIcon->showMessage("", "", QSystemTrayIcon::NoIcon, 1);
    } else {
        enableControls(false);
        this->ui->statusLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/res/redled.png")));
        this->ui->statusLabel->setToolTip(statusMessageNotOK);
        showMessage(statusMessageNotOK, QSystemTrayIcon::Critical);
    }
}

void CompactWindow::showMessage(const QString &message, QSystemTrayIcon::MessageIcon severity)
{
    if (this->trayIcon->supportsMessages()) {
        this->trayIcon->showMessage(APP_NAME, message, severity, 50000);
    } else {
        switch (severity) {
        case QSystemTrayIcon::NoIcon:
        case QSystemTrayIcon::Information:
            QMessageBox::information(this, APP_NAME, message);
            break;
        case QSystemTrayIcon::Warning:
            QMessageBox::warning(this, APP_NAME, message);
            break;
        case QSystemTrayIcon::Critical:
            QMessageBox::critical(this, APP_NAME, message);
            break;
        }
    }
}

void CompactWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        e->accept();
    }
}

void CompactWindow::closeEvent(QCloseEvent *e)
{
    if (this->canClose) {
        writeSettings();
        e->accept();
    } else {
        e->ignore();
    }
}

void CompactWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool CompactWindow::eventFilter(QObject *object, QEvent *e)
{
    bool accepted = false;

    QEvent::Type type = e->type();

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(e);

    if (isValidDrag(mouseEvent)) {
        this->dragOrigin = mouseEvent->globalPos();
        e->accept();
        accepted = true;
    } else if (type == QEvent::MouseButtonRelease) {
        this->dragOrigin = QPoint(-1, -1);
        e->accept();
        accepted = true;
    } else if (type == QEvent::MouseMove) {
        if (this->dragOrigin != QPoint(-1, -1)) {
            if (object == this->ui->sizeLabel) {
                int newWidth = this->width() + (mouseEvent->globalX() - this->dragOrigin.x());
                if (newWidth >= this->minimumWidth())
                    this->resize(newWidth, this->height());
            } else {
                this->move(this->pos() + (mouseEvent->globalPos() - this->dragOrigin));
            }
            this->dragOrigin = mouseEvent->globalPos();
            e->accept();
            accepted = true;
        }
    }

    return accepted;
}

bool CompactWindow::isValidDrag(QMouseEvent *mouseEvent) const
{
    if (!mouseEvent || mouseEvent->modifiers() != Qt::NoModifier)
        return false;

    if (mouseEvent->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::LeftButton)
        return true;
    else
        return false;
}

void CompactWindow::placeCall()
{
    //TODO
    ManagerWindow *win = new ManagerWindow(this);
    win->show();
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

void CompactWindow::pauseError(const QString &message)
{
   this->ui->pauseButton->setEnabled(true);
   //TODO: decide what to do with pause Errors
   //a pause error can occur also if we, administratively using CLI,
   //remove an agent from queue...
   this->showMessage(pauseErrorMessage + "\n\n" + message, QSystemTrayIcon::Warning);
}

void CompactWindow::pauseAccepted() {
    bool paused = !this->ui->pauseButton->isChecked();

    this->ui->callComboBox->setEnabled(!paused);
    this->ui->callButton->setEnabled(!paused);
    this->ui->passwordButton->setEnabled(!paused);
    this->ui->repeatButton->setEnabled(!paused);
    this->ui->pauseButton->setChecked(paused);
    this->ui->pauseButton->setEnabled(true);
}

void CompactWindow::pause(bool paused)
{
    this->ui->pauseButton->setEnabled(false);
    emit this->pauseRequest(paused);
}

void CompactWindow::quit(bool skipCheck)
{
    if (!skipCheck) {
        QMessageBox msgBox(this);

        msgBox.setText(trUtf8("Are you sure you want to quit?"));
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
        emit logOff();

        this->close();
    }
}

void CompactWindow::writeSettings()
{
    QSettings settings(APP_NAME);

    settings.beginGroup("CompactWindow." + this->userName);
    settings.setValue("geometry", this->saveGeometry());
    settings.endGroup();
 }

void CompactWindow::readSettings()
{
    QSettings settings(APP_NAME);

    settings.beginGroup("CompactWindow." + this->userName);
    this->restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
 }
