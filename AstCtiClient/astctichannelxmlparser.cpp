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
 * AstCtiClient.  If you copy code from other releases into a copy of GNU
 * AstCtiClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCtiClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include <QHashIterator>
#include <QDebug>

#include "astctichannelxmlparser.h"

AstCtiChannelXmlParser::AstCtiChannelXmlParser(AstCtiChannel *channel)
        : QXmlDefaultHandler()
{
	this->channel = channel;

    this->inVariables = false;
}

bool AstCtiChannelXmlParser::startElement(const QString &, const QString &,
										  const QString &name, const QXmlAttributes &attrs)
{
    if (name == "Variables") {
        this->inVariables = true;
    } else if (name == "Application") {
		this->currentAction = QSharedPointer<AstCtiAction>(new AstCtiAction);
        this->currentAction->type = ActionTypeApplication;
    } else if (name == "Browser") {
		this->currentAction = QSharedPointer<AstCtiAction>(new AstCtiAction);
		this->currentAction->type = ActionTypeExternalBrowser;
    } else if (name == "InternalBrowser") {
		this->currentAction = QSharedPointer<AstCtiAction>(new AstCtiAction);
		this->currentAction->type = ActionTypeInternalBrowser;
    } else if (name == "TcpMessage") {
		this->currentAction = QSharedPointer<AstCtiAction>(new AstCtiAction);
		this->currentAction->type = ActionTypeTcpMessage;
    } else if (name == "UdpMessage") {
		this->currentAction = QSharedPointer<AstCtiAction>(new AstCtiAction);
		this->currentAction->type = ActionTypeUdpMessage;
	} else if (name == "Channel") {
		this->channel->uniqueId = attrs.value(0);
    }

    return true;
}

bool AstCtiChannelXmlParser::characters(const QString &ch)
{
    this->currentText = ch;

    return true;
}

bool AstCtiChannelXmlParser::endElement(const QString &, const QString &, const QString &name)
{
    if (inVariables) {
		this->channel->variables.insert(name, this->currentText);
    } else if (name == "Variables") {
        this->inVariables = false;
    } else if (name == "Application" ||
               name == "Browser" ||
               name == "InternalBrowser" ||
               name == "TcpMessage" ||
               name == "UdpMessage") {
		this->channel->actions.append(this->currentAction);
	} else if (name == "Destination") {
		this->currentAction->destination = this->currentText;
	} else if (name == "Parameters") {
        //Server will fill in the variable values, no need to do it here
//        QHashIterator<QString, QString> i(this->channel->variables);
//        while (i.hasNext()) {
//            i.next();
//            this->currentText.replace(i.key(), i.value(), Qt::CaseSensitive);
//        }
        this->currentAction->parameters = this->currentText;
    } else if (name == "Encoding") {
        this->currentAction->encoding = this->currentText;
    } else if (name == "Channel") {
		this->channel->channel = this->currentText;
    } else if (name == "ParsedChannel") {
		this->channel->parsedChannel = this->currentText;
    } else if (name == "CallerIdNum") {
		this->channel->callerIdNum = this->currentText;
    } else if (name == "CallerIdName") {
		this->channel->callerIdName = this->currentText;
    } else if (name == "Context") {
		this->channel->context = this->currentText;
	} else if (name == "Exten") {
		this->channel->exten = this->currentText;
	} else if (name == "State") {
		this->channel->state = this->currentText;
	} else if (name == "AccountCode") {
		this->channel->accountCode = this->currentText;
	} else if (name == "BridgeId") {
		this->channel->bridgeId = this->currentText.toInt();
	}

    return true;
}
