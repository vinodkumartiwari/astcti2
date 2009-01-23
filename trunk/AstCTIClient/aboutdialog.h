#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui>
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

private slots:
    void on_btnClose_clicked();
    void on_btnAboutQt_clicked();
};

#endif // ABOUTDIALOG_H
