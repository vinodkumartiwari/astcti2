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
 * AstCtiServer.  If you copy code from other releases into a copy of GNU
 * AstCtiServer, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCtiServer, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include "QsLog.h"
#include "db.h"

//Define static variables
QSqlDatabase* DB::connection;
bool DB::supportsTransactions;
bool DB::inTransaction;

//Creates a database connection and adds it to the list of connections for later use
bool DB::buildConnection(const QString& host, const quint16 port,
						 const QString& user, const QString& pass, const QString& database)
{
	connection = new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL", dbConnectionName));

	if(connection->isValid()) {
		connection->setHostName(host);
		connection->setPort(port);
		connection->setUserName(user);
		connection->setPassword(pass);
		connection->setDatabaseName(database);

		supportsTransactions = connection->driver()->hasFeature(QSqlDriver::Transactions);
		inTransaction = false;

		return true;
	} else {
		QLOG_ERROR() << "Unable to create database connection:" << connection->lastError();
		return false;
	}
}

//Opens the database connection
bool DB::openConnection()
{
	if (!connection->isOpen())
		if (!connection->open()) {
			QLOG_ERROR() << "Unable to open database:" << connection->lastError();
		}

	return connection->isOpen();
}

//Closes the database connection
void DB::closeConnection()
{
	if (inTransaction)
		return;

	if (connection->isValid())
		if (connection->isOpen())
			connection->close();
}

//Closes the database connection and removes it from the list of connections
void DB::destroyConnection()
{
	closeConnection();
	delete connection;
	QSqlDatabase::removeDatabase(dbConnectionName);
}

bool DB::beginTransaction()
{
	if (supportsTransactions) {
		if (connection->transaction()) {
			inTransaction = true;
		} else {
			QString logMsg = "Error while opening transaction:\n";
			if (connection->lastError().isValid())
				logMsg.append("Error message: ").append(connection->lastError().text());
			else
				logMsg.append("No error information available.");

			QLOG_ERROR() << logMsg;
		}
	}

	return inTransaction;
}

bool DB::commitTransaction()
{
	if (inTransaction) {
		if (connection->commit()) {
			inTransaction = false;
			closeConnection();
		} else {
			QString logMsg = "Error while committing transaction:\n";
			if (connection->lastError().isValid())
				logMsg.append("Error message: ").append(connection->lastError().text());
			else
				logMsg.append("No error information available.");

			QLOG_ERROR() << logMsg;
		}
	}

	return !inTransaction;
}

bool DB::rollbackTransaction()
{
	if (inTransaction) {
		if (connection->rollback()) {
			inTransaction = false;
			closeConnection();
		} else {
			QString logMsg = "Error while rolling back transaction:\n";
			if (connection->lastError().isValid())
				logMsg.append("Error message: ").append(connection->lastError().text());
			else
				logMsg.append("No error information available.");

			QLOG_ERROR() << logMsg;
		}
	}

	return !inTransaction;
}

//Executes SQL command with optional list of parameters
//Returns QSqlQuery object
QSqlQuery* DB::execSQL(const QString& sql, const QVariantList& params, bool* ok)
{
	QSqlQuery* query = new QSqlQuery(*connection);
	if (!openConnection()) {
		*ok = false;
		return query;
	}
	query->setForwardOnly(true);
	if (query->prepare(sql)) {
		for (int i = 0; i < params.size(); ++i) {
			query->addBindValue(params.at(i));
        }
		*ok = query->exec();
	} else {
		*ok = false;
	}

	if (!*ok) {
		QString logMsg = "Error while executing SQL query: ";
		logMsg.append(query->executedQuery());
		if (params.size() > 0) {
			logMsg.append("\nParameters: ");
			for (int i = 0; i < params.size(); ++i) {
				logMsg.append(params.at(i).toString()).append("|");
			}
		}
		if (query->lastError().isValid())
			logMsg.append("\nError message: ").append(query->lastError().text());
		else
			logMsg.append("\nNo error information available.");

		QLOG_ERROR() << logMsg;
	}

	return query;
}

//Executes SQL command and returns a single value
QVariant DB::readScalar(const QString& sql, bool* ok)
{
    return readScalar(sql, QVariantList(), ok);
}

//Executes SQL command with parameters and returns a single value
QVariant DB::readScalar(const QString& sql, const QVariantList& params, bool* ok)
{
	QVariant result;

	QSqlQuery* query = execSQL(sql, params, ok);

	if (*ok) {
		if (query->first())
			result = query->value(0);
		else
			//No error, but record was not found, so we set ok to false
			*ok = false;
	}

	delete query;
	closeConnection();
	return result;
}

//Executes SQL command and returns a list of values
QVariantList DB::readList(const QString& sql, bool* ok)
{
    return readList(sql, QVariantList(), ok);
}

//Executes SQL command with parameters and returns a list of values
QVariantList DB::readList(const QString& sql, const QVariantList& params, bool* ok)
{
	QVariantList result;

	QSqlQuery* query = execSQL(sql, params, ok);

	if (*ok)
		while (query->next())
			result.append(query->value(0));

	delete query;
	closeConnection();
	return result;
}

//Executes SQL command and returns a single row as QVariantList
QVariantList DB::readRow(const QString& sql, bool* ok)
{
	return readRow(sql, QVariantList(), ok);
}

//Executes SQL command and returns a single row as QVariantList
QVariantList DB::readRow(const QString& sql, const QVariantList& params, bool* ok)
{
	QVariantList result;

	QSqlQuery* query = execSQL(sql, params, ok);

	if (*ok) {
		*ok = query->first();
		if (*ok) {
			const int columnCount = query->record().count();
			for (int column = 0; column < columnCount; column++)
				result.append(query->value(column));
		}
	}

	delete query;
	closeConnection();
	return result;
}

//Executes SQL command and returns values in a form of a table
QVariantTable DB::readTable(const QString& sql, bool* ok)
{
    return readTable(sql, QVariantList(), ok);
}

//Executes SQL command with parameters and returns values in form of a table
QVariantTable DB::readTable(const QString& sql, const QVariantList& params, bool* ok)
{
	QVariantTable result;

	QSqlQuery* query = execSQL(sql, params, ok);

	if (*ok) {
		const int columnCount = query->record().count();
		QVariantList rowData;
		for (int column = 0; column < columnCount; column++)
			rowData.append(QVariant());
		while (query->next()) {
			for (int column = 0; column < columnCount; column++)
				rowData[column] = query->value(column);
			result.append(rowData);
		}
	}

	delete query;
	closeConnection();
	return result;
}

//Executes SQL command
//Returns the number of affected rows on success or -1 on failure
int DB::executeNonQuery(const QString& sql)
{
	return executeNonQuery(sql, QVariantList());
}

//Executes SQL command with parameters
//Returns the number of affected rows on success or -1 on failure
int DB::executeNonQuery(const QString& sql, const QVariantList& params)
{
	int result = -1;

	bool ok;
	QSqlQuery* query = execSQL(sql, params, &ok);
	if (ok)
		result = query->numRowsAffected();

	delete query;
	closeConnection();
	return result;
}
