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

#include "qastctivariable.h"

QAstCTIVariable::QAstCTIVariable(const int &id, QObject *parent) :
        QObject(parent), idVariable(id), idService(0), varName("")
{

}

QAstCTIVariable::~QAstCTIVariable()
{
    qDebug() << "In QAstCTIVariable::~QAstCTIVariable()";
}

bool QAstCTIVariable::load()
{
    bool retVal = false;
    QSqlDatabase db = QSqlDatabase::database("mysqldb");
    if (!db.isOpen()) {
        db.open();
    }
    QSqlQuery query(db);

    QString sql = "SELECT * FROM services_variables WHERE ID_VARIABLE=:id";
    if (!query.prepare(sql)) {
        qCritical("Prepare failed in QAstCTIVariable::load() %s:%d",  __FILE__ , __LINE__);
    } else {
        query.bindValue(":id", this->idVariable);
        if (!query.exec()) {
            qCritical("Query execution failed in QAstCTIVariable::load() %s:%d",  __FILE__ , __LINE__);
        } else {
            query.first();
            this->idService = query.value(1).toInt(0);
            this->varName = query.value(2).toString();
            query.finish();
        }
    }
    query.clear();

    emit this->loadComplete(retVal);
    return retVal;
}

int QAstCTIVariable::getIdVariable()
{
    return this->idVariable;
}

int QAstCTIVariable::getIdService()
{
    return this->idService;
}

QString QAstCTIVariable::getVarName()
{
    return this->varName;
}