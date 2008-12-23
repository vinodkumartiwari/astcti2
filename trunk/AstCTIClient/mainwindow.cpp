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

    lblCurrentStatus = new QLabel("<b>Not Connected</b>") ;
    this->statusBar()->addWidget(lblCurrentStatus);


    lblCurrentTime = new QLabel(QString("00:00:00")) ;
    this->statusBar()->addWidget(lblCurrentTime);

    linkLabel = new QLabel(QString("<a href=\"http://centralino-voip.brunosalzano.com\">Centralino Voip</a>")) ;
    linkLabel->setOpenExternalLinks(true);

    linkLabel->setCursor(Qt::PointingHandCursor);
    this->statusBar()->addWidget(linkLabel);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(UpdateTimer()));
    timer->start(1000);


    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));


}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::toolBarActionTriggered(QAction* action)
{
    if (action->text() != "Exit")
    {
    QMessageBox msgBox;
     msgBox.setText(action->text());


     msgBox.setInformativeText("Action triggered");
     msgBox.setStandardButtons(QMessageBox::Ok);
     msgBox.setDefaultButton(QMessageBox::Ok);
     int ret = msgBox.exec();
    }

}

void MainWindow::close()
{
    QApplication::quit();

}
