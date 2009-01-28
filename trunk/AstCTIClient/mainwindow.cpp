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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cticlientapplication.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent,Qt::Dialog) , ui(new Ui::MainWindowClass)
{
    createActions();
    createTrayIcon();


    // Append links to the link label
    links.append("<a href=\"http://code.google.com/p/astcti2\">Asterisk CTI</a>");
    links.append("<a href=\"http://www.advert.it\">Advert SRL</a>");
    links.append("<a href=\"http://centralino-voip.brunosalzano.com\">Centralino Voip</a>");

    labelPos = 0;
    labelTime = LINKS_TIMEOUT;
    ui->setupUi(this);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);

    this->setDesktopPosition();
    this->addExtraWidgets();
    this->enableBannerTimer();
    this->connectSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::enableBannerTimer()
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(UpdateTimer()));
    timer->start(1000);
}

void MainWindow::addExtraWidgets()
{
    statusImage = new QIcon(QPixmap(QString::fromUtf8(":/res/res/redled.png")));

    lblCurrentStatus = new QLabel(trUtf8("<b>Not Connected</b>")) ;
    this->statusBar()->addWidget(lblCurrentStatus);

    lblCurrentTime = new QLabel(QString("00:00:00")) ;
    lblCurrentTime->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
    lblCurrentTime->setAlignment(Qt::AlignCenter);
    this->statusBar()->addWidget(lblCurrentTime);

    linkLabel = new QLabel(links.at(0)) ;
    linkLabel->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);
    linkLabel->setAlignment(Qt::AlignRight);
    linkLabel->setOpenExternalLinks(true);

    linkLabel->setCursor(Qt::PointingHandCursor);
    this->statusBar()->addWidget(linkLabel);
}

void MainWindow::connectSlots()
{
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionExit_2, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), this, SLOT(configure()));
    connect(ui->actionConfigure_2, SIGNAL(triggered()), this, SLOT(configure()));
    connect(ui->actionLogin, SIGNAL(triggered()), this, SLOT(login()));
    connect(ui->actionPause, SIGNAL(triggered()), this, SLOT(pause()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAbout_2, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::setDesktopPosition()
{
    QDesktopWidget *desktop = QApplication::desktop();
    QRect screenRect = desktop->screenGeometry();
    QPoint location = QPoint(( (screenRect.width() - this->width()) / 2), ((screenRect.height() - this->height()) / 2));
    this->move(location);
}

void MainWindow::createActions()
 {
     minimizeAction = new QAction(trUtf8("Mi&nimize"), this);
     connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

     restoreAction = new QAction(trUtf8("&Restore"), this);
     connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

     quitAction = new QAction(trUtf8("&Quit"), this);
     connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
 }

 void MainWindow::createTrayIcon()
 {
     QIcon icon;
     icon.addPixmap(QPixmap(QString::fromUtf8(":/res/res/asteriskcti16x16.png")), QIcon::Normal, QIcon::Off);
     trayIconMenu = new QMenu(this);
     trayIconMenu->addAction(minimizeAction);
     trayIconMenu->addAction(restoreAction);
     trayIconMenu->addSeparator();
     trayIconMenu->addAction(quitAction);

     trayIcon = new QSystemTrayIcon(icon, this);
     trayIcon->setContextMenu(trayIconMenu);
     trayIcon->show();
 }

 void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
 {
     switch (reason)
     {
         case QSystemTrayIcon::Trigger:
            this->isVisible() ? this->hide() :  this->show();
            break;

         case QSystemTrayIcon::DoubleClick:
             break;
         case QSystemTrayIcon::MiddleClick:
             showMessage();
             break;
         default:
            ;
     }
 }


 void MainWindow::showMessage()
 {
     trayIcon->showMessage("Test", "this is a test", QSystemTrayIcon::Information , 3000);
 }


 void MainWindow::messageClicked()
 {
     QMessageBox::information(0, trUtf8("Systray"), trUtf8("Test"));
 }


 void MainWindow::closeEvent(QCloseEvent *event)
 {
     QMessageBox msgBox(this);
     
     msgBox.setText( trUtf8("Confirm application close."));
     msgBox.setInformativeText( trUtf8("Sure you want to exit?"));
     QPushButton *yesBtn = msgBox.addButton( trUtf8("&Yes"),QMessageBox::YesRole);
     yesBtn->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/ok.png"))));
     QPushButton *noBtn = msgBox.addButton( trUtf8("&No"),QMessageBox::NoRole);
     noBtn->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/cancel.png"))));


     msgBox.setDefaultButton( noBtn );
     msgBox.setIcon(QMessageBox::Question);

     msgBox.exec();
     if (msgBox.clickedButton() == yesBtn)
     {
             event->accept();
             return;
     }
     event->ignore();

 }


void MainWindow::UpdateTimer()
{
    QTime qtime = QTime::currentTime();
    QString stime = qtime.toString(Qt::TextDate);

    if (!stime.isNull())
        this->lblCurrentTime->setText(stime);

    labelTime--;
    if (labelTime==0)
    {
        QString link = links.at(labelPos);
        linkLabel->setText(link);
        labelPos++;
        if (labelPos == links.size()) labelPos = 0;
        labelTime = LINKS_TIMEOUT;
    }
}

void MainWindow::about()
{
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, trUtf8("About Qt Version"));
}


void MainWindow::configure()
{
    CtiClientApplication::instance()->newBrowserWindow();
}

void MainWindow::login()
{
    // does actually nothing

}

void MainWindow::pause()
{
    // does actually nothing

}
