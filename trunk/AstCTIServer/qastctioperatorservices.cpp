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

#include <QtSql>
#include <QDebug>

#include "qastctioperatorservices.h"

QAstCTIOperatorServices::QAstCTIOperatorServices(QObject* parent)
        : QObject(parent), ID_OPERATOR(0)
{
}
QAstCTIOperatorServices::QAstCTIOperatorServices(const int& idoperator, QObject* parent)
        : QObject(parent), ID_OPERATOR(idoperator)
{
    this->fill_list();
}
QAstCTIOperatorServices::~QAstCTIOperatorServices()
{
    this->services_list.clear();
}

void QAstCTIOperatorServices::set_id_operator(const int &idoperator)
{
    this->ID_OPERATOR = idoperator;
    this->fill_list();
}

int QAstCTIOperatorServices::count()
{
    return this->services_list.count();
}

QHash<QString,int>* QAstCTIOperatorServices::get_services_list()
{
    return &this->services_list;
}

void QAstCTIOperatorServices::fill_list()
{
    QSqlDatabase db = QSqlDatabase::database("sqlitedb");
    QSqlQuery query(db);

    QString sql = "SELECT services.SERVICE_NAME, services.SERVICE_IS_QUEUE iq, services.SERVICE_QUEUE_NAME q,";
            sql +="services_operators.PENALTY FROM services_operators ";
            sql +="JOIN services ON services_operators.ID_SERVICE=services.ID_SERVICE ";
            sql +="WHERE iq=1 AND ID_OPERATOR=:id";
    query.prepare(sql);
    query.bindValue(":id", this->ID_OPERATOR);
    query.exec();

    this->services_list.clear();
    while(query.next())
    {
        QString service_name = query.value(0).toString();
        int penalty = query.value(1).toInt(0);
        if (!this->services_list.contains(service_name))
        {
            this->services_list.insert(service_name, penalty);
        }
    }
    query.finish();
    query.clear();
}