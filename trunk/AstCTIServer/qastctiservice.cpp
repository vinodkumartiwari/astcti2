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

#include "qastctiservice.h"

QAstCTIService::QAstCTIService(const int &id)
        : ID_SERVICE(id), SERVICE_NAME(""), SERVICE_CONTEXT_TYPE(""),
        SERVICE_IS_QUEUE(false), SERVICE_QUEUE_NAME(""),
        SERVICE_TRIGGER_TYPE(""), ENABLED(false)
{

}

QAstCTIService::~QAstCTIService()
{
    qDebug() << "Destroying QAstCTIService";

}

bool QAstCTIService::Load()
{
    bool retVal = false;
    QSqlDatabase db = QSqlDatabase::database("sqlitedb");
    QSqlQuery query(db);
    query.prepare("SELECT * FROM services WHERE ID_SERVICE=:id");
    query.bindValue(":id", this->ID_SERVICE);
    retVal = query.exec();
    if ( (retVal) &  (query.first()) )
    {
        this->SERVICE_NAME = query.value(1).toString();
        this->SERVICE_CONTEXT_TYPE = query.value(2).toString();
        this->SERVICE_IS_QUEUE = query.value(3).toBool();
        this->SERVICE_QUEUE_NAME = query.value(4).toString();
        this->SERVICE_TRIGGER_TYPE = query.value(5).toString();
        this->ENABLED = query.value(6).toBool();

        query.finish();
    }
    query.clear();

    emit this->LoadComplete(retVal);
    return retVal;
}

int QAstCTIService::getIdService()
{
    return this->ID_SERVICE;
}

QString QAstCTIService::getServiceName()
{
    return this->SERVICE_NAME;
}

QString QAstCTIService::getServiceContextType()
{
    return this->SERVICE_CONTEXT_TYPE;
}

bool    QAstCTIService::getServiceIsQueue()
{
    return this->SERVICE_IS_QUEUE;
}

QString QAstCTIService::getServiceQueueName()
{
    return this->SERVICE_QUEUE_NAME;
}

QString QAstCTIService::getServiceTriggerType()
{
    return this->SERVICE_TRIGGER_TYPE;
}

bool    QAstCTIService::getEnabled()
{
    return this->ENABLED;
}
