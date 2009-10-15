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

QAstCTIOperatorServices::QAstCTIOperatorServices(QObject *parent)
        : QObject(parent), idOperator(0)
{
}

QAstCTIOperatorServices::QAstCTIOperatorServices(const int &idOperator, QObject *parent)
        : QObject(parent), idOperator(idOperator)
{
    this->fillList();
}

QAstCTIOperatorServices::~QAstCTIOperatorServices()
{
    this->servicesList.clear();
}

void QAstCTIOperatorServices::setIdOperator(const int &idOperator)
{
    this->idOperator = idOperator;
    this->fillList();
}

int QAstCTIOperatorServices::count()
{
    return this->servicesList.count();
}

QHash<QString,int> *QAstCTIOperatorServices::getServicesList()
{
    return &this->servicesList;
}

void QAstCTIOperatorServices::fillList()
{
    QSqlDatabase db = QSqlDatabase::database("mysqldb");
    if (!db.isOpen()) {
        db.open();
    }
    QSqlQuery query(db);

    QString sql = "SELECT s.SERVICE_NAME, s.SERVICE_IS_QUEUE, s.SERVICE_QUEUE_NAME,";
            sql +="so.PENALTY FROM services_operators so ";
            sql +="JOIN services s ON so.ID_SERVICE=s.ID_SERVICE ";
            sql +="WHERE s.SERVICE_IS_QUEUE=1 AND so.ID_OPERATOR=:id";

    if (!query.prepare(sql)) {
        QString lastError = db.lastError().text();
        qCritical("Prepare failed in QAstCTIOperatorServices::fillList() %s:%d: %s",  __FILE__ , __LINE__,
                  qPrintable(lastError)
                  );
    } else {
        query.bindValue(":id", this->idOperator);
        query.exec();

        this->servicesList.clear();
        while(query.next()) {
            QString serviceName = query.value(0).toString();
            int penalty = query.value(1).toInt(0);
            if (!this->servicesList.contains(serviceName)) {
                this->servicesList.insert(serviceName, penalty);
            }
        }
    }
    query.finish();
    query.clear();
}
