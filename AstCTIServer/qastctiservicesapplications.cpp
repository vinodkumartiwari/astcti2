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

#include "qastctiservicesapplications.h"
#include "qastctiapplication.h"

QAstCTIServicesApplications::QAstCTIServicesApplications()
        : ID_SERVICE(0)
{
}

QAstCTIServicesApplications::QAstCTIServicesApplications(const int &idservice)
        : ID_SERVICE(idservice)
{
    this->fillApplications();
}

QAstCTIServicesApplications::~QAstCTIServicesApplications()
{
    this->clear();
}


QAstCTIApplication *QAstCTIServicesApplications::operator[](const QString &key)
{
    return (this->applications.contains(key)) ? this->applications[key] : 0;

}

void QAstCTIServicesApplications::setIdService(const int &idservice)
{
    this->ID_SERVICE = idservice;
    this->fillApplications();
}
void QAstCTIServicesApplications::addApplication(QAstCTIApplication *app)
{
    this->applications.insert(app->getApplicationOsType() , app);
}

void QAstCTIServicesApplications::removeApplication(const QString &key)
{
    if (this->applications.contains(key))
    {
        QAstCTIApplication *app = this->applications[key];
        if (app != 0)
        {
            delete(app);
        }
        this->applications.remove(key);
    }
}
int QAstCTIServicesApplications::count()
{
    // Count how many elements we have
    return this->applications.count();
}

void QAstCTIServicesApplications::clear()
{
    // Do a clear only if really needed
    if (this->applications.count() > 0)
    {
        QMutableHashIterator<QString, QAstCTIApplication*> i(this->applications);
        while (i.hasNext()) {
            i.next();
            delete(i.value());
        }
        this->applications.clear();
    }
}

void QAstCTIServicesApplications::fillApplications()
{
    QSqlDatabase db = QSqlDatabase::database("sqlitedb");
    QSqlQuery query(db);
    query.prepare("SELECT ID_APPLICATION FROM applications WHERE ID_SERVICE=:id ORDER BY ID_APPLICATION ASC");
    query.bindValue(":id", this->ID_SERVICE);
    query.exec();
    while(query.next())
    {
        QAstCTIApplication *app = new QAstCTIApplication(query.value(0).toInt(0));
        if (app->Load())
        {
            QString appType = app->getApplicationOsType();

            // Remove service if exists before load
            this->removeApplication(appType);
            this->addApplication(app);
        }
    }
    query.finish();
    query.clear();
}
