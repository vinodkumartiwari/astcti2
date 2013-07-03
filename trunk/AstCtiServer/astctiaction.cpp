/* Copyright (C) 2007-2009 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
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
#include "astctiaction.h"

AstCtiAction::AstCtiAction(const int &id, const QString &osType, const QString &actionType,
						   const QString &destination, const QString &parameters,
						   const QString &messageEncoding, QObject *parent) :	QObject(parent)
{
	QLOG_TRACE() << "Creating new AstCtiAction" << id << actionType << destination;

	this->id = id;
	this->osType = parseOsType(osType);
	this->actionType = parseActionType(actionType);
	this->destination = destination;
	this->parameters = parameters;
	this->messageEncoding = messageEncoding;
}

AstCtiAction::~AstCtiAction()
{
	QLOG_TRACE() << "Destroying AstCtiAction" << this->id << getActionName(this->actionType)
				 << this->destination;
}

QString AstCtiAction::getActionName(const AstCtiActionType actionType)
{
	switch(actionType) {
	case ActionApplication:
		return "Application";
	case ActionBrowser:
		return "Browser";
	case ActionInternalBrowser:
		return "InternalBrowser";
	case ActionTcpMessage:
		return "TcpMessage";
	default:
		return "UdpMessage";
	}
}

AstCtiActionType AstCtiAction::parseActionType(const QString &actionTypeString) {
	if (actionTypeString == "Application")
		return ActionApplication;
	else if (actionTypeString == "Browser")
		return ActionBrowser;
	else if (actionTypeString == "InternalBrowser")
		return ActionInternalBrowser;
	else if (actionTypeString == "TcpMessage")
		return ActionTcpMessage;
	else
		return ActionUdpMessage;
}

AstCtiActionOsType AstCtiAction::parseOsType(const QString &osTypeString) {
	if (osTypeString == "Linux")
		return ActionOsLinux;
	else if (osTypeString == "Macintosh")
		return ActionOsMacintosh;
	else if (osTypeString == "Windows")
		return ActionOsWindows;
	else
		return ActionOsAll;
}

int AstCtiAction::getId()
{
	return this->id;
}

AstCtiActionType AstCtiAction::getActionType()
{
    return this->actionType;
}

AstCtiActionOsType AstCtiAction::getOsType()
{
	return this->osType;
}

QString AstCtiAction::getDestination()
{
	return this->destination;
}

QString AstCtiAction::getParameters()
{
	return this->parameters;
}

void AstCtiAction::setParameters(const QString &newParameters)
{
	this->parameters = newParameters;
}

QString AstCtiAction::getMessageEncoding() {
	return this->messageEncoding;
}
