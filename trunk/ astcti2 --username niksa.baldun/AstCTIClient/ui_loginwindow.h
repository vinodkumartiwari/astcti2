/********************************************************************************
** Form generated from reading UI file 'loginwindow.ui'
**
** Created: Mon Mar 15 13:08:55 2010
**      by: Qt User Interface Compiler version 4.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINWINDOW_H
#define UI_LOGINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoginWindow
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *mainHorizontalLayout;
    QLabel *imageLabel;
    QVBoxLayout *loginVerticalLayout;
    QLabel *messageLabel;
    QFrame *credentialsFrame;
    QHBoxLayout *innerHorizontalLayout;
    QVBoxLayout *labelVerticalLayout;
    QLabel *usernameLabel;
    QLabel *passwordLabel;
    QVBoxLayout *inputVerticalLayout;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QDialogButtonBox *dialogButtonBox;

    void setupUi(QDialog *LoginWindow)
    {
        if (LoginWindow->objectName().isEmpty())
            LoginWindow->setObjectName(QString::fromUtf8("LoginWindow"));
        LoginWindow->resize(505, 200);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LoginWindow->sizePolicy().hasHeightForWidth());
        LoginWindow->setSizePolicy(sizePolicy);
        LoginWindow->setMinimumSize(QSize(505, 200));
        LoginWindow->setMaximumSize(QSize(505, 200));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/res/asteriskcti16x16.png"), QSize(), QIcon::Normal, QIcon::Off);
        LoginWindow->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(LoginWindow);
#ifndef Q_OS_MAC
        verticalLayout->setSpacing(6);
#endif
        verticalLayout->setContentsMargins(6, 6, 6, 6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        mainHorizontalLayout = new QHBoxLayout();
        mainHorizontalLayout->setObjectName(QString::fromUtf8("mainHorizontalLayout"));
        imageLabel = new QLabel(LoginWindow);
        imageLabel->setObjectName(QString::fromUtf8("imageLabel"));
        sizePolicy.setHeightForWidth(imageLabel->sizePolicy().hasHeightForWidth());
        imageLabel->setSizePolicy(sizePolicy);
        imageLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/res/asteriskcti-3d128x128.png")));

        mainHorizontalLayout->addWidget(imageLabel);

        loginVerticalLayout = new QVBoxLayout();
        loginVerticalLayout->setObjectName(QString::fromUtf8("loginVerticalLayout"));
        messageLabel = new QLabel(LoginWindow);
        messageLabel->setObjectName(QString::fromUtf8("messageLabel"));
        messageLabel->setWordWrap(true);

        loginVerticalLayout->addWidget(messageLabel);

        credentialsFrame = new QFrame(LoginWindow);
        credentialsFrame->setObjectName(QString::fromUtf8("credentialsFrame"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(credentialsFrame->sizePolicy().hasHeightForWidth());
        credentialsFrame->setSizePolicy(sizePolicy1);
        innerHorizontalLayout = new QHBoxLayout(credentialsFrame);
        innerHorizontalLayout->setContentsMargins(0, 0, 0, 0);
        innerHorizontalLayout->setObjectName(QString::fromUtf8("innerHorizontalLayout"));
        labelVerticalLayout = new QVBoxLayout();
        labelVerticalLayout->setSpacing(3);
        labelVerticalLayout->setObjectName(QString::fromUtf8("labelVerticalLayout"));
        usernameLabel = new QLabel(credentialsFrame);
        usernameLabel->setObjectName(QString::fromUtf8("usernameLabel"));
        sizePolicy1.setHeightForWidth(usernameLabel->sizePolicy().hasHeightForWidth());
        usernameLabel->setSizePolicy(sizePolicy1);

        labelVerticalLayout->addWidget(usernameLabel);

        passwordLabel = new QLabel(credentialsFrame);
        passwordLabel->setObjectName(QString::fromUtf8("passwordLabel"));
        sizePolicy1.setHeightForWidth(passwordLabel->sizePolicy().hasHeightForWidth());
        passwordLabel->setSizePolicy(sizePolicy1);

        labelVerticalLayout->addWidget(passwordLabel);


        innerHorizontalLayout->addLayout(labelVerticalLayout);

        inputVerticalLayout = new QVBoxLayout();
        inputVerticalLayout->setSpacing(3);
        inputVerticalLayout->setObjectName(QString::fromUtf8("inputVerticalLayout"));
        usernameLineEdit = new QLineEdit(credentialsFrame);
        usernameLineEdit->setObjectName(QString::fromUtf8("usernameLineEdit"));

        inputVerticalLayout->addWidget(usernameLineEdit);

        passwordLineEdit = new QLineEdit(credentialsFrame);
        passwordLineEdit->setObjectName(QString::fromUtf8("passwordLineEdit"));
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        inputVerticalLayout->addWidget(passwordLineEdit);


        innerHorizontalLayout->addLayout(inputVerticalLayout);


        loginVerticalLayout->addWidget(credentialsFrame);


        mainHorizontalLayout->addLayout(loginVerticalLayout);


        verticalLayout->addLayout(mainHorizontalLayout);

        dialogButtonBox = new QDialogButtonBox(LoginWindow);
        dialogButtonBox->setObjectName(QString::fromUtf8("dialogButtonBox"));
        dialogButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(dialogButtonBox);

#ifndef QT_NO_SHORTCUT
        imageLabel->setBuddy(credentialsFrame);
        usernameLabel->setBuddy(usernameLineEdit);
        passwordLabel->setBuddy(passwordLineEdit);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(usernameLineEdit, passwordLineEdit);
        QWidget::setTabOrder(passwordLineEdit, dialogButtonBox);

        retranslateUi(LoginWindow);

        QMetaObject::connectSlotsByName(LoginWindow);
    } // setupUi

    void retranslateUi(QDialog *LoginWindow)
    {
        LoginWindow->setWindowTitle(QApplication::translate("LoginWindow", "Login", 0, QApplication::UnicodeUTF8));
        messageLabel->setText(QApplication::translate("LoginWindow", "Welcome to AsteriskCTI!\n"
"\n"
"Please enter your username and password to continue.", 0, QApplication::UnicodeUTF8));
        usernameLabel->setText(QApplication::translate("LoginWindow", "&Username", 0, QApplication::UnicodeUTF8));
        passwordLabel->setText(QApplication::translate("LoginWindow", "&Password", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class LoginWindow: public Ui_LoginWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINWINDOW_H
