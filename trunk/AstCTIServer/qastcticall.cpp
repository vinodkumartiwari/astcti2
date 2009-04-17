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

#include "qastcticall.h"

QAstCTICall::QAstCTICall(QObject* parent)
        : QObject(parent), channel(""), parsed_channel(""),
        callerid_num(""), callerid_name(""),
        uniqueid(""), context(""), state("")
{

}

QString& QAstCTICall::get_channel()
{
    return this->channel;
}
void QAstCTICall::set_channel(QString channel)
{
    this->channel = channel;
}
QString& QAstCTICall::get_parsed_channel()
{
    return this->parsed_channel;
}
void QAstCTICall::set_parsed_channel(QString parsedchannel)
{
    this->parsed_channel = parsedchannel;
}
QString& QAstCTICall::get_callerid_num()
{
    return this->callerid_num;
}
void QAstCTICall::set_callerid_num(QString calleridnum)
{
    this->callerid_num = calleridnum;
}
QString& QAstCTICall::get_callerid_name()
{
    return this->callerid_name;
}
void QAstCTICall::set_callerid_name(QString calleridname)
{
    this->callerid_name = calleridname;
}
QString& QAstCTICall::get_uniqueid()
{
    return this->uniqueid;
}
void QAstCTICall::set_uniqueid(QString uniqueid)
{
    this->uniqueid = uniqueid;
}
QString& QAstCTICall::get_context()
{
    return this->context;
}
void QAstCTICall::set_context(QString context)
{
    this->context = context;
}
QString& QAstCTICall::get_state()
{
    return this->state;
}
void QAstCTICall::set_state(QString state)
{
    this->state = state;
}
