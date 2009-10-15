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
#include "qastctioperator.h"


QAstCTIOperator::QAstCTIOperator(const int &id, QObject *parent)
        : QObject(parent), idOperator(id), fullName(""), userName(""),
        passWord(""), lastSeatId(0), beginInPause(false),
        enabled(false), lastSeat(0), listOfServices(0)
{
    // Let's connect our signals
    connect(this, SIGNAL(loadComplete(const bool&)), this, SLOT(loadSeat(const bool&)));
    connect(this, SIGNAL(loadComplete(const bool&)), this, SLOT(loadListOfServices()));
    connect(this, SIGNAL(updateComplete(const bool&)), this, SLOT(loadSeat(const bool&)));
}

QAstCTIOperator::~QAstCTIOperator()
{
    qDebug() << "In QAstCTIOperator::~QAstCTIOperator()";
    if (this->lastSeat != 0) {
        delete(this->lastSeat);
    }
    if (this->listOfServices != 0) {
        delete (this->listOfServices);
    }
}


bool QAstCTIOperator::load()
{
    bool retVal = false;
    QSqlDatabase db = QSqlDatabase::database("mysqldb");
    QSqlQuery query(db);
    query.prepare("SELECT * FROM operators WHERE ID_OPERATOR=:id");
    query.bindValue(":id", this->idOperator);
    retVal = query.exec();
    if ( (retVal) &  (query.first()) ) {
        this->fullName = query.value(1).toString();
        this->userName = query.value(2).toString();
        this->passWord = query.value(3).toString();
        this->lastSeatId = query.value(4).toInt(0);
        this->beginInPause = query.value(5).toBool();
        this->enabled = query.value(6).toBool();

        query.finish();
    }
    query.clear();

    emit this->loadComplete(retVal);
    return retVal;
}

void QAstCTIOperator::loadSeat(const bool &bMayLoad)
{
    if (!bMayLoad) return;

    if (this->lastSeatId > 0) {
        this->lastSeat = new QAstCTISeat(this->lastSeatId, this);
        if (this->lastSeat->load()) {
            qDebug() << "loadSeat complete for operator" << this->userName;
        }
    }
}

void QAstCTIOperator::loadListOfServices()
{
    if (this->idOperator > 0) {
        this->listOfServices = new QAstCTIOperatorServices(this->idOperator, this);
        qDebug() << "loadListOfServices complete for operator" << this->userName;
    }
}

bool QAstCTIOperator::save()
{
    bool retVal = false;
    QSqlDatabase db = QSqlDatabase::database("mysqldb");
    if (!db.isOpen()) {
        db.open();
    }
    if (db.driver()->hasFeature(QSqlDriver::Transactions)) {
        db.transaction();
    }
    QSqlQuery query(db);

    // For now we need to update just only the field last_seat in the operators table
    query.prepare("UPDATE operators SET LAST_SEAT=:seat WHERE ID_OPERATOR=:id");
    query.bindValue(":seat", this->lastSeatId);
    query.bindValue(":id", this->idOperator);
    retVal = query.exec();

    if (db.driver()->hasFeature(QSqlDriver::Transactions)) {
        retVal ? db.commit() :  db.rollback();
    }
    query.clear();

    emit this->updateComplete(retVal);
    return retVal;
}

bool QAstCTIOperator::checkPassword(const QString& password)
{

    /*QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(QByteArray(password.toUtf8()));
    QByteArray result = md5.result();*/

    return (password.compare(this->passWord) == 0);
}

int  QAstCTIOperator::getIdOperator()
{
    return this->idOperator;
}

QString  QAstCTIOperator::getUserName()
{
    return this->userName;
}

QString  QAstCTIOperator::getFullName()
{
    return this->fullName;
}

QString  QAstCTIOperator::getPassWord()
{
    return this->passWord;
}

int QAstCTIOperator::getLastSeatId()
{
    return this->lastSeatId;
}

QAstCTISeat* QAstCTIOperator::getLastSeat()
{
    return this->lastSeat;
}

void QAstCTIOperator::setLastSeatId(const int &newSeat)
{
    this->lastSeatId = newSeat;
}

bool QAstCTIOperator::getBeginInPause()
{
    return this->beginInPause;
}

bool QAstCTIOperator::getEnabled()
{
    return this->enabled;
}

QHash<QString,int> *QAstCTIOperator::getListOfServices()
{
    return this->listOfServices->getServicesList();
}
