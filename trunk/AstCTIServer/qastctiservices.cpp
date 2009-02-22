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
 * AstCTIClient.  If you copy code from other releases into a copy of GNU
 * AstCTIClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTIClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */
#include <QtSql>

#include "qastctiservices.h"
#include "qastctiservice.h"

QAstCTIServices::QAstCTIServices()
{
    this->fillServices();
}

QAstCTIServices::~QAstCTIServices()
{
    this->clear();
}

QAstCTIService *QAstCTIServices::operator[](const QString &key)
{
    return (this->services.contains(key)) ? this->services[key] : 0;

}

void QAstCTIServices::addService(QAstCTIService *service)
{
    this->services.insert(service->getServiceName(), service);
}

void QAstCTIServices::removeService(const QString &key)
{
    if (this->services.contains(key))
    {
        QAstCTIService *ser = this->services[key];
        delete(ser);
        this->services.remove(key);
    }
}
int QAstCTIServices::count()
{
    return this->services.count();
}

void QAstCTIServices::clear()
{
    QMutableHashIterator<QString, QAstCTIService*> i(this->services);
    while (i.hasNext()) {
        i.next();
        delete(i.value());
    }
    this->services.clear();
}

void QAstCTIServices::fillServices()
{
    QSqlDatabase db = QSqlDatabase::database("sqlitedb");
    QSqlQuery query(db);
    query.exec("SELECT ID_SERVICE FROM services ORDER BY ID_SERVICE ASC");
    while(query.next())
    {
        QAstCTIService *service = new QAstCTIService(query.value(0).toInt(0));
        if (service->Load())
        {
            QString serviceName = service->getServiceName();

            // Remove service if exists before load
            this->removeService(serviceName);
            this->addService(service);
        }
    }
    query.finish();
    query.clear();
}
