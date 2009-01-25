#ifndef BROWSERDIALOG_H
#define BROWSERDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class BrowserDialog;
}

class BrowserDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(BrowserDialog)
public:
    explicit BrowserDialog(QWidget *parent = 0);
    virtual ~BrowserDialog();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::BrowserDialog *m_ui;

private slots:
    void on_btnGo_clicked();
};

#endif // BROWSERDIALOG_H
