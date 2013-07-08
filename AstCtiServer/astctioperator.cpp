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

#include <QDomDocument>

#include "QsLog.h"
#include "db.h"
#include "astctioperator.h"

AstCtiOperator::AstCtiOperator(int id, const QString &fullName, const QString &username,
							   const QString &password, bool beginInPause, int seatID,
							   bool canMonitor, bool canAlterSpeedDials,
							   QObject *parent) : QObject(parent)
{
	QLOG_TRACE() << "Creating new AstCtiOperator" << id << username << fullName;

	this->id = id;
	this->fullName = fullName;
	this->username = username;
	this->password = password;
	this->beginInPause = beginInPause;
	this->seatId = seatID;
	this->canMonitor = canMonitor;
	this->canAlterSpeedDials = canAlterSpeedDials;
}

AstCtiOperator::~AstCtiOperator()
{
	QLOG_TRACE() << "Destroying AstCtiOperator" << this->id << this->username << this->fullName;

	qDeleteAll(this->speedDials);
}

bool AstCtiOperator::loadSpeedDials()
{
	QLOG_TRACE() << "Loading speed dials for operator" << this->id << this->username;

	this->speedDials.clear();

	bool ok;
	QVariantList params;
	params.append(this->id);
	const QList<QVariantList> speedDialData =
			DB::readTable("SELECT GROUP_NAME,NAME,NUM,BLF,ORDER_NUM FROM speeddials "
						  "WHERE ID_OPERATOR=? ORDER BY GROUP_NAME,ORDER_NUM", params, &ok);
	if (ok) {
		const int listSize = speedDialData.size();
		for (int i = 0; i < listSize; i++) {
			const QVariantList speedDialRow = speedDialData.at(i);
			AstCtiSpeedDial *speedDial = new AstCtiSpeedDial();
			const QString groupName = speedDialRow.at(0).toString();
			const quint8 orderNum = speedDialRow.at(4).value<quint8>();
			speedDial->groupName = groupName;
			speedDial->name = speedDialRow.at(1).toString();
			speedDial->number = speedDialRow.at(2).toString();
			speedDial->isBlf = speedDialRow.at(3).toBool();
			speedDial->order = orderNum;
			// For key, we use groupName:order, with order left padded with zeros
			const QString key = QString("%1:%2").arg(groupName).arg(orderNum, 3, 10, QChar('0'));
			this->speedDials.insert(key, speedDial);
		}
	} else {
		QLOG_ERROR() << "Loading speed dials failed for operator" << this->id << this->username;
	}

	return ok;
}

bool AstCtiOperator::loadServices(QHash<int, AstCtiService*> *serviceList)
{
	QLOG_TRACE() << "Loading services for operator" << this->id << this->username;

	this->services.clear();

	bool ok;
	QVariantList params;
	params.append(this->id);
	const QList<QVariantList> serviceData =
			DB::readTable("SELECT ID_SERVICE,PENALTY FROM services_operators "
						  "WHERE ID_OPERATOR=? AND ENABLED=1", params, &ok);
	if (ok) {
		const int listSize = serviceData.size();
		for (int i = 0; i < listSize; i++) {
			const QVariantList serviceRow = serviceData.at(i);
			const int serviceId = serviceRow.at(0).toInt();
			const int penalty = serviceRow.at(1).toInt();
			AstCtiService *service = serviceList->value(serviceId);
			if (service != 0)
				this->services.insert(service, penalty);
		}
	} else {
		QLOG_ERROR() << "Loading services failed for operator" << this->id << this->username;
	}

	return ok;
}

bool AstCtiOperator::changePassword(QString &newPassword)
{
	QVariantList params;
	params.append(newPassword);
	params.append(this->id);
	const int result =
		  DB::executeNonQuery("UPDATE operators SET PASS_WORD=? WHERE ID_OPERATOR=?", params);
	const bool ok = result > 0;
	if (ok)
		this->password = newPassword;
	else
		QLOG_ERROR() << "Changing password failed for operator" << this->id << this->username;

	return ok;
}

bool AstCtiOperator::checkPassword(const QString &password)
{
	return password.compare(this->password) == 0;
}

int  AstCtiOperator::getId()
{
	return this->id;
}

QString  AstCtiOperator::getFullName()
{
	return this->fullName;
}

QString  AstCtiOperator::getUsername()
{
	return this->username;
}

QString  AstCtiOperator::getPassword()
{
	return this->password;
}

bool AstCtiOperator::getBeginInPause()
{
    return this->beginInPause;
}

int AstCtiOperator::getSeatId()
{
	return this->seatId;
}

