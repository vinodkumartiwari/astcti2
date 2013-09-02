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

#include <QMessageBox>

#include "cticlientwindow.h"

CtiClientWindow::CtiClientWindow(AstCtiConfiguration* config) :
    QWidget()
{
	this->statusMsgOK = tr("Conection to server OK");
	this->statusMsgNotOK = tr("Conection to server has been lost. Trying to reconnect...");
	this->pauseErrorMsg = tr("There was an error with pause operation:");

	this->newConfig(config);

	this->canClose = false;
    this->dragOrigin = QPoint(-1, -1);

    this->createTrayIcon();
}

CtiClientWindow::~CtiClientWindow()
{
	delete this->trayIcon;

	qDeleteAll(this->activeChannels);
}

void CtiClientWindow::closeEvent(QCloseEvent* e)
{
    if (this->canClose) {
		this->writeSettings();
        e->accept();
    } else {
        e->ignore();
    }
}

bool CtiClientWindow::isValidDrag(QMouseEvent* const mouseEvent) const
{
    if (!mouseEvent || mouseEvent->modifiers() != Qt::NoModifier)
        return false;

	if (mouseEvent->type() == QEvent::MouseButtonPress &&
		mouseEvent->button() == Qt::LeftButton)
        return true;
    else
        return false;
}

void CtiClientWindow::createTrayIcon()
{
     QIcon icon;
	 icon.addPixmap(QPixmap(QStringLiteral(":/res/res/asteriskcti16x16.png")),
					QIcon::Normal, QIcon::Off);

     this->trayIcon = new QSystemTrayIcon(icon, this);
	 this->trayIcon->setToolTip(QString(APP_NAME) + tr(" client ") + QString(APP_VERSION_LONG));
     this->trayIcon->show();
}

// Returns true if status is the same in all queues, otherwise false
// Paused and LoggedOut are considered to be the same for this purpose
bool CtiClientWindow::setAgentStatus(const QString &queue, const QString &channelName,
									 AstCtiAgentStatus& status)
{
	bool allSet = true;


	AstCtiExtensionPtr extension;
	const int listSize = this->config->extensions.size();
	for (int i = 0; i < listSize; i++) {
		if (this->config->extensions.at(i)->channelName == channelName) {
			extension = this->config->extensions.at(i);
			if (status == AgentStatusLoginFailed || status == AgentStatusPauseFailed)
				status = extension->queues.value(queue);
			break;
		}
	}

	if (!extension.isNull()) {
		for (AstCtiAgentStatusHash::iterator i = extension->queues.begin();
			 i != extension->queues.end();
			 i++) {
			if (i.key() == queue) {
				i.value() = status;
			} else {
				if (status == AgentStatusLoggedIn) {
					if (i.value() != status)
						allSet = false;
				} else {
					if (i.value() != AgentStatusLoggedOut && i.value() != AgentStatusPaused)
						allSet = false;
				}
			}
		}
	}

	return allSet;
}

void CtiClientWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        this->isVisible() ? this->hide() : this->show();
        break;
    default:
        ;
    }
}

void CtiClientWindow::showMessage(const QString& message, QSystemTrayIcon::MessageIcon severity)
{
    if (this->trayIcon->supportsMessages()) {
        this->trayIcon->showMessage(APP_NAME, message, severity, 50000);
    } else {
        switch (severity) {
        case QSystemTrayIcon::NoIcon:
        case QSystemTrayIcon::Information:
            QMessageBox::information(this, APP_NAME, message);
            break;
        case QSystemTrayIcon::Warning:
            QMessageBox::warning(this, APP_NAME, message);
            break;
        case QSystemTrayIcon::Critical:
            QMessageBox::critical(this, APP_NAME, message);
            break;
        }
    }
}

void CtiClientWindow::newConfig(AstCtiConfiguration* config)
{
	this->config = config;

	//TODO
}

void CtiClientWindow::setStatus(bool status)
{
	this->enableControls(status);

	if (status) {
        //Hide the message
        if (this->trayIcon->supportsMessages())
            this->trayIcon->showMessage("", "", QSystemTrayIcon::NoIcon, 1);
    } else {
		this->showMessage(statusMsgNotOK, QSystemTrayIcon::Critical);
    }
}

