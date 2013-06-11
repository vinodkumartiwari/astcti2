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
 * AstCTIServer.  If you copy code from other releases into a copy of GNU
 * AstCTIServer, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTIServer, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include "db.h"
#include "astctiservice.h"

AstCtiService::AstCtiService(const int &id, const QString &name, const QString &contextType,
							   const QString &queueName, QObject *parent) : QObject(parent)
{
	this->serviceId = id;
	this->name = name;
	this->contextType = contextType == "INBOUND" ? ServiceTypeInbound : ServiceTypeOutbound;
	this->queueName = queueName;
}

AstCtiService::~AstCtiService()
{
}

bool AstCtiService::loadVariables()
{
	this->variables.clear();

	bool ok;
	QVariantList params;
	params.append(this->serviceId);
	const QVariantList variableData =
			DB::readList("SELECT VARNAME FROM services_variables WHERE ID_SERVICE=?", params, &ok);
	if (ok) {
		const int listSize = variableData.size();
		for (int i = 0; i < listSize; i++) {
			this->variables.append(variableData.at(i).toString());
		}
	}

	return ok;
}

bool AstCtiService::loadActions(QHash<int, AstCtiAction*> *actionList)
{
	this->actions.clear();

	bool ok;
	QVariantList params;
	params.append(this->serviceId);
	const QList<QVariantList> actionData =
			DB::readTable("SELECT ID_ACTION,ACTION_ORDER FROM services_actions WHERE ID_SERVICE=? "
						  "ORDER BY ACTION_ORDER ASC, ID_ACTION ASC", params, &ok);
	if (ok) {
		const int listSize = actionData.size();
		for (int i = 0; i < listSize; i++) {
			const QVariantList actionRow = actionData.at(i);
			const int actionId = actionRow.at(0).toInt();
			const int actionOrder = actionRow.at(1).toInt();
			AstCtiAction *action = actionList->value(actionId);
			if (action != 0) {
				//There may be actions with same order number, so we use insertMulti()
				this->actions.insertMulti(actionOrder, action);
			}
		}
	}

	return ok;
}

int AstCtiService::getId()
{
    return this->serviceId;
}

QString AstCtiService::getName()
{
    return this->name;
}

AstCtiServiceType AstCtiService::getContextType()
{
    return this->contextType;
}

bool AstCtiService::getServiceIsQueue()
{
	return !this->queueName.isEmpty();
}

QString AstCtiService::getQueueName()
{
    return this->queueName;
}

bool AstCtiService::hasVariable(const QString &variableName)
{
	return this->variables.contains(variableName);
}

QMap<int, AstCtiAction*> *AstCtiService::getActions()
{
    return &this->actions;
}
