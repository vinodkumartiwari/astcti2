/********************************************************************************
** Form generated from reading UI file 'passwordwindow.ui'
**
** Created: Mon Mar 15 13:08:55 2010
**      by: Qt User Interface Compiler version 4.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PASSWORDWINDOW_H
#define UI_PASSWORDWINDOW_H

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

class Ui_PasswordWindow
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *passwordFrame;
    QHBoxLayout *innerHorizontalLayout;
    QVBoxLayout *labelVerticalLayout;
    QLabel *oldPasswordLabel;
    QLabel *passwordLabel;
    QLabel *confirmPasswordLabel;
    QVBoxLayout *inputVerticalLayout;
    QLineEdit *oldPasswordLineEdit;
    QLineEdit *passwordLineEdit;
    QLineEdit *confirmPasswordLineEdit;
    QDialogButtonBox *dialogButtonBox;

    void setupUi(QDialog *PasswordWindow)
    {
        if (PasswordWindow->objectName().isEmpty())
            PasswordWindow->setObjectName(QString::fromUtf8("PasswordWindow"));
        PasswordWindow->resize(421, 129);
        verticalLayout = new QVBoxLayout(PasswordWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        passwordFrame = new QFrame(PasswordWindow);
        passwordFrame->setObjectName(QString::fromUtf8("passwordFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(passwordFrame->sizePolicy().hasHeightForWidth());
        passwordFrame->setSizePolicy(sizePolicy);
        innerHorizontalLayout = new QHBoxLayout(passwordFrame);
        innerHorizontalLayout->setContentsMargins(0, 0, 0, 0);
        innerHorizontalLayout->setObjectName(QString::fromUtf8("innerHorizontalLayout"));
        labelVerticalLayout = new QVBoxLayout();
        labelVerticalLayout->setSpacing(3);
        labelVerticalLayout->setObjectName(QString::fromUtf8("labelVerticalLayout"));
        oldPasswordLabel = new QLabel(passwordFrame);
        oldPasswordLabel->setObjectName(QString::fromUtf8("oldPasswordLabel"));
        sizePolicy.setHeightForWidth(oldPasswordLabel->sizePolicy().hasHeightForWidth());
        oldPasswordLabel->setSizePolicy(sizePolicy);

        labelVerticalLayout->addWidget(oldPasswordLabel);

        passwordLabel = new QLabel(passwordFrame);
        passwordLabel->setObjectName(QString::fromUtf8("passwordLabel"));
        sizePolicy.setHeightForWidth(passwordLabel->sizePolicy().hasHeightForWidth());
        passwordLabel->setSizePolicy(sizePolicy);

        labelVerticalLayout->addWidget(passwordLabel);

        confirmPasswordLabel = new QLabel(passwordFrame);
        confirmPasswordLabel->setObjectName(QString::fromUtf8("confirmPasswordLabel"));
        sizePolicy.setHeightForWidth(confirmPasswordLabel->sizePolicy().hasHeightForWidth());
        confirmPasswordLabel->setSizePolicy(sizePolicy);

        labelVerticalLayout->addWidget(confirmPasswordLabel);


        innerHorizontalLayout->addLayout(labelVerticalLayout);

        inputVerticalLayout = new QVBoxLayout();
        inputVerticalLayout->setSpacing(3);
        inputVerticalLayout->setObjectName(QString::fromUtf8("inputVerticalLayout"));
        oldPasswordLineEdit = new QLineEdit(passwordFrame);
        oldPasswordLineEdit->setObjectName(QString::fromUtf8("oldPasswordLineEdit"));
        oldPasswordLineEdit->setEchoMode(QLineEdit::Password);

        inputVerticalLayout->addWidget(oldPasswordLineEdit);

        passwordLineEdit = new QLineEdit(passwordFrame);
        passwordLineEdit->setObjectName(QString::fromUtf8("passwordLineEdit"));
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        inputVerticalLayout->addWidget(passwordLineEdit);

        confirmPasswordLineEdit = new QLineEdit(passwordFrame);
        confirmPasswordLineEdit->setObjectName(QString::fromUtf8("confirmPasswordLineEdit"));
        confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);

        inputVerticalLayout->addWidget(confirmPasswordLineEdit);


        innerHorizontalLayout->addLayout(inputVerticalLayout);


        verticalLayout->addWidget(passwordFrame);

        dialogButtonBox = new QDialogButtonBox(PasswordWindow);
        dialogButtonBox->setObjectName(QString::fromUtf8("dialogButtonBox"));
        dialogButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(dialogButtonBox);

#ifndef QT_NO_SHORTCUT
        oldPasswordLabel->setBuddy(oldPasswordLineEdit);
        passwordLabel->setBuddy(passwordLineEdit);
        confirmPasswordLabel->setBuddy(confirmPasswordLineEdit);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(oldPasswordLineEdit, passwordLineEdit);
        QWidget::setTabOrder(passwordLineEdit, confirmPasswordLineEdit);
        QWidget::setTabOrder(confirmPasswordLineEdit, dialogButtonBox);

        retranslateUi(PasswordWindow);

        QMetaObject::connectSlotsByName(PasswordWindow);
    } // setupUi

    void retranslateUi(QDialog *PasswordWindow)
    {
        PasswordWindow->setWindowTitle(QApplication::translate("PasswordWindow", "Change password", 0, QApplication::UnicodeUTF8));
        oldPasswordLabel->setText(QApplication::translate("PasswordWindow", "&Old password", 0, QApplication::UnicodeUTF8));
        passwordLabel->setText(QApplication::translate("PasswordWindow", "&New password", 0, QApplication::UnicodeUTF8));
        confirmPasswordLabel->setText(QApplication::translate("PasswordWindow", "&Confirm new password", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PasswordWindow: public Ui_PasswordWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PASSWORDWINDOW_H