void CtiClientWindow::agentStatusChanged(const QString &queue, const QString &channelName,
										 AstCtiAgentStatus status)
{
	switch (status) {
	case AgentStatusLoggedOut:
		// This should not happen
		this->showMessage(tr("Agent logged out from queue '%1'.").arg(queue),
						  QSystemTrayIcon::Warning);
		break;
	case AgentStatusLoggedIn:
		this->showMessage(tr("Agent logged in successfuly."), QSystemTrayIcon::Information);
		break;
	case AgentStatusPaused:
		this->showMessage(tr("Agent paused successfully."), QSystemTrayIcon::Information);
		break;
	case AgentStatusLoginFailed:
		this->showMessage(tr("Agent log-in to queue '%1' failed due to server error.").arg(queue),
						  QSystemTrayIcon::Critical);
		break;
	case AgentStatusPauseFailed:
		this->showMessage(tr("Pause request in queue '%1' failed due to server error.").arg(queue),
						  QSystemTrayIcon::Critical);
		break;
	}
	this->setAgentStatus(queue, channelName, status);
}

void CtiClientWindow::toggleStartPauseButton(QAbstractButton * const button, const bool paused)
{
	if (paused) {
		button->setToolTip(tr("Start"));
		button->setIcon(QIcon(QPixmap(QStringLiteral(":/res/res/agt_forward.png"))));
	} else {
		button->setToolTip(tr("Pause"));
		button->setIcon(QIcon(QPixmap(QStringLiteral(":/res/res/agt_pause-queue.png"))));
	}
	button->setEnabled(true);
}

void CtiClientWindow::about()
{
	QString serviceData = QStringLiteral("");

	for (QStringHash::const_iterator i = this->config->services.constBegin();
		 i != this->config->services.constEnd();
		 i++) {
		const QString serviceName = i.key();
		QString serviceType = i.value();
		if (serviceType == QStringLiteral("Inbound"))
			serviceType = tr("Inbound");
		else if (serviceType == QStringLiteral("Outbound"))
			serviceType = tr("Outbound");
		else if (serviceType == QStringLiteral("Queue"))
			serviceType = tr("Queue");
		serviceData.append(QString("<b>%1</b> - %2").arg(serviceName).arg(serviceType));
		if (i.value() == QStringLiteral("Queue")) {
			AstCtiAgentStatus status = this->config->extensions.at(0)->queues.value(serviceName);
			serviceData.append(QString(" (%1)").arg(agentStatusToString(status)));
		}
		serviceData.append(QStringLiteral("<br>"));
	}

	AboutDialog aboutDialog(serviceData);
    aboutDialog.exec();
}

void CtiClientWindow::minimizeToTray()
{
    this->hide();
}

void CtiClientWindow::placeCall()
{
    //TODO
}

void CtiClientWindow::quit(bool skipCheck)
{
    if (!skipCheck) {
        QMessageBox msgBox;

		msgBox.setText(tr("Are you sure you want to quit?"));
		QPushButton* yesBtn = msgBox.addButton(tr("&Yes"), QMessageBox::YesRole);
		yesBtn->setIcon(QIcon(QPixmap(QStringLiteral(":/res/res/ok.png"))));
		QPushButton* noBtn = msgBox.addButton(tr("&No"), QMessageBox::NoRole);
		noBtn->setIcon(QIcon(QPixmap(QStringLiteral(":/res/res/cancel.png"))));

        msgBox.setDefaultButton(noBtn);
        msgBox.setIcon(QMessageBox::Question);

        msgBox.exec();
        if (msgBox.clickedButton() == yesBtn)
            this->canClose = true;
    } else {
        this->canClose = true;
    }

    if (this->canClose) {
		emit this->logOff();

        this->close();
    }
}

QString CtiClientWindow::agentStatusToString(const AstCtiAgentStatus status)
{
	//We use a variable to exploit NRVO
	QString result;

	switch (status) {
	case AgentStatusLoggedIn:
		result = tr("Started");
		break;
	case AgentStatusLoggedOut:
		result = tr("Stopped");
		break;
	case AgentStatusPaused:
		result = tr("Paused");
		break;
	default:
		result = QStringLiteral("");
		break;
	}

	return result;
}
