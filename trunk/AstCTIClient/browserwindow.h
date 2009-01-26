#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QtGui>
#include <QtGui/QMainWindow>

namespace Ui {
    class BrowserWindowClass;
}

class BrowserWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit BrowserWindow(QWidget *parent = 0);
    virtual ~BrowserWindow();

protected:
    QLabel *lblCurrentStatus;
    virtual void changeEvent(QEvent *e);

private:
    Ui::BrowserWindowClass *m_ui;
    void addExtraWidgets();

private slots:
    void on_webView_loadProgress(int progress);
    void on_webView_titleChanged(QString title);
    void on_webView_statusBarMessage(QString text);
    void on_webView_loadFinished(bool );
    void on_btnGo_clicked();
    void on_webView_loadStarted();
};

#endif // BROWSERWINDOW_H
