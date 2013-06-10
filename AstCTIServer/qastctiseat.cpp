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
#include <QDebug>

#include "db.h"
#include "qastctiseat.h"

QAstCTISeat::QAstCTISeat(const int &id, QObject *parent)
        : QObject(parent),  idSeat(id), seatMac("00:00:00:00:00:00"),
        seatExten(""), description("")
{

}

QAstCTISeat::QAstCTISeat(const QString &mac, QObject *parent)
        : QObject(parent),  idSeat(0), seatMac(mac),
        seatExten(""), description("")
{
}

QAstCTISeat::~QAstCTISeat()
{
    qDebug() << "In QAstCTISeat::~QAstCTISeat()";
}

bool QAstCTISeat::load()
{
	bool ok;
	QVariantList params;
	params.append(this->idSeat);
	const QVariantList seatData = DB::readRow("SELECT * FROM seats WHERE ID_SEAT=?", params, &ok);
	if (ok) {
		this->seatMac = seatData.at(1).toString();
		this->seatExten  = seatData.at(2).toString();
		this->description = seatData.at(3).toString();
	}

	emit this->loadComplete(ok);
	return ok;
}

bool QAstCTISeat::loadFromMac() {
	bool ok;
	QVariantList params;
	params.append(this->seatMac);
	const QVariantList seatData = DB::readRow("SELECT * FROM seats WHERE SEAT_MAC=?", params, &ok);
	if (ok) {
		this->idSeat = seatData.at(0).toInt();
		this->seatExten  = seatData.at(2).toString();
		this->description = seatData.at(3).toString();
	}

	emit this->loadComplete(ok);
	return ok;
}

int  QAstCTISeat::getIdSeat()
{
    return this->idSeat;
}

QString  QAstCTISeat::getSeatMac()
{
    return this->seatMac;
}

QString  QAstCTISeat::getSeatExten()
{
    return this->seatExten;
}

QString  QAstCTISeat::getDescription()
{
    return this->description;
}
