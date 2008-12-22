#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMessageBox>
#include <QTimer>
#include <QTime>

#include <QLabel>
#include <QtGui/QMainWindow>


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


private:
    Ui::MainWindowClass *ui;

    int labelTime;
    int labelPos;
     QVector<QString> links;

public slots:
    void toolBarActionTriggered(QAction*);
    void UpdateTimer();
    void close();
};

#endif // MAINWINDOW_H
