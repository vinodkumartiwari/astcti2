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

#include "qastctiservicesoperators.h"
#include "qastctiservice.h"


QAstCTIService::QAstCTIService(const int &id, QAstCTIActions *theActionsList, QObject *parent)
        : QObject(parent), idService(id), serviceName(""), serviceContextType(""),
        serviceIsQueue(false), serviceQueueName(""),  enabled(false)
{
    this->actionsList = theActionsList;

    this->operators = new QAstCTIServicesOperators(this);    
    this->variables = new QAstCTIServicesVariables(this);
    connect(this, SIGNAL(loadComplete(const bool&)), this, SLOT(loadActions(const bool&)));
    connect(this, SIGNAL(loadComplete(const bool&)), this, SLOT(loadOperators(const bool&)));
    connect(this, SIGNAL(loadComplete(const bool&)), this, SLOT(loadVariables(const bool&)));    

}

QAstCTIService::~QAstCTIService()
{
    qDebug() << "In QAstCTIService::~QAstCTIService()";

}

bool QAstCTIService::load()
{
    bool retVal = false;
    QSqlDatabase db = QSqlDatabase::database("mysqldb");
    if (!db.isOpen()) {
        db.open();
    }
    QSqlQuery query(db);
    QString sql = "SELECT * FROM services WHERE ID_SERVICE=:id";

    if (!query.prepare(sql)) {
        qCritical("Prepare failed in QAstCTIService::load() %s:%d",  __FILE__ , __LINE__);
    } else {
        query.bindValue(":id", this->idService);
        if (!query.exec()) {
            qCritical("Query execution failed in QAstCTIService::load() %s:%d",  __FILE__ , __LINE__);
            return false;
        } else {
            retVal = query.first();
            this->serviceName = query.value(1).toString();
            this->serviceContextType= query.value(2).toString();
            this->serviceIsQueue = query.value(3).toBool();
            this->serviceQueueName = query.value(4).toString();
            this->enabled = query.value(5).toBool();
            query.finish();
        }
    }
    query.clear();

    emit this->loadComplete(retVal);
    return retVal;
}

void QAstCTIService::loadActions(const bool &bMayLoad)
{
    if (!bMayLoad) return;

    this->actions.clear();
    QSqlDatabase db = QSqlDatabase::database("mysqldb");
    if (!db.isOpen()) {
        db.open();
    }
    QSqlQuery query(db);
    QString sql = "SELECT ID_ACTION FROM services_actions WHERE ID_SERVICE=:id ORDER BY ACTION_ORDER ASC, ID_ACTION ASC";
    if (!query.prepare(sql)) {
        qCritical("Prepare failed in QAstCTIService::loadActions() %s:%d",  __FILE__ , __LINE__);
    } else {
        query.bindValue(":id", this->idService);
        if (!query.exec()) {
            qCritical("Query execution failed in QAstCTIService::loadActions() %s:%d",  __FILE__ , __LINE__);
        } else {
            while(query.next()) {
                int actionId = query.value(0).toInt(0);
                QAstCTIAction *action = this->actionsList->operator [](actionId);
                if (action != 0) {
                    this->actions.insert(actionId, action);
                }
            }
            query.finish();
        }
    }

    query.clear();
}

void QAstCTIService::loadOperators(const bool &bMayLoad)
{
    if (bMayLoad) {
        this->operators->setIdService(this->idService);
    }
}

void QAstCTIService::loadVariables(const bool &bMayLoad)
{
    if (bMayLoad) {
        this->variables->setIdService(this->idService);
    }
}


int QAstCTIService::getIdService()
{
    return this->idService;
}

QString QAstCTIService::getServiceName()
{
    return this->serviceName;
}

QString QAstCTIService::getServiceContextType()
{
    return this->serviceContextType;
}

bool QAstCTIService::getServiceIsQueue()
{
    return this->serviceIsQueue;
}

QString QAstCTIService::getServiceQueueName()
{
    return this->serviceQueueName;
}


bool QAstCTIService::getEnabled()
{
    return this->enabled;
}

QAstCTIServicesOperators *QAstCTIService::getOperators()
{
    return this->operators;
}

QAstCTIServicesVariables *QAstCTIService::getVariables()
{
    return this->variables;
}

QHash<int, QAstCTIAction *> *QAstCTIService::getActions()
{
    return &this->actions;
}
