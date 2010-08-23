/* Copyright (C) 2007-2010 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * Copyright (C) 2007-2010 Lumiss d.o.o.
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

#include "db.h"

//Creates a database connection and adds it to the list of connections for later use
bool DB::buildSqlDatabase(const QString &host, quint16 &port, const QString &user, const QString &pass, const QString &database)
{
    QSqlDatabase db  = QSqlDatabase::database("mysqldb");
    if (db.isValid()) {
        destroySqlDatabase();
    }

    db = QSqlDatabase::addDatabase("QMYSQL", "mysqldb");
    db.setHostName(host);
    db.setPort(port);
    db.setUserName(user);
    db.setPassword(pass);
    db.setDatabaseName(database);

    if(!db.open()) {
        qCritical() << "Unable to open MySQL database: " << db.lastError() << "\n";

        return false;
    }

    return true;
}

//Cleses the database connection and removes it from the list of connections
void DB::destroySqlDatabase()
{
    if (QSqlDatabase::contains("mysqldb")) {
        QSqlDatabase db = QSqlDatabase::database("mysqldb");

        if (db.isValid()) {
            if (db.isOpen())
                db.close();
            QSqlDatabase::removeDatabase("mysqldb");
        }
    }
}

//Executes SQL command with optional list of parameters and returns QSqlQuery object
QSqlQuery DB::execSQL(const QString &sql, const QVariantList &parameters, bool *ok)
{
    QSqlDatabase db = QSqlDatabase::database("mysqldb");
    if (!db.isOpen()) {
        db.open();
    }
    QSqlQuery query(db);
    query.setForwardOnly(true);
    if (query.prepare(sql)) {
        for (int i = 0; i < parameters.size(); ++i) {
            query.addBindValue(parameters.at(i));
        }
        if (query.exec()) {
            *ok = true;
        } else {
            if (query.lastError().isValid()) {
                QString params = "";
                for (int i = 0; i < parameters.size(); ++i) {
                    params += parameters.at(i).toString() + "|";
                }
                if (params.length() > 0)
                    params.remove(params.length() - 1, 1);

                qDebug() << "Error while executing SQL query:\n" << query.executedQuery() <<
                            "\nParameters: " << params << "\n" << query.lastError();
            }
            *ok = false;
        }
    }

    return query;
}

//Executes SQL command and returns a single value
QVariant DB::readScalar(const QString &sql, bool *ok)
{
    return readScalar(sql, QVariantList(), ok);
}

//Executes SQL command with parameters and returns a single value
QVariant DB::readScalar(const QString &sql, const QVariantList &parameters, bool *ok)
{
    QSqlQuery query = execSQL(sql, parameters, ok);

    if (*ok && query.first()) {
        return query.value(0);
    } else {
        return QVariant();
    }
}

//Executes SQL command and returns a list of values
QVariantList DB::readList(const QString &sql, bool *ok)
{
    return readList(sql, QVariantList(), ok);
}

//Executes SQL command with parameters and returns a list of values
QVariantList DB::readList(const QString &sql, const QVariantList &parameters, bool *ok)
{
    QVariantList result;

    QSqlQuery query = execSQL(sql, parameters, ok);

    if (*ok) {
        while (query.next())
            result.append(query.value(0));
    }

    return result;
}

//Executes SQL command and returns values in a form of a table
QList<QVariantList> DB::readTable(const QString &sql, bool *ok)
{
    return readTable(sql, QVariantList(), ok);
}

//Executes SQL command with parameters and returns values in form of a table
QList<QVariantList> DB::readTable(const QString &sql, const QVariantList &parameters, bool *ok)
{
    QList<QVariantList> result;

    QSqlQuery query = execSQL(sql, parameters, ok);

    if (*ok) {
        int columnCount = query.record().count();
        QVariantList rowData;
        for (int column = 0; column < columnCount; ++column) {
            rowData.append(QVariant());
        }
        while (query.next()) {
            for (int column = 0; column < columnCount; ++column) {
                rowData[column] = query.value(column);
            }
            result.append(rowData);
        }
    }

    return result;
}

//Executes SQL command and returns the number of affected rows
int DB::executeNonQuery(const QString &sql, bool *ok)
{
    return executeNonQuery(sql, QVariantList(), ok);
}

//Executes SQL command with parameters and returns the number of affected rows
int DB::executeNonQuery(const QString &sql, const QVariantList &parameters, bool *ok)
{
    QSqlQuery query = execSQL(sql, parameters, ok);

    if (*ok) {
        return query.numRowsAffected();
    } else {
        return -1;
    }
}
