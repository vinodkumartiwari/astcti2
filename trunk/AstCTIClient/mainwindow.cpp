#include "mainwindow.h"
#include "ui_mainwindow.h"

static int LINKS_TIMEOUT = 30;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent,Qt::Dialog) , ui(new Ui::MainWindowClass)
{

    // Append links to the link label
    links.append("<a href=\"http://code.google.com/p/astcti2\">Asterisk CTI</a>");
    links.append("<a href=\"http://www.advert.it\">Advert SRL</a>");
    links.append("<a href=\"http://centralino-voip.brunosalzano.com\">Centralino Voip</a>");

    labelPos = 0;
    labelTime = LINKS_TIMEOUT;
    ui->setupUi(this);

    statusImage = new QIcon(QPixmap(QString::fromUtf8(":/res/res/redled.png")));
    //this->statusBar()->addWidget(statusImage);

    lblCurrentStatus = new QLabel(trUtf8("<b>Not Connected</b>")) ;
    this->statusBar()->addWidget(lblCurrentStatus);


    lblCurrentTime = new QLabel(QString("00:00:00")) ;
    this->statusBar()->addWidget(lblCurrentTime);

    linkLabel = new QLabel(links.at(0)) ;
    linkLabel->setOpenExternalLinks(true);

    linkLabel->setCursor(Qt::PointingHandCursor);
    this->statusBar()->addWidget(linkLabel);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(UpdateTimer()));
    timer->start(1000);


    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), this, SLOT(configure()));
    connect(ui->actionLogin, SIGNAL(triggered()), this, SLOT(login()));
    connect(ui->actionPause, SIGNAL(triggered()), this, SLOT(pause()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

 void MainWindow::closeEvent(QCloseEvent *event)
 {
     QMessageBox msgBox;
     msgBox.setText( trUtf8("Confirm application close."));
     msgBox.setInformativeText( trUtf8("Sure you want to exit?"));
     msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No );
     msgBox.setDefaultButton(QMessageBox::No);
     msgBox.setIcon(QMessageBox::Question);
     int ret = msgBox.exec();
     switch (ret) {
         case QMessageBox::Yes:
             event->accept();
             return;
     }
     event->ignore();

 }


void MainWindow::UpdateTimer()
{
    QTime qtime = QTime::currentTime();
    QString stime = qtime.toString(Qt::TextDate);

    if (!stime.isNull())
        this->lblCurrentTime->setText(stime);

    labelTime--;
    if (labelTime==0)
    {
        QString link = links.at(labelPos);
        linkLabel->setText(link);
        labelPos++;
        if (labelPos == links.size()) labelPos = 0;
        labelTime = LINKS_TIMEOUT;
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, trUtf8("About Application"),
        trUtf8("The <b>Application</b> is an AsteriskCTI Client based on QT "));
}

void MainWindow::configure()
{
    // does actually nothing

}

void MainWindow::login()
{
    // does actually nothing

}

void MainWindow::pause()
{
    // does actually nothing

}
