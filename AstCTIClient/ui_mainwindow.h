/********************************************************************************
** Form generated from reading ui file 'mainwindow.ui'
**
** Created: Thu Jan 22 03:05:01 2009
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
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
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
    QAction *actionAbout_2;
    QAction *actionAbout_Qt;
    QAction *actionConfigure_2;
    QAction *actionExit_2;
    QWidget *centralWidget;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu;

    void setupUi(QMainWindow *MainWindowClass)
    {
    if (MainWindowClass->objectName().isEmpty())
        MainWindowClass->setObjectName(QString::fromUtf8("MainWindowClass"));
    MainWindowClass->resize(344, 123);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(MainWindowClass->sizePolicy().hasHeightForWidth());
    MainWindowClass->setSizePolicy(sizePolicy);
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
    actionAbout_2 = new QAction(MainWindowClass);
    actionAbout_2->setObjectName(QString::fromUtf8("actionAbout_2"));
    actionAbout_2->setIcon(icon3);
    actionAbout_Qt = new QAction(MainWindowClass);
    actionAbout_Qt->setObjectName(QString::fromUtf8("actionAbout_Qt"));
    actionAbout_Qt->setMenuRole(QAction::AboutQtRole);
    actionConfigure_2 = new QAction(MainWindowClass);
    actionConfigure_2->setObjectName(QString::fromUtf8("actionConfigure_2"));
    actionConfigure_2->setIcon(icon2);
    actionExit_2 = new QAction(MainWindowClass);
    actionExit_2->setObjectName(QString::fromUtf8("actionExit_2"));
    actionExit_2->setIcon(icon4);
    centralWidget = new QWidget(MainWindowClass);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    MainWindowClass->setCentralWidget(centralWidget);
    mainToolBar = new QToolBar(MainWindowClass);
    mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
    mainToolBar->setEnabled(true);
    QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(mainToolBar->sizePolicy().hasHeightForWidth());
    mainToolBar->setSizePolicy(sizePolicy1);
    mainToolBar->setMovable(false);
    mainToolBar->setIconSize(QSize(24, 24));
    mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    mainToolBar->setFloatable(false);
    MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
    statusBar = new QStatusBar(MainWindowClass);
    statusBar->setObjectName(QString::fromUtf8("statusBar"));
    MainWindowClass->setStatusBar(statusBar);
    menuBar = new QMenuBar(MainWindowClass);
    menuBar->setObjectName(QString::fromUtf8("menuBar"));
    menuBar->setGeometry(QRect(0, 0, 344, 25));
    menu_File = new QMenu(menuBar);
    menu_File->setObjectName(QString::fromUtf8("menu_File"));
    menu = new QMenu(menuBar);
    menu->setObjectName(QString::fromUtf8("menu"));
    MainWindowClass->setMenuBar(menuBar);

    mainToolBar->addAction(actionLogin);
    mainToolBar->addAction(actionPause);
    mainToolBar->addSeparator();
    mainToolBar->addAction(actionConfigure);
    mainToolBar->addSeparator();
    mainToolBar->addAction(actionAbout);
    mainToolBar->addSeparator();
    mainToolBar->addAction(actionExit);
    menuBar->addAction(menu_File->menuAction());
    menuBar->addAction(menu->menuAction());
    menu_File->addAction(actionConfigure_2);
    menu_File->addSeparator();
    menu_File->addAction(actionExit_2);
    menu->addAction(actionAbout_2);
    menu->addSeparator();
    menu->addAction(actionAbout_Qt);

    retranslateUi(MainWindowClass);

    QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
    MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "AstCTIClient", 0, QApplication::UnicodeUTF8));
    MainWindowClass->setStyleSheet(QString());
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
    actionAbout_2->setText(QApplication::translate("MainWindowClass", "About", 0, QApplication::UnicodeUTF8));
    actionAbout_Qt->setText(QApplication::translate("MainWindowClass", "About Qt", 0, QApplication::UnicodeUTF8));
    actionConfigure_2->setText(QApplication::translate("MainWindowClass", "Configure", 0, QApplication::UnicodeUTF8));
    actionExit_2->setText(QApplication::translate("MainWindowClass", "Exit", 0, QApplication::UnicodeUTF8));
    mainToolBar->setStyleSheet(QApplication::translate("MainWindowClass", "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(43, 64, 255, 255), stop:1 rgba(255, 255, 255, 255));\n"
"", 0, QApplication::UnicodeUTF8));
    statusBar->setStyleSheet(QApplication::translate("MainWindowClass", "padding: 2px;", 0, QApplication::UnicodeUTF8));
    menu_File->setTitle(QApplication::translate("MainWindowClass", "&File", 0, QApplication::UnicodeUTF8));
    menu->setTitle(QApplication::translate("MainWindowClass", "?", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
