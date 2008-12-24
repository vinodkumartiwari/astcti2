/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Wed 24. Dec 06:51:51 2008
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionLogin;
    QAction *actionPause;
    QAction *actionConfigure;
    QAction *actionAbout;
    QAction *actionExit;
    QWidget *centralWidget;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
    if (MainWindowClass->objectName().isEmpty())
        MainWindowClass->setObjectName(QString::fromUtf8("MainWindowClass"));
    MainWindowClass->resize(279, 95);
    MainWindowClass->setContextMenuPolicy(Qt::NoContextMenu);
    MainWindowClass->setAnimated(false);
    actionLogin = new QAction(MainWindowClass);
    actionLogin->setObjectName(QString::fromUtf8("actionLogin"));
    QIcon icon;
    icon.addPixmap(QPixmap(QString::fromUtf8(":/res/res/agt_forward.png")), QIcon::Normal, QIcon::Off);
    actionLogin->setIcon(icon);
    actionPause = new QAction(MainWindowClass);
    actionPause->setObjectName(QString::fromUtf8("actionPause"));
    QIcon icon1;
    icon1.addPixmap(QPixmap(QString::fromUtf8(":/res/res/agt_pause-queue.png")), QIcon::Normal, QIcon::Off);
    actionPause->setIcon(icon1);
    actionConfigure = new QAction(MainWindowClass);
    actionConfigure->setObjectName(QString::fromUtf8("actionConfigure"));
    QIcon icon2;
    icon2.addPixmap(QPixmap(QString::fromUtf8(":/res/res/configure.png")), QIcon::Normal, QIcon::Off);
    actionConfigure->setIcon(icon2);
    actionAbout = new QAction(MainWindowClass);
    actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
    QIcon icon3;
    icon3.addPixmap(QPixmap(QString::fromUtf8(":/res/res/documentinfo.png")), QIcon::Normal, QIcon::Off);
    actionAbout->setIcon(icon3);
    actionExit = new QAction(MainWindowClass);
    actionExit->setObjectName(QString::fromUtf8("actionExit"));
    QIcon icon4;
    icon4.addPixmap(QPixmap(QString::fromUtf8(":/res/res/exit.png")), QIcon::Normal, QIcon::Off);
    actionExit->setIcon(icon4);
    centralWidget = new QWidget(MainWindowClass);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    MainWindowClass->setCentralWidget(centralWidget);
    mainToolBar = new QToolBar(MainWindowClass);
    mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
    mainToolBar->setEnabled(true);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(mainToolBar->sizePolicy().hasHeightForWidth());
    mainToolBar->setSizePolicy(sizePolicy);
    mainToolBar->setMovable(false);
    mainToolBar->setIconSize(QSize(24, 24));
    mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mainToolBar->setFloatable(false);
    MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
    statusBar = new QStatusBar(MainWindowClass);
    statusBar->setObjectName(QString::fromUtf8("statusBar"));
    MainWindowClass->setStatusBar(statusBar);

    mainToolBar->addAction(actionLogin);
    mainToolBar->addAction(actionPause);
    mainToolBar->addSeparator();
    mainToolBar->addAction(actionConfigure);
    mainToolBar->addSeparator();
    mainToolBar->addAction(actionAbout);
    mainToolBar->addSeparator();
    mainToolBar->addAction(actionExit);

    retranslateUi(MainWindowClass);

    QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
    MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "AstCTIClient", 0, QApplication::UnicodeUTF8));
    actionLogin->setText(QApplication::translate("MainWindowClass", "Login", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    actionLogin->setToolTip(QApplication::translate("MainWindowClass", "Login", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    actionLogin->setShortcut(QApplication::translate("MainWindowClass", "Ctrl+L", 0, QApplication::UnicodeUTF8));
    actionPause->setText(QApplication::translate("MainWindowClass", "Pause", 0, QApplication::UnicodeUTF8));
    actionConfigure->setText(QApplication::translate("MainWindowClass", "Configure", 0, QApplication::UnicodeUTF8));
    actionAbout->setText(QApplication::translate("MainWindowClass", "About", 0, QApplication::UnicodeUTF8));
    actionExit->setText(QApplication::translate("MainWindowClass", "Exit", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
    actionExit->setToolTip(QApplication::translate("MainWindowClass", "Exit", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP

    actionExit->setShortcut(QApplication::translate("MainWindowClass", "Alt+E", 0, QApplication::UnicodeUTF8));
    mainToolBar->setStyleSheet(QApplication::translate("MainWindowClass", "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(43, 64, 255, 255), stop:1 rgba(255, 255, 255, 255));\n"
"", 0, QApplication::UnicodeUTF8));
    statusBar->setStyleSheet(QApplication::translate("MainWindowClass", "padding: 2px;", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
