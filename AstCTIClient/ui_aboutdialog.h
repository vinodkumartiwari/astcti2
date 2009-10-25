/********************************************************************************
** Form generated from reading ui file 'aboutdialog.ui'
**
** Created: Sat Oct 24 14:01:55 2009
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QLabel *lblPicture;
    QPushButton *btnClose;
    QPushButton *btnShowLicence;
    QPushButton *btnAboutQt;
    QLabel *lblCopyright;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName(QString::fromUtf8("AboutDialog"));
        AboutDialog->resize(563, 256);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/res/asteriskcti.png"), QSize(), QIcon::Normal, QIcon::Off);
        AboutDialog->setWindowIcon(icon);
        lblPicture = new QLabel(AboutDialog);
        lblPicture->setObjectName(QString::fromUtf8("lblPicture"));
        lblPicture->setGeometry(QRect(10, 10, 141, 131));
        lblPicture->setAutoFillBackground(true);
        lblPicture->setPixmap(QPixmap(QString::fromUtf8(":/res/res/asteriskcti-3d128x128.png")));
        btnClose = new QPushButton(AboutDialog);
        btnClose->setObjectName(QString::fromUtf8("btnClose"));
        btnClose->setGeometry(QRect(470, 210, 81, 31));
        btnClose->setAutoFillBackground(false);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/res/res/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnClose->setIcon(icon1);
        btnShowLicence = new QPushButton(AboutDialog);
        btnShowLicence->setObjectName(QString::fromUtf8("btnShowLicence"));
        btnShowLicence->setGeometry(QRect(10, 210, 111, 31));
        btnAboutQt = new QPushButton(AboutDialog);
        btnAboutQt->setObjectName(QString::fromUtf8("btnAboutQt"));
        btnAboutQt->setGeometry(QRect(140, 210, 111, 31));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/res/res/documentinfo.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnAboutQt->setIcon(icon2);
        lblCopyright = new QLabel(AboutDialog);
        lblCopyright->setObjectName(QString::fromUtf8("lblCopyright"));
        lblCopyright->setGeometry(QRect(170, 10, 381, 181));

        retranslateUi(AboutDialog);

        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QApplication::translate("AboutDialog", "About AsteriskCTI Client", 0, QApplication::UnicodeUTF8));
        AboutDialog->setStyleSheet(QString());
        lblPicture->setStyleSheet(QString());
        lblPicture->setText(QString());
        btnClose->setText(QApplication::translate("AboutDialog", "&Close", 0, QApplication::UnicodeUTF8));
        btnShowLicence->setText(QApplication::translate("AboutDialog", "Show &Licence", 0, QApplication::UnicodeUTF8));
        btnAboutQt->setText(QApplication::translate("AboutDialog", "About &QT", 0, QApplication::UnicodeUTF8));
        lblCopyright->setText(QString());
        Q_UNUSED(AboutDialog);
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
