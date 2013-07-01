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
 * AstCTIClient.  If you copy code from other releases into a copy of GNU
 * AstCTIClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTIClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include <QHashIterator>
#include <QDebug>

#include "astcticallxmlparser.h"

AstCTICallXMLParser::AstCTICallXMLParser(AstCTICall *call)
        : QXmlDefaultHandler()
{
    this->call = call;

    this->inVariables = false;
}

bool AstCTICallXMLParser::startElement(const QString &, const QString &,
									   const QString &name, const QXmlAttributes &attrs)
{
    if (name == "Variables") {
        this->inVariables = true;
    } else if (name == "Application") {
        this->currentAction = new AstCTIAction;
        this->currentAction->type = ActionTypeApplication;
    } else if (name == "Browser") {
        this->currentAction = new AstCTIAction;
        this->currentAction->type = ActionTypeExternalBrowser;
    } else if (name == "InternalBrowser") {
        this->currentAction = new AstCTIAction;
        this->currentAction->type = ActionTypeInternalBrowser;
    } else if (name == "TcpMessage") {
        this->currentAction = new AstCTIAction;
        this->currentAction->type = ActionTypeTcpMessage;
    } else if (name == "UdpMessage") {
        this->currentAction = new AstCTIAction;
        this->currentAction->type = ActionTypeUdpMessage;
    } else if (name == "call") {
        this->call->uniqueId = attrs.value(0);
    }

    return true;
}

bool AstCTICallXMLParser::characters(const QString &ch)
{
    this->currentText = ch;

    return true;
}

bool AstCTICallXMLParser::endElement(const QString &, const QString &, const QString &name)
{
    if (inVariables) {
        this->call->variables.insert(name, this->currentText);
    } else if (name == "Variables") {
        this->inVariables = false;
    } else if (name == "Application" ||
               name == "Browser" ||
               name == "InternalBrowser" ||
               name == "TcpMessage" ||
               name == "UdpMessage") {
        this->call->actions.append(this->currentAction);
        this->currentAction = 0;
	} else if (name == "Destination") {
		this->currentAction->destination = this->currentText;
	} else if (name == "Parameters") {
        //Server will fill in the variable values, no need to do it here
//        QHashIterator<QString, QString> i(this->call->variables);
//        while (i.hasNext()) {
//            i.next();
//            this->currentText.replace(i.key(), i.value(), Qt::CaseSensitive);
//        }
        this->currentAction->parameters = this->currentText;
    } else if (name == "Encoding") {
        this->currentAction->encoding = this->currentText;
    } else if (name == "Channel") {
        this->call->channel = this->currentText;
    } else if (name == "ParsedChannel") {
        this->call->parsedChannel = this->currentText;
    } else if (name == "DestChannel") {
        this->call->destChannel = this->currentText;
    } else if (name == "ParsedDestChannel") {
        this->call->parsedDestChannel = this->currentText;
    } else if (name == "CallerIdNum") {
        this->call->callerIdNum = this->currentText;
    } else if (name == "CallerIdName") {
        this->call->callerIdName = this->currentText;
    } else if (name == "DestUniqueId") {
        this->call->destUniqueId = this->currentText;
    } else if (name == "Context") {
        this->call->context = this->currentText;
    } else if (name == "State") {
        this->call->state = this->currentText;
    }

    return true;
}
