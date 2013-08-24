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

#include <QXmlStreamWriter>

#include "QsLog.h"
#include "db.h"
#include "astctioperator.h"

AstCtiOperator::AstCtiOperator(int id, const QString& fullName, const QString& username,
							   const QString& password, bool beginInPause, int seatID,
							   bool canMonitor, bool canAlterSpeedDials, bool canRecord,
							   QObject* parent) : QObject(parent)
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
	this->canRecord = canRecord;
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
	const QVariantTable speedDialData =
			DB::readTable("SELECT group_name,name,num,blf,order_num FROM speed_dials "
						  "WHERE id_operator=? ORDER BY group_name,order_num", params, &ok);
	if (ok) {
		const int listSize = speedDialData.size();
		for (int i = 0; i < listSize; i++) {
			const QVariantList speedDialRow = speedDialData.at(i);
			AstCtiSpeedDial* speedDial = new AstCtiSpeedDial();
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

bool AstCtiOperator::loadServices(AstCtiServiceHash* serviceList)
{
	QLOG_TRACE() << "Loading services for operator" << this->id << this->username;

	this->services.clear();

	bool ok;
	QVariantList params;
	params.append(this->id);
	const QVariantTable serviceData =
			DB::readTable("SELECT id_service,penalty FROM services_operators "
						  "WHERE id_operator=? AND enabled=true", params, &ok);
	if (ok) {
		const int listSize = serviceData.size();
		for (int i = 0; i < listSize; i++) {
			const QVariantList serviceRow = serviceData.at(i);
			const int serviceId = serviceRow.at(0).toInt();
			const int penalty = serviceRow.at(1).toInt();
			AstCtiService* service = serviceList->value(serviceId);
			if (service != 0)
				this->services.insert(service, penalty);
		}
	} else {
		QLOG_ERROR() << "Loading services failed for operator" << this->id << this->username;
	}

	return ok;
}

bool AstCtiOperator::changePassword(const QString& newPassword)
{
	QVariantList params;
	params.append(newPassword);
	params.append(this->id);
	const int result =
		  DB::executeNonQuery(QStringLiteral(
			   "UPDATE operators SET password=? WHERE id=?"), params);
	const bool ok = result > 0;
	if (ok)
		this->password = newPassword;
	else
		QLOG_ERROR() << "Changing password failed for operator" << this->id << this->username;

	return ok;
}

bool AstCtiOperator::checkPassword(const QString& password) const
{
	return password.compare(this->password) == 0;
}

int  AstCtiOperator::getId() const
{
	return this->id;
}

const QString& AstCtiOperator::getFullName() const
{
	return this->fullName;
}

const QString& AstCtiOperator::getUsername() const
{
	return this->username;
}

const QString& AstCtiOperator::getPassword() const
{
	return this->password;
}

bool AstCtiOperator::getBeginInPause() const
{
    return this->beginInPause;
}

int AstCtiOperator::getSeatId() const
{
	return this->seatId;
}

bool AstCtiOperator::getCanMonitor() const
{
	return this->canMonitor;
}

bool AstCtiOperator::isCallCenter() const
{
	//Operators that don't have associated seat are considered call-center operators
	return this->seatId == 0;
}

const AstCtiServiceRevHash& AstCtiOperator::getServices() const
{
	return this->services;
}

QString AstCtiOperator::toXml(AstCtiSeat* seat)
{
	QString xmlString;
	QXmlStreamWriter writer(&xmlString);

	writer.writeStartDocument();
	writer.writeStartElement(QStringLiteral("Operator"));
	writer.writeAttribute(QStringLiteral("FullName"), this->fullName);

	writer.writeTextElement(QStringLiteral("IsCallCenter"), QString::number(this->isCallCenter()));
	writer.writeTextElement(QStringLiteral("BeginInPause"), QString::number(this->beginInPause));
	writer.writeTextElement(QStringLiteral("CanMonitor"), QString::number(this->canMonitor));
	writer.writeTextElement(QStringLiteral("CanAlterSpeedDials"),
							QString::number(this->canAlterSpeedDials));
	writer.writeTextElement(QStringLiteral("CanRecord"), QString::number(this->canRecord));

	writer.writeStartElement(QStringLiteral("Extensions"));

	AstCtiExtensionList extensions = seat->getExtensions();
	const int listSize = extensions.size();
	for (int i = 0; i < listSize; i++) {
		AstCtiExtension* extension = extensions.at(i);
		writer.writeStartElement(QStringLiteral("Extension"));
		writer.writeAttribute(QStringLiteral("Number"), extension->number);
		writer.writeTextElement(QStringLiteral("Channel"), extension->channelName);
		writer.writeTextElement(QStringLiteral("Name"), extension->name);
		writer.writeTextElement(QStringLiteral("CanAutoAnswer"),
								QString::number(extension->canAutoAnswer));
		writer.writeTextElement(QStringLiteral("AgentStatus"),
								QString::number((int)extension->agentStatus));
		writer.writeTextElement(QStringLiteral("Status"),
								QString::number((int)extension->status));
		writer.writeEndElement(); // Extension
	}

	writer.writeEndElement(); // Extensions

	if (this->speedDials.size() > 0) {
		writer.writeStartElement(QStringLiteral("SpeedDials"));

		QMap<QString, AstCtiSpeedDial*>::const_iterator sdIterator = this->speedDials.constBegin();
		while (sdIterator != this->speedDials.constEnd()) {
			AstCtiSpeedDial* speedDial = sdIterator.value();

			writer.writeStartElement(QStringLiteral("SpeedDial"));

			writer.writeTextElement(QStringLiteral("GroupName"), speedDial->groupName);
			writer.writeTextElement(QStringLiteral("Name"), speedDial->name);
			writer.writeTextElement(QStringLiteral("Number"), speedDial->number);
			writer.writeTextElement(QStringLiteral("BLF"), QString::number(speedDial->isBlf));
			writer.writeTextElement(QStringLiteral("Order"), QString::number(speedDial->order));
			writer.writeTextElement(QStringLiteral("ExtensionStatus"),
									QString::number((int)speedDial->extensionStatus));

			writer.writeEndElement(); // SpeedDial

			sdIterator++;
		}

		writer.writeEndElement(); // SpeedDials
	}

	if (this->services.size() > 0) {
		writer.writeStartElement(QStringLiteral("Services"));

		AstCtiServiceRevHash::const_iterator servicesIterator = this->services.constBegin();
		while (servicesIterator != this->services.constEnd()) {
			AstCtiService* service = servicesIterator.key();

			writer.writeStartElement(QStringLiteral("Service"));

			writer.writeTextElement(QStringLiteral("Name"), service->getName());
			writer.writeTextElement(QStringLiteral("QueueName"), service->getQueueName());
			writer.writeTextElement(QStringLiteral("ContextType"), service->getContextTypeString());

			writer.writeEndElement(); // Service
			servicesIterator++;
		}

		writer.writeEndElement(); // Services
	}

	writer.writeEndElement(); // Operator
	writer.writeEndDocument();

	return xmlString;
}
