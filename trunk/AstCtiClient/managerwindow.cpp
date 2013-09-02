/* Copyright (C) 2007-2010 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * Copyright (C) 2007-2010 Lumiss d.o.o.
 * http://www.lumiss.hr
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * This exception applies only to the code released under the name GNU
 * AstCtiClient.  If you copy code from other releases into a copy of GNU
 * AstCtiClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCtiClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include <QBitmap>
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

#include "coreconstants.h"
#include "aboutdialog.h"
#include "astctidefaultstyle.h"
#include "astcticallwidget.h"
#include "astctispeeddialwidget.h"
#include "managerwindow.h"
#include "ui_managerwindow.h"

ManagerWindow::ManagerWindow(AstCtiConfiguration* const config) :
	CtiClientWindow(config), ui(new Ui::ManagerWindow)
{
    ui->setupUi(this);

    QApplication::setStyle(new AstCtiDefaultStyle);

    //TODO
    //Workaround for QTBUG-13157 http://bugreports.qt.nokia.com/browse/QTBUG-13157
	//ui->callLogTableWidget->horizontalHeader()->setStyleSheet(
	//        "QHeaderView::section {background-color: rgb(0, 170, 255); color: white; "
	//        "border: 1px solid rgb(0, 102, 153);}");
	//ui->contactsTableWidget->horizontalHeader()->setStyleSheet(
	//        "QHeaderView::section {background-color: rgb(0, 170, 255); color: white; "
	//        "border: 1px solid rgb(0, 102, 153);}");

    titleDummyWidget = new QWidget();
	titleDummyWidget->setObjectName(QStringLiteral("titleWidget"));
    titleDummyWidget->setMinimumSize(ui->titleWidget->minimumSize());
    titleDummyWidget->setMaximumSize(ui->titleWidget->maximumSize());
    titleStackedLayout = new QStackedLayout(titleDummyWidget);
	titleStackedLayout->setObjectName(QStringLiteral("titleStackedLayout"));
    titleStackedLayout->setStackingMode(QStackedLayout::StackAll);
    titleStackedLayout->setContentsMargins(0, 0, 0, 0);
    titleStackedLayout->addWidget(ui->aboutWidget);
    titleStackedLayout->addWidget(ui->titleWidget);
//    accountsLayout = new QHBoxLayout(ui->accountsWidget);
//    accountsLayout->setObjectName(QStringLiteral("accountsLayout"));
//    accountsLayout->setContentsMargins(46, 2, 0, 0);
//    accountsTabBar = new QTabBar(ui->accountsWidget);
//    accountsTabBar->setObjectName(QStringLiteral("accountsTabBar"));
//    accountsTabBar->setExpanding(false);
//    accountsTabBar->setDrawBase(false);
//    accountsTabBar->addTab("Account 1");
//    accountsLayout->addWidget(accountsTabBar);
    mainLayout = new QVBoxLayout(this);
	mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins(borderWidth, borderWidth, borderWidth, borderWidth);
    mainLayout->setSpacing(borderWidth);
    mainLayout->addWidget(titleDummyWidget);
    mainLayout->addWidget(ui->centerSplitter);
    mainLayout->addWidget(ui->parkWidget);

    ui->captionLabel->installEventFilter(this);

    this->setWindowFlags((this->windowFlags() | Qt::Window
                                              | Qt::FramelessWindowHint
                                              | Qt::CustomizeWindowHint)
                                              & ~Qt::WindowSystemMenuHint
                                              & ~Qt::WindowTitleHint
                                              & ~Qt::WindowMinMaxButtonsHint
                                              & ~Qt::WindowCloseButtonHint);

    this->ui->aboutButton->setToolTip(QString(APP_NAME) + " " + QString(APP_VERSION_LONG));

    this->connectSlots();
    this->readSettings();

	this->newConfig(config);

    //TEMP
    //TableWidget will be replaced by TableView
    this->ui->contactsTableWidget->setHorizontalHeaderLabels(QStringList() << "Name" << "Number");
    QTableWidgetItem* newItem;
    newItem = new QTableWidgetItem(tr("Contact1"));
    this->ui->contactsTableWidget->setItem(0, 0, newItem);
    newItem = new QTableWidgetItem(tr("123456789"));
    this->ui->contactsTableWidget->setItem(0, 1, newItem);
    newItem = new QTableWidgetItem(tr("Contact2"));
    this->ui->contactsTableWidget->setItem(1, 0, newItem);
    newItem = new QTableWidgetItem(tr("987654321"));
    this->ui->contactsTableWidget->setItem(1, 1, newItem);
	this->ui->callLogTableWidget->setHorizontalHeaderLabels(
			QStringList() << "Number" << "Ring time" << "Ring duration" << "Call duration");
    newItem = new QTableWidgetItem(tr("1234567890"));
    this->ui->callLogTableWidget->setItem(0, 0, newItem);
    newItem = new QTableWidgetItem(tr("Today 13:15"));
    this->ui->callLogTableWidget->setItem(0, 1, newItem);
    newItem = new QTableWidgetItem(tr("12s"));
    this->ui->callLogTableWidget->setItem(0, 2, newItem);
    newItem = new QTableWidgetItem(tr("58s"));
    this->ui->callLogTableWidget->setItem(0, 3, newItem);
}

ManagerWindow::~ManagerWindow()
{
//    delete accountsLayout;
//    delete accountsTabBar;
	qDeleteAll(this->queueButtons.keys());
	delete this->mainLayout;
	delete this->titleDummyWidget;
	//delete this->titleStackedLayout;
	delete this->ui;
}

void ManagerWindow::connectSlots()
{
//    connect(ui->callButton, SIGNAL(clicked()),
//	          this, SLOT(placeCall()));
	connect(this->ui->aboutButton, SIGNAL(clicked()),
			this, SLOT(about()));
	connect(this->ui->minimizeButton, SIGNAL(clicked()),
			this, SLOT(minimizeToTray()));
	connect(this->ui->maximizeButton, SIGNAL(clicked()),
			this, SLOT(showMaxRestore()));
	connect(this->ui->startPauseButton, SIGNAL(clicked()),
			this, SLOT(agentStartPause()));
//    connect(this->ui->passwordButton, SIGNAL(clicked()),
//            this, SIGNAL(changePassword()));
	connect(this->ui->closeButton, SIGNAL(clicked(bool)),
			this, SLOT(quit(bool)));
	connect(this->trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	connect(this->ui->accountsTabWidget, SIGNAL(currentChanged(int)),
			this, SLOT(switchAccount(int)));
}

void ManagerWindow::newConfig(AstCtiConfiguration* config)
{
	CtiClientWindow::newConfig(config);

	bool isInitialized = this->ui->accountsTabWidget->count() > 0;

	this->ui->accountsTabWidget->setUpdatesEnabled(false);

	const int listSize = config->extensions.size();

	if (!isInitialized) {
		// We only set accounts the first time this procedure is called
		// After than, only extension name can change
		// Changing extension number(s) would cause dropping the connection
		for (int i = 0; i < listSize; i++) {
			AstCtiExtensionPtr extension = config->extensions.at(i);

			QWidget* tab = new QWidget();
			tab->setObjectName("tab" + extension->number);
			tab->setProperty("Paused", QVariant(true));
			this->ui->accountsTabWidget->addTab(tab, QString());
		}
	}
	for (int i = 0; i < listSize; i++) {
		AstCtiExtensionPtr extension = config->extensions.at(i);
		QString ext = extension->number;
		if (!extension->name.isEmpty())
			ext = QString("%1 (%2)").arg(extension->name).arg(ext);
		this->ui->accountsTabWidget->setTabText(i, ext);
	}

	this->ui->accountsTabWidget->setUpdatesEnabled(true);

	this->ui->speedDialsTabWidget->setUpdatesEnabled(false);

	for (int i = 0; i < this->ui->speedDialsTabWidget->count(); i++)
		delete this->ui->speedDialsTabWidget->widget(i);
	this->ui->speedDialsTabWidget->clear();

	QString lastGroup = "";
	QWidget* newTab = 0;

	for (AstCtiSpeedDialPtrMap::const_iterator i = config->speedDials.constBegin();
		 i != config->speedDials.constEnd();
		 i++) {
		AstCtiSpeedDialPtr speedDial = i.value();

		if (lastGroup != speedDial->groupName) {
			lastGroup = speedDial->groupName;
			newTab = new QWidget();
			newTab->setObjectName(lastGroup + "Tab");
			this->ui->speedDialsTabWidget->addTab(newTab, lastGroup);
		}

		AstCtiSpeedDialWidget* sdWidget = new AstCtiSpeedDialWidget(newTab);

		sdWidget->setName(speedDial->name);
		sdWidget->setNumber(speedDial->number);
		sdWidget->setBusyLampField(speedDial->isBlf);
	}
	if (this->ui->speedDialsTabWidget->count() == 0) {
		newTab = new QWidget();
		newTab->setObjectName(QStringLiteral("defaultTab"));
		this->ui->speedDialsTabWidget->addTab(newTab, tr("Default"));
	}

	this->ui->speedDialsTabWidget->setUpdatesEnabled(true);

	if (!isInitialized) {
		if (config->isCallCenter) {
			this->ui->queuesTableView->setUpdatesEnabled(false);

			this->queuesModel = new QStandardItemModel();
			this->ui->queuesTableView->setModel(this->queuesModel);

			QStringList headerLabels;
			headerLabels.append(QStringLiteral(""));
			headerLabels.append(tr("Queue"));
			headerLabels.append(tr("Status"));
			this->queuesModel->setHorizontalHeaderLabels(headerLabels);

			int row = 0;
			QStandardItem *newItem = 0;
			// All extensions must have the same queues
			AstCtiExtensionPtr extension = config->extensions.at(0);
			for (AstCtiAgentStatusHash::const_iterator i = extension->queues.constBegin();
				 i != extension->queues.constEnd();
				 i++) {
				const QString queue = i.key();

				QToolButton* queueButton = new QToolButton();
				connect(queueButton, SIGNAL(clicked()),
						this, SLOT(startPauseButtonClicked()));

				// First column, containing a button
				newItem = new QStandardItem();
				this->queuesModel->setItem(row, 0, newItem);
				this->ui->queuesTableView->setIndexWidget(newItem->index(), queueButton);
				this->queueButtons.insert(queueButton, queue);

				// Second columnn with queue name
				newItem = new QStandardItem(queue);
				this->queuesModel->setItem(row, 1, newItem);

				// Third columnn with agent status
				newItem = new QStandardItem();
				this->queuesModel->setItem(row, 2, newItem);

				row++;
			}

			// No need for this, switchAccount() will enable updates
			//this->ui->queuesTableView->setUpdatesEnabled(true);
		} else {
			this->ui->bottomRightTabWidget->removeTab(3);
			this->ui->startPauseButton->setVisible(false);
		}
	}

	// Read data for the current account
	this->switchAccount(this->ui->accountsTabWidget->currentIndex());
}

void ManagerWindow::handleChannelEvent(AstCtiChannel* channel)
{
	const QString eventName = channel->lastEvent;

	if (eventName == QStringLiteral("NewChannel")) {
		AstCtiCallWidget* callWidget = new AstCtiCallWidget(this->ui->callsScrollArea);
		callWidget->setCallerIdNumber(channel->callerIdNum);
		callWidget->setCallerIdName(channel->callerIdName);
		callWidget->setCallDirection(channel->dialedLineNum.isEmpty()
									 ? AstCtiCallWidget::CallDirectionIncoming
									 : AstCtiCallWidget::CallDirectionOutgoing);
		callWidget->setCallState(AstCtiCallWidget::callStateFromString(channel->state));
		// TODO: Answering is currently not possible because there is no way to tell the
		// TODO: ringing phone to answer. Asterisk 11 may include this functionality
		callWidget->setCanAnswer(false);
		// There is currently no way to tell the phone to hold while in-call
		// Hold will be implemented via parking feature
		callWidget->setCanHold(true);
		callWidget->setCanConference(true);
		callWidget->setCanTransfer(true);
		callWidget->setCanRecord(this->config->canRecord);
		// TODO: Change when contacs are implemented
		callWidget->setCanEditContact(true);
	}
}

void ManagerWindow::agentStartPause()
{
	bool allAccounts;
	if (this->ui->accountsTabWidget->count() > 1) {
		QMessageBox msgBox;

		msgBox.setText(tr("You have more than one account."));
		msgBox.setInformativeText(tr("Do you want to start/pause all accounts "
									 "or only the current account?"));
		QAbstractButton* allBtn = msgBox.addButton(tr("All accounts"), QMessageBox::YesRole);
		QAbstractButton* currentBtn =
				msgBox.addButton(tr("Current account only"), QMessageBox::NoRole);
		msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);

		msgBox.setIcon(QMessageBox::Question);
		msgBox.exec();
		if (msgBox.clickedButton() == allBtn)
			allAccounts = true;
		else if (msgBox.clickedButton() == currentBtn)
			allAccounts = false;
		else
			return;
	}

	//Disable all buttons
	this->ui->startPauseButton->setEnabled(false);
	for (QHash<QToolButton*, QString>::const_iterator i = this->queueButtons.constBegin();
		 i != this->queueButtons.constEnd();
		 i++) {
		QToolButton* const queueButton = i.key();
		queueButton->setEnabled(false);
	}

	bool paused = this->ui->accountsTabWidget->currentWidget()->property("Paused").toBool();

	const int listSize = this->config->extensions.size();
	for (int i = 0; i < listSize; i++) {
		if (allAccounts || i == this->ui->accountsTabWidget->currentIndex()) {
			//Send start/pause request for all queues this account belongs to
			AstCtiExtensionPtr extension = this->config->extensions.at(i);
			if (paused == this->ui->accountsTabWidget->widget(i)->property("Paused").toBool()) {
				for (AstCtiAgentStatusHash::const_iterator i = extension->queues.constBegin();
					 i != extension->queues.constEnd();
					 i++) {
					const QString queue = i.key();
					if (paused)
						if (i.value() == AgentStatusPaused)
							emit this->pauseRequest(queue, extension->channelName);
						else
							emit this->startRequest(queue, extension->channelName);
					else
						if (i.value() == AgentStatusLoggedIn)
							emit this->pauseRequest(queue, extension->channelName);
				}
			}
		}
	}
}

void ManagerWindow::agentStatusChanged(const QString& queue, const QString& channelName,
									   AstCtiAgentStatus status) {
	const int listSize = this->config->extensions.size();
	int i;
	for (i = 0; i < listSize; i++)
		if (this->config->extensions.at(i)->channelName == channelName)
			break;
	if (i == listSize)
		return;

	CtiClientWindow::agentStatusChanged(queue, channelName, status);
}

// Returns true if status is the same in all queues, otherwise false
// Paused and LoggedOut are considered to be the same for this purpose
bool ManagerWindow::setAgentStatus(const QString& queue, const QString& channelName,
								   AstCtiAgentStatus& status)
{
	bool allSet = CtiClientWindow::setAgentStatus(queue, channelName, status);

	const int listSize = this->config->extensions.size();
	for (int i = 0; i < listSize; i++) {
		AstCtiExtensionPtr extension = this->config->extensions.at(i);
		if (extension->channelName == channelName) {
			// If we are logged-in even in one queue, we consider us to be "started"
			switch (status) {
			case AgentStatusLoggedIn:
				this->ui->accountsTabWidget->widget(i)->setProperty("Paused", QVariant(false));
				break;
			case AgentStatusLoggedOut:
			case AgentStatusPaused:
				if (allSet)
					this->ui->accountsTabWidget->widget(i)->setProperty("Paused", QVariant(true));
				break;
			}

			if (i == this->ui->accountsTabWidget->currentIndex()) {
				// If we are logged-in even in one queue, we consider us to be "started"
				switch (status) {
				case AgentStatusLoggedIn:
					this->toggleStartPauseButton(this->ui->startPauseButton, false);
					break;
				case AgentStatusLoggedOut:
				case AgentStatusPaused:
					if (allSet)
						this->toggleStartPauseButton(this->ui->startPauseButton, true);
					break;
				}

				for (QHash<QToolButton*, QString>::const_iterator j =
						this->queueButtons.constBegin();
					 j != this->queueButtons.constEnd();
					 j++) {
					if (j.value() == queue) {
						switch (status) {
						case AgentStatusLoggedIn:
							this->toggleStartPauseButton(j.key(), false);
							break;
						case AgentStatusLoggedOut:
						case AgentStatusPaused:
							if (allSet)
								this->toggleStartPauseButton(j.key(), true);
							break;
						}
						break;
					}
				}
				for (int row = 0; row < this->queuesModel->rowCount(); row++) {
					QModelIndex idx = this->queuesModel->index(row, 1);
					if (this->queuesModel->data(idx).toString() == queue) {
						idx = this->queuesModel->index(row, 2);
						this->queuesModel->setData(idx,
												   CtiClientWindow::agentStatusToString(status),
												   Qt::DisplayRole);
						break;
					}
				}
			}
		}
	}

	return allSet;
}

void ManagerWindow::setStatus(bool status)
{
	CtiClientWindow::setStatus(status);

	//TODO
//    if (status) {
//		this->ui->statusLabel->setPixmap(QPixmap(QStringLiteral(":/res/res/greenled.png")));
//        this->ui->statusLabel->setToolTip(statusMsgOK);
//    } else {
//		this->ui->statusLabel->setPixmap(QPixmap(QStringLiteral(":/res/res/redled.png")));
//        this->ui->statusLabel->setToolTip(statusMsgNotOK);
//    }
}

void ManagerWindow::readSettings()
{
    QSettings settings(APP_NAME);

	settings.beginGroup("ManagerWindow." + this->config->userName);
	this->restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
	ui->centerSplitter->restoreState(
				settings.value(QStringLiteral("centerSplitterSizes")).toByteArray());
	ui->leftSplitter->restoreState(
				settings.value(QStringLiteral("leftSplitterSizes")).toByteArray());
	ui->rightSplitter->restoreState(
				settings.value(QStringLiteral("rightSplitterSizes")).toByteArray());
    settings.endGroup();
}

void ManagerWindow::writeSettings()
{
    QSettings settings(APP_NAME);

	settings.beginGroup("ManagerWindow." + this->config->userName);
	settings.setValue(QStringLiteral("geometry"), this->saveGeometry());
	settings.setValue(QStringLiteral("centerSplitterSizes"), ui->centerSplitter->saveState());
	settings.setValue(QStringLiteral("leftSplitterSizes"), ui->leftSplitter->saveState());
	settings.setValue(QStringLiteral("rightSplitterSizes"), ui->rightSplitter->saveState());
    settings.endGroup();
}

void ManagerWindow::enableControls(const bool enable)
{
	//TODO
	//	this->ui->callComboBox->setEnabled(!paused);
	//	this->ui->callButton->setEnabled(!paused);
	//	this->ui->passwordButton->setEnabled(!paused);
	//	this->ui->repeatButton->setEnabled(!paused);
	//	this->ui->pauseButton->setChecked(paused);
	//	this->ui->pauseButton->setEnabled(true);
}

void ManagerWindow::showHelp()
{
    //TODO
}

void ManagerWindow::showMaxRestore()
{
    if (this->isMaximized()) {
        this->showNormal();
    } else {
        this->showMaximized();
    }
}

void ManagerWindow::switchAccount(const int index)
{
	if (config->isCallCenter) {
		bool paused =
				this->ui->accountsTabWidget->currentWidget()->property("Paused").toBool();
		if (paused)
			this->ui->startPauseButton->setIcon(
						QIcon(QPixmap(QStringLiteral(":/res/res/agt_forward.png"))));
		else
			this->ui->startPauseButton->setIcon(
						QIcon(QPixmap(QStringLiteral(":/res/res/agt_pause-queue.png"))));

		this->ui->queuesTableView->setUpdatesEnabled(false);

		AstCtiExtensionPtr extension = this->config->extensions.at(index);
		for (QHash<QToolButton*, QString>::const_iterator i = this->queueButtons.constBegin();
			 i != this->queueButtons.constEnd();
			 i++) {
			QToolButton* const queueButton = i.key();
			const QString queue = i.value();
			const AstCtiAgentStatus status = extension->queues.value(queue);

			if (status == AgentStatusLoggedIn) {
				queueButton->setIcon(
							QIcon(QPixmap(QStringLiteral(":/res/res/agt_pause-queue.png"))));
				queueButton->setToolTip(tr("Pause"));
			} else {
				queueButton->setIcon(
							QIcon(QPixmap(QStringLiteral(":/res/res/agt_forward.png"))));
				queueButton->setToolTip(tr("Start"));
			}

			for (int row = 0; row < this->queuesModel->rowCount(); row++) {
				QModelIndex idx = this->queuesModel->index(row, 1);
				if (this->queuesModel->data(idx).toString() == queue) {
					idx = this->queuesModel->index(row, 2);
					this->queuesModel->setData(idx, CtiClientWindow::agentStatusToString(status),
											   Qt::DisplayRole);
					break;
				}
			}
		}

		this->ui->queuesTableView->setUpdatesEnabled(true);
	}
}

void ManagerWindow::startPauseButtonClicked()
{
	QToolButton* queueButton = qobject_cast<QToolButton*>(QObject::sender());
	queueButton->setEnabled(false);
	QString queue = this->queueButtons.value(queueButton);
	AstCtiExtensionPtr extension =
			this->config->extensions.at(this->ui->accountsTabWidget->currentIndex());
	AstCtiAgentStatus status = extension->queues.value(queue);

	if (status == AgentStatusLoggedIn)
		emit this->pauseRequest(queue, extension->channelName);
	else
		emit this->startRequest(queue, extension->channelName);
}

void ManagerWindow::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::WindowStateChange) {
        if (this->isMaximized()) {
			this->ui->maximizeButton->setIcon(
					QIcon(QPixmap(QStringLiteral(":/res/res/window-restore.png"))));
			this->ui->maximizeButton->setToolTip(tr("Restore"));
        } else {
			this->ui->maximizeButton->setIcon(
					QIcon(QPixmap(QStringLiteral(":/res/res/window-maximize.png"))));
			this->ui->maximizeButton->setToolTip(tr("Maximize"));
        }
    }
}

void ManagerWindow::resizeEvent(QResizeEvent* e)
{
    if (!this->isFullScreen() && !this->isMaximized()) {
		//Round corners
        QImage image(e->size(), QImage::Format_Mono);
        image.fill(1);

		//Top left
		image.setPixel(0, 0, 0);
		image.setPixel(1, 0, 0);
		image.setPixel(2, 0, 0);
		image.setPixel(3, 0, 0);
		image.setPixel(0, 1, 0);
		image.setPixel(1, 1, 0);
        image.setPixel(0, 2, 0);
        image.setPixel(0, 3, 0);

		//Top right
		image.setPixel(width() - 4, 0, 0);
		image.setPixel(width() - 3, 0, 0);
		image.setPixel(width() - 2, 0, 0);
		image.setPixel(width() - 1, 0, 0);
		image.setPixel(width() - 2, 1, 0);
		image.setPixel(width() - 1, 1, 0);
		image.setPixel(width() - 1, 2, 0);
		image.setPixel(width() - 1, 3, 0);

		//Bottom left
        image.setPixel(0, height() - 4, 0);
        image.setPixel(0, height() - 3, 0);
		image.setPixel(0, height() - 2, 0);
		image.setPixel(1, height() - 2, 0);
		image.setPixel(0, height() - 1, 0);
		image.setPixel(1, height() - 1, 0);
		image.setPixel(2, height() - 1, 0);
		image.setPixel(3, height() - 1, 0);

		//Bootom right
		image.setPixel(width() - 1, height() - 4, 0);
		image.setPixel(width() - 1, height() - 3, 0);
		image.setPixel(width() - 2, height() - 2, 0);
		image.setPixel(width() - 1, height() - 2, 0);
		image.setPixel(width() - 4, height() - 1, 0);
		image.setPixel(width() - 3, height() - 1, 0);
		image.setPixel(width() - 2, height() - 1, 0);
		image.setPixel(width() - 1, height() - 1, 0);

        this->setMask(QPixmap::fromImage(image).createMaskFromColor(Qt::black));
    } else {
        this->clearMask();
        this->resizeLeft = false;
        this->resizeRight = false;
        this->resizeUp = false;
        this->resizeDown = false;
    }

// Alternative
//    if (!this->isFullScreen() && !this->isMaximized()) {
//        //Draw window with rounded corners
//        QPixmap pixmap(e->size());
//        pixmap.fill(Qt::white);
//        QPainter painter(&pixmap);
//        painter.setRenderHint(QPainter::Antialiasing);
//        painter.setBrush(Qt::black);
//        painter.setPen(Qt::NoPen);
//        painter.drawRoundedRect(pixmap.rect(), 6, 6);
//        this->setMask(pixmap.createMaskFromColor(Qt::white));
//    } else {
//        this->clearMask();
//        this->resizeLeft = false;
//        this->resizeRight = false;
//        this->resizeUp = false;
//        this->resizeDown = false;
//    }
}

void ManagerWindow::mouseMoveEvent(QMouseEvent* e)
{
    if (this->dragOrigin != QPoint(-1, -1)) {
        int dx = e->globalX() - this->dragOrigin.x();
        int dy = e->globalY() - this->dragOrigin.y();

        QRect g = this->geometry();

        if (this->resizeLeft)
            g.setLeft(g.left() + dx);
        if (this->resizeRight)
            g.setRight(g.right() + dx);
        if (this->resizeUp)
            g.setTop(g.top() + dy);
        if (this->resizeDown)
            g.setBottom(g.bottom() + dy);

        this->setGeometry(g);

        this->dragOrigin = e->globalPos();
    } else {
        if (!this->isFullScreen() && !this->isMaximized()) {
            int x = e->x();
            int y = e->y();

            QRect r = this->rect();
            this->resizeLeft = qAbs(x - r.left()) <= borderWidth;
            this->resizeRight = qAbs(x - r.right()) <= borderWidth;
            this->resizeUp = qAbs(y - r.top()) <= borderWidth;
            this->resizeDown = qAbs(y - r.bottom()) <= borderWidth;

            if ((this->resizeLeft & this->resizeUp) || (this->resizeRight & this->resizeDown))
                this->setCursor(QCursor(Qt::SizeFDiagCursor));
            else if ((this->resizeLeft & this->resizeDown) || (this->resizeRight & this->resizeUp))
                this->setCursor(QCursor(Qt::SizeBDiagCursor));
            else if (this->resizeLeft || this->resizeRight)
                this->setCursor(QCursor(Qt::SizeHorCursor));
            else if (this->resizeUp || this->resizeDown)
                this->setCursor(QCursor(Qt::SizeVerCursor));
            else
                //By default, child widgets inherit the cursor form the parent, which means
                //that if the mouse pointer moves from border directly to child widget,
                //the cursor will keep whatever shape it had.
				//Therefore, the cursor for all DIRECT children of this window
				//(QWidgets in this case) must explicitly be set to Qt::ArrowCursor
				// in designer or constructor.
				//In designer, it appears that the cursor is already set to Qt::ArrowCursor,
				//but actually, the cursor is unset.
				//It must be set to some other value, then back to ArrowCursor.
                this->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void ManagerWindow::mousePressEvent(QMouseEvent* e)
{
    //Recheck if mouse position is within border
    int x = e->x();
    int y = e->y();

    QRect r = this->rect();
    this->resizeLeft = qAbs(x - r.left()) <= borderWidth;
    this->resizeRight = qAbs(x - r.right()) <= borderWidth;
    this->resizeUp = qAbs(y - r.top()) <= borderWidth;
    this->resizeDown = qAbs(y - r.bottom()) <= borderWidth;

    if (isValidDrag(e))
        this->dragOrigin = e->globalPos();
}

void ManagerWindow::mouseReleaseEvent(QMouseEvent* )
{
    this->dragOrigin = QPoint(-1, -1);
}

bool ManagerWindow::eventFilter(QObject* object, QEvent* e)
{
    bool accepted = false;

    QEvent::Type type = e->type();

    if (object == this->ui->captionLabel) {
		QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(e);

        if (isValidDrag(mouseEvent)) {
            this->dragOrigin = mouseEvent->globalPos();
            e->accept();
            accepted = true;
        } else if (type == QEvent::MouseButtonRelease) {
            this->dragOrigin = QPoint(-1, -1);
            e->accept();
            accepted = true;
        } else if (type == QEvent::MouseMove) {
            //TODO
            //Integrate moving with window manager
			//e.g. in Compiz, window is not moved to another desktop
			//when it reaches the egde of screen
            if (this->dragOrigin != QPoint(-1, -1)) {
                this->move(this->pos() + (mouseEvent->globalPos() - this->dragOrigin));
                this->dragOrigin = mouseEvent->globalPos();
                e->accept();
                accepted = true;
            }
        } else if (type == QEvent::MouseButtonDblClick) {
            if (mouseEvent->button() == Qt::LeftButton) {
                this->showMaxRestore();
                e->accept();
                accepted = true;
            }
        }
    }

    return accepted;
}
