#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui>
#include "coreconstants.h"

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(AboutDialog)
public:
    explicit AboutDialog(QWidget *parent = 0);
    virtual ~AboutDialog();

protected:
    virtual void changeEvent(QEvent *e);


private:
    Ui::AboutDialog *m_ui;
    void setUpInfoLabel();

private slots:
    void on_btnShowLicence_clicked();
    void on_btnClose_clicked();
    void on_btnAboutQt_clicked();
};

#endif // ABOUTDIALOG_H
