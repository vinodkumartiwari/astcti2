/********************************************************************************
** Form generated from reading UI file 'compactwindow.ui'
**
** Created: Mon Mar 15 13:08:55 2010
**      by: Qt User Interface Compiler version 4.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPACTWINDOW_H
#define UI_COMPACTWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_CompactWindow
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *moveLabel;
    QLabel *statusLabel;
    QComboBox *callComboBox;
    QToolButton *callButton;
    QFrame *line1;
    QToolButton *passwordButton;
    QToolButton *aboutButton;
    QFrame *line2;
    QToolButton *repeatButton;
    QFrame *line3;
    QToolButton *pauseButton;
    QToolButton *minimizeButton;
    QToolButton *quitButton;
    QLabel *sizeLabel;

    void setupUi(QDialog *CompactWindow)
    {
        if (CompactWindow->objectName().isEmpty())
            CompactWindow->setObjectName(QString::fromUtf8("CompactWindow"));
        CompactWindow->resize(400, 32);
        CompactWindow->setMinimumSize(QSize(400, 32));
        CompactWindow->setMaximumSize(QSize(16777215, 32));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/res/asteriskcti16x16.png"), QSize(), QIcon::Normal, QIcon::Off);
        CompactWindow->setWindowIcon(icon);
        CompactWindow->setStyleSheet(QString::fromUtf8("#CompactWindow {\n"
"	border: 1px solid purple;\n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(184, 97, 158, 255), stop:1 rgba(255, 255, 255, 255));\n"
"}\n"
"\n"
"QToolButton {\n"
"	background-color: transparent;\n"
"}\n"
"\n"
"QToolButton:hover, QToolButton:focus {\n"
"	background-color: transparent;\n"
"	border-width: 1px;\n"
"	border-style: outset;\n"
"	border-color: rgb(170, 85, 255);\n"
"}\n"
"\n"
"QToolButton:pressed , QToolButton:checked {\n"
"	background-color: transparent;\n"
"	border-width: 1px;\n"
"	border-style: inset;\n"
"	border-color: rgb(170, 85, 255);\n"
"}\n"
"\n"
"#messageLabel[severity=\"Information\"] {\n"
"	color: black;\n"
"}\n"
"\n"
"#messageLabel[severity=\"Warning\"] {\n"
"	color: orange;\n"
"}\n"
"\n"
"#messageLabel[severity=\"Critical\"] {\n"
"	color: red;\n"
"}\n"
"\n"
"#moveLabel {\n"
"	background-image: url(:/res/res/toolbarhandle.png);\n"
"\n"
"}\n"
"\n"
"#sizeLabel {\n"
"	background-image: url(:/res/res/toolbarhandle.png);\n"
"}\n"
""));
        horizontalLayout = new QHBoxLayout(CompactWindow);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setContentsMargins(3, 3, 3, 3);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        moveLabel = new QLabel(CompactWindow);
        moveLabel->setObjectName(QString::fromUtf8("moveLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(moveLabel->sizePolicy().hasHeightForWidth());
        moveLabel->setSizePolicy(sizePolicy);
        moveLabel->setMinimumSize(QSize(6, 0));
        moveLabel->setMaximumSize(QSize(6, 16777215));
        moveLabel->setCursor(QCursor(Qt::SizeAllCursor));

        horizontalLayout->addWidget(moveLabel);

        statusLabel = new QLabel(CompactWindow);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        sizePolicy.setHeightForWidth(statusLabel->sizePolicy().hasHeightForWidth());
        statusLabel->setSizePolicy(sizePolicy);
        statusLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/res/redled.png")));

        horizontalLayout->addWidget(statusLabel);

        callComboBox = new QComboBox(CompactWindow);
        callComboBox->setObjectName(QString::fromUtf8("callComboBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(callComboBox->sizePolicy().hasHeightForWidth());
        callComboBox->setSizePolicy(sizePolicy1);
        callComboBox->setMinimumSize(QSize(0, 0));
        callComboBox->setEditable(true);
        callComboBox->setInsertPolicy(QComboBox::InsertAtTop);

        horizontalLayout->addWidget(callComboBox);

        callButton = new QToolButton(CompactWindow);
        callButton->setObjectName(QString::fromUtf8("callButton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/res/res/call-start.png"), QSize(), QIcon::Normal, QIcon::Off);
        callButton->setIcon(icon1);

        horizontalLayout->addWidget(callButton);

        line1 = new QFrame(CompactWindow);
        line1->setObjectName(QString::fromUtf8("line1"));
        line1->setFrameShape(QFrame::VLine);
        line1->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line1);

        passwordButton = new QToolButton(CompactWindow);
        passwordButton->setObjectName(QString::fromUtf8("passwordButton"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/res/res/configure.png"), QSize(), QIcon::Normal, QIcon::Off);
        passwordButton->setIcon(icon2);

        horizontalLayout->addWidget(passwordButton);

        aboutButton = new QToolButton(CompactWindow);
        aboutButton->setObjectName(QString::fromUtf8("aboutButton"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/res/res/documentinfo.png"), QSize(), QIcon::Normal, QIcon::Off);
        aboutButton->setIcon(icon3);

        horizontalLayout->addWidget(aboutButton);

        line2 = new QFrame(CompactWindow);
        line2->setObjectName(QString::fromUtf8("line2"));
        line2->setFrameShape(QFrame::VLine);
        line2->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line2);

        repeatButton = new QToolButton(CompactWindow);
        repeatButton->setObjectName(QString::fromUtf8("repeatButton"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/res/res/applications-system.png"), QSize(), QIcon::Normal, QIcon::Off);
        repeatButton->setIcon(icon4);

        horizontalLayout->addWidget(repeatButton);

        line3 = new QFrame(CompactWindow);
        line3->setObjectName(QString::fromUtf8("line3"));
        line3->setFrameShape(QFrame::VLine);
        line3->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line3);

        pauseButton = new QToolButton(CompactWindow);
        pauseButton->setObjectName(QString::fromUtf8("pauseButton"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/res/res/agt_pause-queue.png"), QSize(), QIcon::Normal, QIcon::Off);
        pauseButton->setIcon(icon5);
        pauseButton->setCheckable(true);

        horizontalLayout->addWidget(pauseButton);

        minimizeButton = new QToolButton(CompactWindow);
        minimizeButton->setObjectName(QString::fromUtf8("minimizeButton"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/res/res/arrow-down.png"), QSize(), QIcon::Normal, QIcon::Off);
        minimizeButton->setIcon(icon6);

        horizontalLayout->addWidget(minimizeButton);

        quitButton = new QToolButton(CompactWindow);
        quitButton->setObjectName(QString::fromUtf8("quitButton"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/res/res/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        quitButton->setIcon(icon7);

        horizontalLayout->addWidget(quitButton);

        sizeLabel = new QLabel(CompactWindow);
        sizeLabel->setObjectName(QString::fromUtf8("sizeLabel"));
        sizePolicy.setHeightForWidth(sizeLabel->sizePolicy().hasHeightForWidth());
        sizeLabel->setSizePolicy(sizePolicy);
        sizeLabel->setMinimumSize(QSize(3, 0));
        sizeLabel->setMaximumSize(QSize(3, 16777215));
        sizeLabel->setCursor(QCursor(Qt::SizeHorCursor));

        horizontalLayout->addWidget(sizeLabel);

        QWidget::setTabOrder(callComboBox, callButton);
        QWidget::setTabOrder(callButton, passwordButton);
        QWidget::setTabOrder(passwordButton, aboutButton);
        QWidget::setTabOrder(aboutButton, repeatButton);
        QWidget::setTabOrder(repeatButton, pauseButton);
        QWidget::setTabOrder(pauseButton, minimizeButton);
        QWidget::setTabOrder(minimizeButton, quitButton);

        retranslateUi(CompactWindow);

        QMetaObject::connectSlotsByName(CompactWindow);
    } // setupUi

    void retranslateUi(QDialog *CompactWindow)
    {
        CompactWindow->setWindowTitle(QApplication::translate("CompactWindow", "AsteriskCTI", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        callButton->setToolTip(QApplication::translate("CompactWindow", "Call", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        passwordButton->setToolTip(QApplication::translate("CompactWindow", "Change password", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        aboutButton->setToolTip(QApplication::translate("CompactWindow", "About", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        repeatButton->setToolTip(QApplication::translate("CompactWindow", "Repeat last action", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        repeatButton->setText(QString());
#ifndef QT_NO_TOOLTIP
        pauseButton->setToolTip(QApplication::translate("CompactWindow", "Pause", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pauseButton->setShortcut(QApplication::translate("CompactWindow", "Pause", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        minimizeButton->setToolTip(QApplication::translate("CompactWindow", "Minimize to system tray", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        quitButton->setToolTip(QApplication::translate("CompactWindow", "Quit", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        quitButton->setShortcut(QApplication::translate("CompactWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CompactWindow: public Ui_CompactWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPACTWINDOW_H
