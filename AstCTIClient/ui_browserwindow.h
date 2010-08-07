/********************************************************************************
** Form generated from reading UI file 'browserwindow.ui'
**
** Created: Sat Jul 24 16:20:03 2010
**      by: Qt User Interface Compiler version 4.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BROWSERWINDOW_H
#define UI_BROWSERWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BrowserWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QToolButton *prevButton;
    QToolButton *nextButton;
    QSpacerItem *horizontalSpacer;
    QToolButton *reloadButton;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *stopButton;
    QLineEdit *urlLineEdit;
    QToolButton *goButton;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *BrowserWindow)
    {
        if (BrowserWindow->objectName().isEmpty())
            BrowserWindow->setObjectName(QString::fromUtf8("BrowserWindow"));
        BrowserWindow->resize(800, 600);
        BrowserWindow->setStyleSheet(QString::fromUtf8("#prevButton, #stopButton {\n"
"	border-top-left-radius: 3px;\n"
"	border-bottom-left-radius: 3px;\n"
"	border: 1px solid gray;\n"
"	border-right: none;\n"
"	background: transparent;\n"
"}\n"
"#nextButton, #goButton {\n"
"	border-top-right-radius: 3px;\n"
"	border-bottom-right-radius: 3px;\n"
"	border: 1px solid gray;\n"
"	border-left: none;\n"
"	background: transparent;\n"
"}\n"
"#reloadButton {\n"
"	border-radius: 3px;\n"
"	border: 1px solid gray;\n"
"	background: transparent;\n"
"}\n"
"#urlLineEdit {\n"
"	border: 1px solid gray;\n"
"}"));
        centralwidget = new QWidget(BrowserWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        prevButton = new QToolButton(centralwidget);
        prevButton->setObjectName(QString::fromUtf8("prevButton"));
        prevButton->setMinimumSize(QSize(26, 25));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/res/browser-previous.png"), QSize(), QIcon::Normal, QIcon::Off);
        prevButton->setIcon(icon);

        horizontalLayout->addWidget(prevButton);

        nextButton = new QToolButton(centralwidget);
        nextButton->setObjectName(QString::fromUtf8("nextButton"));
        nextButton->setMinimumSize(QSize(26, 25));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/res/res/browser-next.png"), QSize(), QIcon::Normal, QIcon::Off);
        nextButton->setIcon(icon1);

        horizontalLayout->addWidget(nextButton);

        horizontalSpacer = new QSpacerItem(6, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        reloadButton = new QToolButton(centralwidget);
        reloadButton->setObjectName(QString::fromUtf8("reloadButton"));
        reloadButton->setMinimumSize(QSize(26, 25));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/res/res/browser-refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        reloadButton->setIcon(icon2);

        horizontalLayout->addWidget(reloadButton);

        horizontalSpacer_2 = new QSpacerItem(6, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        stopButton = new QToolButton(centralwidget);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));
        stopButton->setMinimumSize(QSize(26, 25));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/res/res/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        stopButton->setIcon(icon3);

        horizontalLayout->addWidget(stopButton);

        urlLineEdit = new QLineEdit(centralwidget);
        urlLineEdit->setObjectName(QString::fromUtf8("urlLineEdit"));
        urlLineEdit->setMinimumSize(QSize(0, 25));

        horizontalLayout->addWidget(urlLineEdit);

        goButton = new QToolButton(centralwidget);
        goButton->setObjectName(QString::fromUtf8("goButton"));
        goButton->setMinimumSize(QSize(26, 25));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/res/res/browser-go.png"), QSize(), QIcon::Normal, QIcon::Off);
        goButton->setIcon(icon4);

        horizontalLayout->addWidget(goButton);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);

        BrowserWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(BrowserWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        BrowserWindow->setStatusBar(statusbar);
        QWidget::setTabOrder(urlLineEdit, goButton);
        QWidget::setTabOrder(goButton, prevButton);
        QWidget::setTabOrder(prevButton, nextButton);
        QWidget::setTabOrder(nextButton, reloadButton);
        QWidget::setTabOrder(reloadButton, stopButton);

        retranslateUi(BrowserWindow);

        QMetaObject::connectSlotsByName(BrowserWindow);
    } // setupUi

    void retranslateUi(QMainWindow *BrowserWindow)
    {
        BrowserWindow->setWindowTitle(QApplication::translate("BrowserWindow", "AsteriskCTI Browser", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        prevButton->setToolTip(QApplication::translate("BrowserWindow", "Previous page", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        nextButton->setToolTip(QApplication::translate("BrowserWindow", "Next page", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        reloadButton->setToolTip(QApplication::translate("BrowserWindow", "Reload current page", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        stopButton->setToolTip(QApplication::translate("BrowserWindow", "Stop loading current page", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        goButton->setToolTip(QApplication::translate("BrowserWindow", "Go to this page", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class BrowserWindow: public Ui_BrowserWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BROWSERWINDOW_H
