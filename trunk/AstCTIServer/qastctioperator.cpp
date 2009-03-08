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


QAstCTIOperator::QAstCTIOperator(const int &id, QObject* parent)
        : QObject(parent), ID_OPERATOR(id), FULL_NAME(""), USER_NAME(""),
        PASS_WORD(""), LAST_SEAT(0), BEGIN_IN_PAUSE(false),
        ENABLED(false), lastSeat(0)
{
    // Let's connect our signals
    connect(this, SIGNAL(load_complete(const bool&)), this, SLOT(load_seat(const bool&)));
    connect(this, SIGNAL(update_complete(const bool&)), this, SLOT(load_seat(const bool&)));
}

QAstCTIOperator::~QAstCTIOperator()
{
    qDebug() << "In QAstCTIOperator::~QAstCTIOperator()";
}


bool QAstCTIOperator::load()
{
    bool retVal = false;
    QSqlDatabase db = QSqlDatabase::database("sqlitedb");
    QSqlQuery query(db);
    query.prepare("SELECT * FROM operators WHERE ID_OPERATOR=:id");
    query.bindValue(":id", this->ID_OPERATOR);
    retVal = query.exec();
    if ( (retVal) &  (query.first()) )
    {
        this->FULL_NAME = query.value(1).toString();
        this->USER_NAME = query.value(2).toString();
        this->PASS_WORD = query.value(3).toString();
        this->LAST_SEAT = query.value(4).toInt(0);
        this->BEGIN_IN_PAUSE = query.value(5).toBool();
        this->ENABLED = query.value(6).toBool();

        query.finish();
    }
    query.clear();

    emit this->load_complete(retVal);
    return retVal;
}

void QAstCTIOperator::load_seat(const bool &bMayLoad)
{
    if (!bMayLoad) return;

    if (this->LAST_SEAT > 0)
    {
        this->lastSeat = new QAstCTISeat(this->LAST_SEAT, this);
        if (this->lastSeat->load())
        {
            qDebug() << "load_seat complete for operator" << this->USER_NAME;
        }
    }
}

bool QAstCTIOperator::save()
{
    bool retVal = false;
    QSqlDatabase db = QSqlDatabase::database("sqlitedb");

    if (db.driver()->hasFeature(QSqlDriver::Transactions))
        db.transaction();

    QSqlQuery query(db);

    // For now we need to update just only the field last_seat in the operators table
    query.prepare("UPDATE operators SET LAST_SEAT=:seat WHERE ID_OPERATOR=:id");
    query.bindValue(":seat", this->LAST_SEAT);
    query.bindValue(":id", this->ID_OPERATOR);
    retVal = query.exec();

    if (db.driver()->hasFeature(QSqlDriver::Transactions))
        retVal ? db.commit() :  db.rollback();

    query.clear();

    emit this->update_complete(retVal);
    return retVal;
}

bool QAstCTIOperator::check_password(const QString& password)
{

    /*QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(QByteArray(password.toUtf8()));
    QByteArray result = md5.result();*/

    return (password.compare(this->PASS_WORD) == 0);
}

int  QAstCTIOperator::get_id_operator()
{
    return this->ID_OPERATOR;
}

QString  QAstCTIOperator::get_user_name()
{
    return this->USER_NAME;
}

QString  QAstCTIOperator::get_full_name()
{
    return this->FULL_NAME;
}

QString  QAstCTIOperator::get_pass_word()
{
    return this->PASS_WORD;
}

int QAstCTIOperator::get_last_seat()
{
    return this->LAST_SEAT;
}

QAstCTISeat* QAstCTIOperator::get_seat()
{
    return this->lastSeat;
}


void QAstCTIOperator::set_last_seat(const int &newSeat)
{
    this->LAST_SEAT = newSeat;
}

bool QAstCTIOperator::get_begin_in_pause()
{
    return this->BEGIN_IN_PAUSE;
}

bool QAstCTIOperator::get_enabled()
{
    return this->ENABLED;
}