bool AstCtiOperator::isCallCenter()
{
	//Operators that don't have associated seat are considered call-center operators
	return this->seatId == 0;
}

QHash<AstCtiService*, int> *AstCtiOperator::getServices()
{
	return &(this->services);
}

QString AstCtiOperator::toXml()
{
	QDomDocument xmlDoc("AstCtiOperator");

	QDomElement xmlOperator = xmlDoc.createElement("Operator");
	xmlOperator.setAttribute("FullName", this->fullName);
	xmlDoc.appendChild(xmlOperator);

	QDomElement xmlElement;
	QDomText xmlText;

	xmlElement = xmlDoc.createElement("IsCallCenter");
	xmlOperator.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(QString::number(this->isCallCenter()));
	xmlElement.appendChild(xmlText);

	xmlElement = xmlDoc.createElement("BeginInPause");
	xmlOperator.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(QString::number(this->beginInPause));
	xmlElement.appendChild(xmlText);

	xmlElement = xmlDoc.createElement("CanMonitor");
	xmlOperator.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(QString::number(this->canMonitor));
	xmlElement.appendChild(xmlText);

	xmlElement = xmlDoc.createElement("CanAlterSpeedDials");
	xmlOperator.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(QString::number(this->canAlterSpeedDials));
	xmlElement.appendChild(xmlText);

	if (this->speedDials->size() > 0) {
		QDomElement xmlSpeedDials = xmlDoc.createElement("SpeedDials");
		xmlSpeedDials.setAttribute("Count", this->speedDials->size());
		xmlOperator.appendChild(xmlSpeedDials);

		QString lastGroup = "";
		QDomElement xmlSpeedDialGroup;

		QMap<QString, AstCtiSpeedDial*>::const_iterator sdIterator = this->speedDials.constBegin();
		while (sdIterator != this->speedDials.constEnd()) {
			AstCtiSpeedDial *speedDial = sdIterator.value();

			if (lastGroup != speedDial->groupName) {
				lastGroup = speedDial->groupName;
				xmlSpeedDialGroup = xmlDoc.createElement("SpeedDialGroup");
				xmlSpeedDialGroup.setAttribute("Name", lastGroup);
				xmlSpeedDials.appendChild(xmlSpeedDialGroup);
			}

			QDomElement xmlSpeedDial = xmlDoc.createElement("SpeedDial");

			xmlElement = xmlDoc.createElement("Name");
			xmlSpeedDial.appendChild(xmlElement);
			xmlText = xmlDoc.createTextNode(speedDial->name);
			xmlElement.appendChild(xmlText);

			xmlElement = xmlDoc.createElement("Number");
			xmlSpeedDial.appendChild(xmlElement);
			xmlText = xmlDoc.createTextNode(speedDial->number);
			xmlElement.appendChild(xmlText);

			xmlElement = xmlDoc.createElement("BLF");
			xmlSpeedDial.appendChild(xmlElement);
			xmlText = xmlDoc.createTextNode(QString::number(speedDial->isBlf));
			xmlElement.appendChild(xmlText);

			xmlElement = xmlDoc.createElement("Order");
			xmlSpeedDial.appendChild(xmlElement);
			xmlText = xmlDoc.createTextNode(speedDial->order);
			xmlElement.appendChild(xmlText);

			xmlSpeedDialGroup.appendChild(xmlSpeedDial);

			sdIterator++;
		}
	}

	 if (this->services.size() > 0) {
		QDomElement xmlServices = xmlDoc.createElement("Services");
		xmlServices.setAttribute("Count", this->services.size());

		QMap<int, AstCtiAction*>::const_iterator servicesIterator = this->services->constBegin();
		while (servicesIterator != this->services->constEnd()) {
			AstCtiService *service = servicesIterator.key();
			QDomElement xmlService = xmlDoc.createElement("Service");

			xmlElement = xmlDoc.createElement("Name");
			xmlService.appendChild(xmlElement);
			xmlText = xmlDoc.createTextNode(service->getName());
			xmlElement.appendChild(xmlText);

			xmlElement = xmlDoc.createElement("QueueName");
			xmlService.appendChild(xmlElement);
			xmlText = xmlDoc.createTextNode(service->getQueueName());
			xmlElement.appendChild(xmlText);

			xmlElement = xmlDoc.createElement("ContextType");
			xmlService.appendChild(xmlElement);
			xmlText = xmlDoc.createTextNode(service->getContextTypeString());
			xmlElement.appendChild(xmlText);

			xmlServices.appendChild(xmlService);
			servicesIterator++;
		}

		xmlOperator.appendChild(xmlServices);
	}

	return xmlDoc.toString();
}
