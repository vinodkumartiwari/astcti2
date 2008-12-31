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

static int LINKS_TIMEOUT = 30;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent,Qt::Dialog) , ui(new Ui::MainWindowClass)
{

    // Append links to the link label
    links.append("<a href=\"http://code.google.com/p/astcti2\">Asterisk CTI</a>");
    links.append("<a href=\"http://www.advert.it\">Advert SRL</a>");
    links.append("<a href=\"http://centralino-voip.brunosalzano.com\">Centralino Voip</a>");

    labelPos = 0;
    labelTime = LINKS_TIMEOUT;
    ui->setupUi(this);

    statusImage = new QIcon(QPixmap(QString::fromUtf8(":/res/res/redled.png")));
    //this->statusBar()->addWidget(statusImage);

    lblCurrentStatus = new QLabel(trUtf8("<b>Not Connected</b>")) ;
    this->statusBar()->addWidget(lblCurrentStatus);


    lblCurrentTime = new QLabel(QString("00:00:00")) ;
    this->statusBar()->addWidget(lblCurrentTime);

    linkLabel = new QLabel(links.at(0)) ;
    linkLabel->setOpenExternalLinks(true);

    linkLabel->setCursor(Qt::PointingHandCursor);
    this->statusBar()->addWidget(linkLabel);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(UpdateTimer()));
    timer->start(1000);


    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), this, SLOT(configure()));
    connect(ui->actionLogin, SIGNAL(triggered()), this, SLOT(login()));
    connect(ui->actionPause, SIGNAL(triggered()), this, SLOT(pause()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::closeEvent(QCloseEvent *event)
 {
     QMessageBox msgBox;
     msgBox.setText( trUtf8("Confirm application close."));
     msgBox.setInformativeText( trUtf8("Sure you want to exit?"));
     msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
     msgBox.setDefaultButton(QMessageBox::No);
     msgBox.setIcon(QMessageBox::Question);
     int ret = msgBox.exec();
     switch (ret) {
         case QMessageBox::Yes:
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
    QMessageBox::about(this, trUtf8("About Application"),
        trUtf8("The <b>Application</b> is an AsteriskCTI Client based on QT "));
}

void MainWindow::configure()
{
    // does actually nothing

}

void MainWindow::login()
{
    // does actually nothing

}

void MainWindow::pause()
{
    // does actually nothing

}