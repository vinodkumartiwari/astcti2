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
#include "db.h"
#include "astctiservice.h"

AstCtiService::AstCtiService(const int id, const QString& name, const QString& contextType,
							   const QString& queueName, QObject* parent) : QObject(parent)
{
	QLOG_TRACE() << "Creating new AstCtiService" << id << name;

	this->id = id;
	this->name = name;
	this->contextType =
			contextType == QStringLiteral("INBOUND") ? ServiceTypeInbound : ServiceTypeOutbound;
	this->queueName = queueName;
}

AstCtiService::~AstCtiService()
{
	QLOG_TRACE() << "Destroying AstCtiService" << this->id << this->name;
}

bool AstCtiService::loadVariables()
{
	QLOG_TRACE() << "Loading variables for service" << this->id << this->name;

	this->variables.clear();

	bool ok;
	QVariantList params;
	params.append(this->id);
	const QVariantList variableData =
			DB::readList("SELECT varname FROM services_variables "
						 "WHERE id_service=? AND enabled=true", params, &ok);
	if (ok) {
		const int listSize = variableData.size();
		for (int i = 0; i < listSize; i++) {
			this->variables.append(variableData.at(i).toString());
		}
	} else {
		QLOG_ERROR() << "Loading variables failed for service" << this->id << this->name;
	}

	return ok;
}

bool AstCtiService::loadActions(AstCtiActionHash* actionList)
{
	QLOG_TRACE() << "Loading actions for service" << this->id << this->name;

	this->actions.clear();

	bool ok;
	QVariantList params;
	params.append(this->id);
	const QVariantTable actionData =
			DB::readTable("SELECT id_action,action_order FROM services_actions "
						  "WHERE id_service=? AND enabled=true "
						  "ORDER BY action_order ASC, id_action ASC", params, &ok);
	if (ok) {
		const int listSize = actionData.size();
		for (int i = 0; i < listSize; i++) {
			const QVariantList actionRow = actionData.at(i);
			const int actionId = actionRow.at(0).toInt();
			const int actionOrder = actionRow.at(1).toInt();
			AstCtiAction* action = actionList->value(actionId);
			if (action != 0)
				//There may be actions with same order number, so we use insertMulti()
				this->actions.insertMulti(actionOrder, action);
		}
	} else {
		QLOG_ERROR() << "Loading actions failed for service" << this->id << this->name;
	}

	return ok;
}

int AstCtiService::getId() const
{
	return this->id;
}

const QString& AstCtiService::getName() const
{
    return this->name;
}

AstCtiServiceType AstCtiService::getContextType() const
{
    return this->contextType;
}

QString AstCtiService::getContextTypeString() const
{
	//We use a variable to exploit NRVO
	QString typeName;

	switch (this->contextType) {
	case ServiceTypeInbound:
		typeName = QStringLiteral("Inbound");
	default: // ServiceTypeOutbound
		typeName = QStringLiteral("Outbound");
	}

	return typeName;
}

bool AstCtiService::isQueue() const
{
	return !this->queueName.isEmpty();
}

const QString& AstCtiService::getQueueName() const
{
    return this->queueName;
}

const QStringList& AstCtiService::getVariables() const
{
	return this->variables;
}

const AstCtiActionMap& AstCtiService::getActions() const
{
	return this->actions;
}
