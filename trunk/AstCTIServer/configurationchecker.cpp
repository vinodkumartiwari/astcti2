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
#include "configurationchecker.h"

ConfigurationChecker::ConfigurationChecker(QObject *parent) :
        QThread(parent), lastTimeStamp(0)
{

}

ConfigurationChecker::~ConfigurationChecker()
{
    qDebug() << "In ConfigurationChecker::~ConfigurationChecker()";

}

void ConfigurationChecker::startConfigurationCheckerThread()
{
    this->running = true;
    this->start();
}

void ConfigurationChecker::stopConfigurationCheckerThread()
{
    this->running = false;

}

int ConfigurationChecker::getLastModified()
{
    return this->lastTimeStamp;
}

void ConfigurationChecker::run()
{
    long modified = 0;
    while(this->running) {
        modified = readLastModified();
        if (modified > this->lastTimeStamp) {
            this->lastTimeStamp = modified;
            emit this->newConfiguration();
        }
        this->sleep(30);
    }
}


int ConfigurationChecker::readLastModified()
{
    int result = 0;
    QSqlDatabase db = QSqlDatabase::database("mysqldb");
    if (!db.isOpen()) {
        db.open();
    }
    QSqlQuery query(db);
    if (query.exec("SELECT LAST_UPDATE FROM dbversion LIMIT 1")) {
        query.first();
        result = query.value(0).toInt();
        query.finish();
    }
    query.clear();
    return result;
}
