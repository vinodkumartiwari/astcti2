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

#ifndef CTICLIENTWINDOW_H
#define CTICLIENTWINDOW_H

#include <QtWidgets/QWidget>
#include <QSystemTrayIcon>
#include <QHash>

#include "coreconstants.h"
#include "cticonfig.h"
#include "astctichannel.h"
#include "aboutdialog.h"
#include "browserwindow.h"

class CtiClientWindow : public QWidget
{
    Q_OBJECT

public:
	explicit CtiClientWindow(AstCtiConfiguration* config);
	~CtiClientWindow();

public slots:
	// Signals from UI
	virtual void about();
	virtual void minimizeToTray();
	virtual void placeCall();
	virtual void agentStartPause() = 0;
	virtual void quit(bool skipCheck);

	// Signals from CtiClientApplication
	virtual void showMessage(const QString& message, QSystemTrayIcon::MessageIcon severity);
	virtual void setStatus(bool status);
	virtual void newConfig(AstCtiConfiguration* config);
	virtual void agentStatusChanged(const QString& queue, const QString& channelName,
									AstCtiAgentStatus status);
	virtual void handleChannelEvent(AstCtiChannel* channel) = 0;

signals:
	void startRequest(const QString& queue, const QString& channelName);
	void pauseRequest(const QString& queue, const QString& channelName);
	void changePassword();
    void logOff();

protected:
	virtual bool isValidDrag(QMouseEvent* const mouseEvent) const;
	virtual void createTrayIcon();
	virtual bool setAgentStatus(const QString& queue, const QString& channelName,
								AstCtiAgentStatus& status);
	virtual void connectSlots() = 0;
	virtual void enableControls(const bool enable) = 0;
	virtual void writeSettings() = 0;
	virtual void readSettings() = 0;
	virtual void toggleStartPauseButton(QAbstractButton* const button, const bool paused);

	static QString agentStatusToString(const AstCtiAgentStatus status);

	void         closeEvent(QCloseEvent* e);

	QString              statusMsgOK;
	QString              statusMsgNotOK;
	QString              pauseErrorMsg;

	bool                 canClose;
	QPoint               dragOrigin;
	AstCtiChannelHash    activeChannels;
	QSystemTrayIcon*     trayIcon;
	AstCtiConfiguration* config;

protected slots:
	virtual void iconActivated(const QSystemTrayIcon::ActivationReason reason);
};

#endif // CTICLIENTWINDOW_H
