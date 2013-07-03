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
#include "astctioperator.h"

AstCtiOperator::AstCtiOperator(int id, const QString &fullName, const QString &username,
								 const QString &password, bool beginInPause, int seatID,
								 QObject *parent) : QObject(parent)
{
	QLOG_TRACE() << "Creating new AstCtiOperator" << id << username << fullName;

	this->id = id;
	this->fullName = fullName;
	this->username = username;
	this->password = password;
	this->beginInPause = beginInPause;
	this->seatId = seatID;
}

AstCtiOperator::~AstCtiOperator()
{
	QLOG_TRACE() << "Destroying AstCtiOperator" << this->id << this->username << this->fullName;
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
	//Opeartors that don't have associated seat are considered call-center operators
	return this->seatId == 0;
}

QHash<AstCtiService*, int> *AstCtiOperator::getServices()
{
	return &(this->services);
}
