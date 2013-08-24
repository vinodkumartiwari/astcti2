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

#ifndef CTICONFIG_H
#define CTICONFIG_H

#include <QHash>
#include <QMap>
#include <QSharedPointer>

typedef QHash<QString, QString> QStringHash;

//	Value   State            Description
//	0	    NOT_INUSE        Channel is not in use (free)
//	1	    INUSE            One or more devices are in use
//	2	    BUSY             All devices are busy
//	4	    UNAVAILABLE      All devices are unavailable
//	8	    RINGING          One or more devices are ringing
enum ExtensionStatus {
	ExtensionStatusNotInUse = 0,
	ExtensionStatusInUse = 1,
	ExtensionStatusBusy = 2,
	ExtensionStatusUnavailable = 4,
	ExtensionStatusRinging = 8
};
Q_DECLARE_FLAGS(AstCtiExtensionStatus, ExtensionStatus)
Q_DECLARE_OPERATORS_FOR_FLAGS(AstCtiExtensionStatus)
Q_DECLARE_METATYPE(AstCtiExtensionStatus)

enum AstCtiAgentStatus {
	AgentStatusLoggedOut,
	AgentStatusLoggedIn,
	AgentStatusPaused,
	AgentStatusLoginFailed, //Agents can't actually have this status, used only for notification
	AgentStatusPauseFailed //Agents can't actually have this status, used only for notification
};
Q_DECLARE_METATYPE(AstCtiAgentStatus)

struct AstCtiExtension
{
	QString               channelName;
	QString               number;
	QString               name;
	bool                  canAutoAnswer;
	AstCtiAgentStatus     agentStatus;
	AstCtiExtensionStatus status;
};

typedef QSharedPointer<AstCtiExtension> AstCtiExtensionPtr;
typedef QList<AstCtiExtensionPtr> AstCtiExtensionPtrList;

struct AstCtiSpeedDial
{
	QString               groupName;
	QString               name;
	QString               number;
	quint8                order;
	bool                  isBlf;
	AstCtiExtensionStatus extensionStatus;
};

typedef QSharedPointer<AstCtiSpeedDial> AstCtiSpeedDialPtr;
typedef QMap<QString, AstCtiSpeedDialPtr> AstCtiSpeedDialPtrMap;

struct AstCtiConfiguration
{
	bool     debug;
	int      keepAliveInterval;
	int      compressionLevel;
	QString  serverHost;
	quint16  serverPort;
	int      connectTimeout;
	quint32  connectRetryInterval;
	QString  userName;
	QString  password;
	QString  fullName;
	bool     isCallCenter;
	bool     beginInPause;
	bool     canMonitor;
	bool     canAlterSpeedDials;
	bool     canRecord;

	AstCtiExtensionPtrList  extensions;
	AstCtiSpeedDialPtrMap   speedDials;
	QStringHash             services;
};
#endif
