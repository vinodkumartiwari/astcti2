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

#include <QDebug>

#include "db.h"
#include "qastctioperators.h"
#include "qastctioperator.h"

QAstCTIOperators::QAstCTIOperators(QObject *parent)
        : QObject(parent)
{
    this->fillOperators();
}

QAstCTIOperators::~QAstCTIOperators()
{
    qDebug() << "In QAstCTIOperators::~QAstCTIOperators()";
    this->clear();
}

QAstCTIOperator *QAstCTIOperators::operator[](const QString &key)
{
    return (this->operators.contains(key)) ? this->operators[key] : 0;
}

void QAstCTIOperators::addOperator(QAstCTIOperator *oper)
{
    this->operators.insert(oper->getUserName(), oper);
}

void QAstCTIOperators::removeOperator(const QString &key)
{
    if (this->operators.contains(key)) {
		delete this->operators[key];
        this->operators.remove(key);
    }
}

int QAstCTIOperators::count()
{
    // Count how many elements we have
    return this->operators.count();
}

void QAstCTIOperators::clear()
{
	qDeleteAll(this->operators);
	this->operators.clear();
}

void QAstCTIOperators::fillOperators()
{
	bool ok;
	const QVariantList operatorIds =
		  DB::readList("SELECT ID_OPERATOR FROM operators ORDER BY ID_OPERATOR ASC", &ok);
	if (ok) {
		const int listSize = operatorIds.size();
		for (int i = 0; i < listSize; i++) {
			QAstCTIOperator *oper = new QAstCTIOperator(operatorIds.at(i).toInt(), this);
			if (oper->load()) {
				// Remove item if it exists
				this->removeOperator(oper->getUserName());
				this->addOperator(oper);
			}
		}
	}
}
