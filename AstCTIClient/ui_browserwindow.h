/********************************************************************************
** Form generated from reading ui file 'browserwindow.ui'
**
** Created: Wed Jan 28 21:56:03 2009
**      by: Qt User Interface Compiler version 4.4.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_BROWSERWINDOW_H
#define UI_BROWSERWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BrowserWindowClass
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *txtUrl;
    QPushButton *btnGo;
    QPushButton *pushButton_2;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *BrowserWindowClass)
    {
    if (BrowserWindowClass->objectName().isEmpty())
        BrowserWindowClass->setObjectName(QString::fromUtf8("BrowserWindowClass"));
    BrowserWindowClass->resize(800, 600);
    centralwidget = new QWidget(BrowserWindowClass);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    verticalLayout_2 = new QVBoxLayout(centralwidget);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    verticalLayout = new QVBoxLayout();
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    txtUrl = new QLineEdit(centralwidget);
    txtUrl->setObjectName(QString::fromUtf8("txtUrl"));

    horizontalLayout->addWidget(txtUrl);

    btnGo = new QPushButton(centralwidget);
    btnGo->setObjectName(QString::fromUtf8("btnGo"));

    horizontalLayout->addWidget(btnGo);

    pushButton_2 = new QPushButton(centralwidget);
    pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

    horizontalLayout->addWidget(pushButton_2);


    verticalLayout->addLayout(horizontalLayout);


    verticalLayout_2->addLayout(verticalLayout);

    BrowserWindowClass->setCentralWidget(centralwidget);
    statusbar = new QStatusBar(BrowserWindowClass);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    BrowserWindowClass->setStatusBar(statusbar);

    retranslateUi(BrowserWindowClass);

    QMetaObject::connectSlotsByName(BrowserWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *BrowserWindowClass)
    {
    BrowserWindowClass->setWindowTitle(QApplication::translate("BrowserWindowClass", "AsteriskCTI Browser", 0, QApplication::UnicodeUTF8));
    btnGo->setText(QApplication::translate("BrowserWindowClass", "&Go", 0, QApplication::UnicodeUTF8));
    pushButton_2->setText(QApplication::translate("BrowserWindowClass", "&Back", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(BrowserWindowClass);
    } // retranslateUi

};

namespace Ui {
    class BrowserWindowClass: public Ui_BrowserWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BROWSERWINDOW_H
