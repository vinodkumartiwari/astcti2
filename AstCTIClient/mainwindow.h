#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtGui>



namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QLabel *lblCurrentTime;
    QLabel *lblCurrentStatus;
    QLabel *linkLabel;
    QIcon *statusImage;

    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
        void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindowClass *ui;

    int labelTime;
    int labelPos;
     QVector<QString> links;

public slots:
    void UpdateTimer();
    void configure();
    void about();
    void login();
    void pause();
};

#endif // MAINWINDOW_H
