/* Copyright (C) 2007-2013 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * Copyright (C) 2007-2013 Lumiss d.o.o.
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
 * AstCtiServer.  If you copy code from other releases into a copy of GNU
 * AstCtiServer, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCtiServer, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include "QsLog.h"
#include "amicommand.h"

AmiCommand::AmiCommand(AmiAction action, QObject* parent) : QObject(parent)
{
	QLOG_TRACE() << "Creating new AMI command" << getActionName(action);

	this->action = action;
	this->channelName = "";
	this->parameters = 0;
	this->variables = 0;
}

AmiCommand::~AmiCommand()
{
	QLOG_TRACE() << "Destroying AMI command" << getActionName(this->action);

	delete this->parameters;
	delete this->variables;
}

QString AmiCommand::getParameter(const QString& name) const
{
	return this->parameters->value(name);
}

void AmiCommand::addParameter(const QString& name, const QString& value)
{
	if (this->parameters == 0)
		this->parameters = new QStringHash;

	this->parameters->insert(name, value);
}

void AmiCommand::addVariable(const QString& name, const QString& value)
{
	if (this->variables == 0)
		this->variables = new QStringHash;

	this->variables->insert(name, value);
}

QString AmiCommand::getActionName(const AmiAction action)
{
	//We use a variable to exploit NRVO
	QString actionName;

	switch (action) {
	case AmiActionLogin:
		actionName = QStringLiteral("Login");
		break;
	case AmiActionLogoff:
		actionName = QStringLiteral("Logoff");
		break;
	case AmiActionOriginate:
		actionName = QStringLiteral("Originate");
		break;
	case AmiActionQueueAdd:
		actionName = QStringLiteral("QueueAdd");
		break;
	case AmiActionQueuePause:
		actionName = QStringLiteral("QueuePause");
		break;
	case AmiActionQueueRemove:
		actionName = QStringLiteral("QueueRemove");
		break;
	case AmiActionSipShowPeer:
		actionName = QStringLiteral("SIPshowpeer");
		break;
	case AmiActionExtensionState:
		actionName = QStringLiteral("ExtensionState");
		break;
	case AmiActionCoreShowChannels:
		actionName = QStringLiteral("CoreShowChannels");
		break;
	default:
		actionName = QStringLiteral("");
		break;
	}

	return actionName;
}

QString AmiCommand::toString(const int actionId) const
{
	QString data = QStringLiteral("");

	//Define action
	data.append(QString("Action:%1\r\n").arg(AmiCommand::getActionName(this->action)));
	//Add action ID
	data.append(QString("ActionId:%1\r\n").arg(actionId));
	//Add parameters, if any
	if (this->parameters != 0) {
		QHashIterator<QString, QString> i(*this->parameters);
		while (i.hasNext()) {
			i.next();
			data.append(QString("%1:%2\r\n").arg(i.key()).arg(i.value()));
		}
	}
	//Add variables, if any
	if (this->variables != 0) {
		QHashIterator<QString, QString> i(*this->variables);
		while (i.hasNext()) {
			i.next();
			data.append(QString("Variable:%1=%2\r\n").arg(i.key()).arg(i.value()));
		}
	}
	//End command
	data.append(QStringLiteral("\r\n"));

	return data;
}
