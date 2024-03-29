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
#include "astcticonfiguration.h"

AstCtiConfiguration::AstCtiConfiguration(QObject* parent) : QObject(parent)
{
	QLOG_TRACE() << "Creating new AstCtiConfiguration";
}

AstCtiConfiguration::~AstCtiConfiguration()
{
	QLOG_TRACE() << "Destroying AstCtiConfiguration";

	qDeleteAll(this->operators);
	qDeleteAll(this->services);
	qDeleteAll(this->actions);
	qDeleteAll(this->seats);
}

AstCtiService* const AstCtiConfiguration::getServiceByName(const QString& serviceName)
{
	foreach (AstCtiService* service, this->services.values()) {
		if (service->getName() == serviceName)
			return service;
	}

	return 0;
}

const int AstCtiConfiguration::getPenaltyByQueueName(const QString& queueName)
{
	for (AstCtiServiceHash::const_iterator i = this->services.constBegin();
		 i != this->services.constEnd();
		 i++)
		if (i.value()->getQueueName() == queueName)
			return i.key();

	return 0;
}

AstCtiSeat* const AstCtiConfiguration::getSeatByMac(const QString& mac)
{
	foreach (AstCtiSeat* seat, this->seats.values()) {
		if (seat->getMac() == mac)
			return seat;
	}

	return 0;
}
