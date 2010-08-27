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
 * %PROJECTNAME%.  If you copy code from other releases into a copy of GNU
 * %PROJECTNAME%, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for %PROJECTNAME%, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include <QBitmap>
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

#include "coreconstants.h"
#include "aboutdialog.h"
#include "managerwindow.h"
#include "ui_managerwindow.h"

ManagerWindow::ManagerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManagerWindow)
{
    ui->setupUi(this);

    //TODO
    //Workaround for QTBUG-13157 http://bugreports.qt.nokia.com/browse/QTBUG-13157
    this->ui->callLogTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color: rgb(0, 170, 255); color: white; border: 1px solid rgb(0, 102, 153);}");
    this->ui->contactsTableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color: rgb(0, 170, 255); color: white; border: 1px solid rgb(0, 102, 153);}");

    accountsLayout = new QHBoxLayout(ui->accountsFrame);
    accountsLayout->setObjectName(QString::fromUtf8("accountsLayout"));
    accountsLayout->setContentsMargins(0, 2, 0, 0);
    accountsTabBar = new QTabBar(ui->accountsFrame);
    accountsTabBar->setObjectName(QString::fromUtf8("accountsTabBar"));
    accountsTabBar->setExpanding(false);
    accountsTabBar->addTab("Account 1");
    accountsLayout->addWidget(accountsTabBar);

    this->ui->captionLabel->installEventFilter(this);

    this->setWindowFlags((this->windowFlags() | Qt::Window
                                              | Qt::FramelessWindowHint
                                              | Qt::CustomizeWindowHint)
                                              & ~Qt::WindowSystemMenuHint
                                              & ~Qt::WindowTitleHint
                                              & ~Qt::WindowMinMaxButtonsHint
                                              & ~Qt::WindowCloseButtonHint);

    this->connectSlots();

    this->ui->aboutButton->setToolTip(QString(APP_NAME) + " " + QString(APP_VERSION_LONG));

    this->dragOrigin = QPoint(-1, -1);

    //TEMP
    //TableWidget will be replaced by TableView
    this->ui->contactsTableWidget->setHorizontalHeaderLabels(QStringList() << "Name" << "Number");
    QTableWidgetItem *newItem;
    newItem = new QTableWidgetItem(tr("Contact1"));
    this->ui->contactsTableWidget->setItem(0, 0, newItem);
    newItem = new QTableWidgetItem(tr("123456789"));
    this->ui->contactsTableWidget->setItem(0, 1, newItem);
    newItem = new QTableWidgetItem(tr("Contact2"));
    this->ui->contactsTableWidget->setItem(1, 0, newItem);
    newItem = new QTableWidgetItem(tr("987654321"));
    this->ui->contactsTableWidget->setItem(1, 1, newItem);
    this->ui->callLogTableWidget->setHorizontalHeaderLabels(QStringList() << "Number" << "Ring time" << "Ring duration" << "Call duration");
    newItem = new QTableWidgetItem(tr("1234567890"));
    this->ui->callLogTableWidget->setItem(0, 0, newItem);
    newItem = new QTableWidgetItem(tr("Today 13:15"));
    this->ui->callLogTableWidget->setItem(0, 1, newItem);
    newItem = new QTableWidgetItem(tr("12s"));
    this->ui->callLogTableWidget->setItem(0, 2, newItem);
    newItem = new QTableWidgetItem(tr("58s"));
    this->ui->callLogTableWidget->setItem(0, 3, newItem);
}

ManagerWindow::~ManagerWindow()
{
    delete accountsLayout;
    delete accountsTabBar;
    delete ui;
}

void ManagerWindow::connectSlots()
{
    connect(ui->aboutButton, SIGNAL(clicked()), this, SLOT(about()));
    connect(ui->helpButton, SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(ui->minimizeButton, SIGNAL(clicked()), this, SLOT(showMinimized()));
    connect(ui->maximizeButton, SIGNAL(clicked()), this, SLOT(showMaxRestore()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

void ManagerWindow::about()
{
    AboutDialog aboutDialog;
    aboutDialog.exec();
}

void ManagerWindow::showHelp()
{
    //TODO
}

void ManagerWindow::showMaxRestore()
{
    if (this->isMaximized()) {
        this->showNormal();
    } else {
        this->showMaximized();
    }
}

void ManagerWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::WindowStateChange) {
        if (this->isMaximized()) {
            this->ui->maximizeButton->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/window-restore.png"))));
            this->ui->maximizeButton->setToolTip(trUtf8("Restore"));
        } else {
            this->ui->maximizeButton->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/window-maximize.png"))));
            this->ui->maximizeButton->setToolTip(trUtf8("Maximize"));
        }
    }
}

void ManagerWindow::resizeEvent(QResizeEvent *e)
{
    if (!this->isFullScreen() && !this->isMaximized()) {
        QImage image(e->size(), QImage::Format_Mono);
        image.fill(1);

        image.setPixel(0, 0, 0); image.setPixel(1, 0, 0); image.setPixel(2, 0, 0); image.setPixel(3, 0, 0);
        image.setPixel(0, 1, 0); image.setPixel(1, 1, 0);
        image.setPixel(0, 2, 0);
        image.setPixel(0, 3, 0);

        image.setPixel(width() - 4, 0, 0); image.setPixel(width() - 3, 0, 0); image.setPixel(width() - 2, 0, 0); image.setPixel(width() - 1, 0, 0);
                                                                              image.setPixel(width() - 2, 1, 0); image.setPixel(width() - 1, 1, 0);
                                                                                                                 image.setPixel(width() - 1, 2, 0);
                                                                                                                 image.setPixel(width() - 1, 3, 0);

        image.setPixel(0, height() - 4, 0);
        image.setPixel(0, height() - 3, 0);
        image.setPixel(0, height() - 2, 0); image.setPixel(1, height() - 2, 0);
        image.setPixel(0, height() - 1, 0); image.setPixel(1, height() - 1, 0); image.setPixel(2, height() - 1, 0); image.setPixel(3, height() - 1, 0);

                                                                                                                                                  image.setPixel(width() - 1, height() - 4, 0);
                                                                                                                                                  image.setPixel(width() - 1, height() - 3, 0);
                                                                                                    image.setPixel(width() - 2, height() - 2, 0); image.setPixel(width() - 1, height() - 2, 0);
        image.setPixel(width() - 4, height() - 1, 0); image.setPixel(width() - 3, height() - 1, 0); image.setPixel(width() - 2, height() - 1, 0); image.setPixel(width() - 1, height() - 1, 0);

        this->setMask(QPixmap::fromImage(image).createMaskFromColor(Qt::black));
    } else {
        this->clearMask();
        this->resizeLeft = false;
        this->resizeRight = false;
        this->resizeUp = false;
        this->resizeDown = false;
    }

// Alternative
//    if (!this->isFullScreen() && !this->isMaximized()) {
//        //Draw window with rounded corners
//        QPixmap pixmap(e->size());
//        pixmap.fill(Qt::white);
//        QPainter painter(&pixmap);
//        painter.setRenderHint(QPainter::Antialiasing);
//        painter.setBrush(Qt::black);
//        painter.setPen(Qt::NoPen);
//        painter.drawRoundedRect(pixmap.rect(), 6, 6);
//        this->setMask(pixmap.createMaskFromColor(Qt::white));
//    } else {
//        this->clearMask();
//        this->resizeLeft = false;
//        this->resizeRight = false;
//        this->resizeUp = false;
//        this->resizeDown = false;
//    }
}

void ManagerWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (this->dragOrigin != QPoint(-1, -1)) {
        int dx = e->globalX() - this->dragOrigin.x();
        int dy = e->globalY() - this->dragOrigin.y();

        QRect g = this->geometry();

        if (this->resizeLeft)
            g.setLeft(g.left() + dx);
        if (this->resizeRight)
            g.setRight(g.right() + dx);
        if (this->resizeUp)
            g.setTop(g.top() + dy);
        if (this->resizeDown)
            g.setBottom(g.bottom() + dy);

        this->setGeometry(g);

        this->dragOrigin = e->globalPos();
    } else {
        if (!this->isFullScreen() && !this->isMaximized()) {
            int x = e->x();
            int y = e->y();

            QRect r = this->rect();
            this->resizeLeft = qAbs(x - r.left()) <= borderWidth;
            this->resizeRight = qAbs(x - r.right()) <= borderWidth;
            this->resizeUp = qAbs(y - r.top()) <= borderWidth;
            this->resizeDown = qAbs(y - r.bottom()) <= borderWidth;

            if ((this->resizeLeft & this->resizeUp) || (this->resizeRight & this->resizeDown))
                this->setCursor(QCursor(Qt::SizeFDiagCursor));
            else if ((this->resizeLeft & this->resizeDown) || (this->resizeRight & this->resizeUp))
                this->setCursor(QCursor(Qt::SizeBDiagCursor));
            else if (this->resizeLeft || this->resizeRight)
                this->setCursor(QCursor(Qt::SizeHorCursor));
            else if (this->resizeUp || this->resizeDown)
                this->setCursor(QCursor(Qt::SizeVerCursor));
            else
                //By default, child widgets inherit the cursor form the parent, which means
                //that if the mouse pointer moves from border directly to child widget,
                //the cursor will keep whatever shape it had.
                //Therefore, the cursor for all DIRECT children of this window (QFrames in this case)
                //must explicitly be set to Qt::ArrowCursor in designer or constructor.
                //In designer, it appears that the cursor is already set to Qt::ArrowCursor, but actually,
                //the cursor is unset. It must be set to some other value, then back to ArrowCursor.
                this->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void ManagerWindow::mousePressEvent(QMouseEvent *e)
{
    if (isValidDrag(e))
        this->dragOrigin = e->globalPos();
}

void ManagerWindow::mouseReleaseEvent(QMouseEvent *)
{
    this->dragOrigin = QPoint(-1, -1);
}

bool ManagerWindow::eventFilter(QObject *object, QEvent *e)
{
    bool accepted = false;

    QEvent::Type type = e->type();

    if (object == this->ui->captionLabel) {
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
            //TODO
            //Integrate moving with window manager
            //e.g. in Compiz, window is not moved to another desktop when it reaches the egde of screen
            if (this->dragOrigin != QPoint(-1, -1)) {
                this->move(this->pos() + (mouseEvent->globalPos() - this->dragOrigin));
                this->dragOrigin = mouseEvent->globalPos();
                e->accept();
                accepted = true;
            }
        } else if (type == QEvent::MouseButtonDblClick) {
            if (mouseEvent->button() == Qt::LeftButton) {
                this->showMaxRestore();
                e->accept();
                accepted = true;
            }
        }
    }

    return accepted;
}

bool ManagerWindow::isValidDrag(QMouseEvent *mouseEvent) const
{
    if (!mouseEvent || mouseEvent->modifiers() != Qt::NoModifier)
        return false;

    if (mouseEvent->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::LeftButton)
        return true;
    else
        return false;
}
