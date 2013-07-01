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

#include <QStringList>
#include <QDomDocument>

#include "QsLog.h"
#include "astcticall.h"

AstCtiCall::AstCtiCall(const QString &uniqueId, QObject *parent) : QObject(parent)
{
	QLOG_TRACE() << "Creating new AstCtiCall" << uniqueId;

	this->uniqueId = uniqueId;
	this->destUniqueId = "";
	this->actions = 0;
	this->channel = "";
	this->parsedChannel = "";
	this->destChannel = "";
	this->parsedDestChannel = "";
	this->callerIdNum = "";
	this->callerIdName = "";
	this->context = "";
	this->state = "";
	this->exten = "";
	this->accountCode = "";
	this->clientOperatingSystem = ActionOsAll;
}

AstCtiCall::~AstCtiCall()
{
	QLOG_TRACE() << "Destroying AstCtiCall" << this->uniqueId;
}

QString &AstCtiCall::getChannel()
{
    return this->channel;
}

void AstCtiCall::setChannel(const QString &channel)
{
    this->channel = channel;
	this->setParsedChannel(this->parseChannel(channel));
}

QString &AstCtiCall::getParsedChannel()
{
    return this->parsedChannel;
}

void AstCtiCall::setParsedChannel(const QString &parsedChannel)
{
    this->parsedChannel = parsedChannel;
}

QString &AstCtiCall::getDestChannel()
{
    return this->destChannel;
}

void AstCtiCall::setDestChannel(const QString &channel)
{
    this->destChannel = channel;
	this->setParsedDestChannel(this->parseChannel(channel));
}

QString &AstCtiCall::getParsedDestChannel()
{
    return this->parsedDestChannel;
}

void AstCtiCall::setParsedDestChannel(const QString &parsedChannel)
{
    this->parsedDestChannel = parsedChannel;
}

QString &AstCtiCall::getCalleridNum()
{
    return this->callerIdNum;
}

void AstCtiCall::setCalleridNum(const QString &callerIdNum)
{
	this->callerIdNum = callerIdNum;
	this->addVariable("CALLERID", callerIdNum);
}

QString &AstCtiCall::getCalleridName()
{
    return this->callerIdName;
}

void AstCtiCall::setCalleridName(const QString &callerIdName)
{
	this->callerIdName = callerIdName;
	this->addVariable("CALLERIDNAME", callerIdName);
}

QString &AstCtiCall::getUniqueId()
{
    return this->uniqueId;
}

void AstCtiCall::setUniqueId(const QString &uniqueId)
{
    this->uniqueId = uniqueId;
}

QString &AstCtiCall::getDestUniqueId()
{
    return this->destUniqueId;
}

void AstCtiCall::setDestUniqueId(const QString &uniqueId)
{
    this->destUniqueId = uniqueId;
}

QString &AstCtiCall::getContext()
{
    return this->context;
}

void AstCtiCall::setContext(const QString &context)
{
    this->context = context;
}

QString &AstCtiCall::getState()
{
    return this->state;
}

void AstCtiCall::setState(const QString &state)
{
    this->state = state;
}

QString &AstCtiCall::getExten()
{
	return this->exten;
}

void AstCtiCall::setExten(const QString &exten)
{
	this->exten = exten;
}

QString &AstCtiCall::getAccountCode()
{
	return this->accountCode;
}

void AstCtiCall::setAccountCode(const QString &accountCode)
{
	this->accountCode = accountCode;
}

void AstCtiCall::addVariable(const QString &name, const QString &value)
{
	//There is no need to remove existing item,
	//insert will replace value if the key already exists
	this->variables.insert(name, value);
}

void AstCtiCall::setVariable(const QString &name, const QString &value)
{
	if (this->variables.contains(name))
		this->variables.insert(name, value);
}

QString AstCtiCall::parseChannel(const QString &channel)
{
    // Channels are usually built like SIP/200-0899e2c8
    // So we need to extract channel by split the string
	if (channel.contains('-')) {
		QStringList parts = channel.split('-');
		if (parts.count() > 0) {
			return parts.at(0);
        }
    }

	return "";
}

void AstCtiCall::setActions(QMap<int, AstCtiAction*> *callActions) {

    this->actions = callActions;
}

void AstCtiCall::setOperatingSystem(QString operatingSystem)
{
	this->clientOperatingSystem = AstCtiAction::parseOsType(operatingSystem);
}

void AstCtiCall::parseActionsParameters()
{
	if (this->actions != 0 && this->variables.count() > 0) {
		QMap<int, AstCtiAction*>::const_iterator actionIterator = this->actions->constBegin();
		while (actionIterator != this->actions->constEnd()) {
			QString parameters = ((AstCtiAction*)actionIterator.value())->getParameters();
			QHash<QString, QString>::const_iterator varIterator = this->variables.constBegin();
			while (varIterator != this->variables.constEnd()) {
				QString varName = QString("{%1}").arg(varIterator.key());
				parameters = parameters.replace(varName, varIterator.value());

				varIterator++;
            }
			((AstCtiAction*)actionIterator.value())->setParameters(parameters);

			actionIterator++;
        }
    }
}

QString AstCtiCall::toXml()
{
    // Here we go for parameter substitution
    this->parseActionsParameters();

    QDomDocument doc("AstCTICallXML");
    QDomElement root = doc.createElement("call");
    root.setAttribute("uniqueid", this->uniqueId);
    doc.appendChild( root );

    // DestUniqueId
    if (this->destUniqueId.length() > 0) {
        QDomElement tag = doc.createElement("DestUniqueId");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->destUniqueId);
        tag.appendChild(t);
    }

    // Context
    if (this->context.length() > 0) {
        QDomElement tag = doc.createElement("Context");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->context);
        tag.appendChild(t);
    }

    // Channel
    if (this->channel.length() > 0) {
        QDomElement tag = doc.createElement("Channel");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->channel);
        tag.appendChild(t);
    }

    // ParsedChannel
    if (this->parsedChannel.length() > 0) {
        QDomElement tag = doc.createElement("ParsedChannel");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->parsedChannel);
        tag.appendChild(t);
    }

    // DestChannel
    if (this->destChannel.length() > 0) {
        QDomElement tag = doc.createElement("DestChannel");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->destChannel);
        tag.appendChild(t);
    }

    // ParsedChannelDest
    if (this->parsedDestChannel.length() > 0) {
        QDomElement tag = doc.createElement("ParsedDestChannel");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->parsedDestChannel);
        tag.appendChild(t);
    }

    // Callerid Name
    if (this->callerIdName.length() > 0) {
        QDomElement tag = doc.createElement("CallerIdName");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->callerIdName);
        tag.appendChild(t);
    }

    // Callerid Num
    if (this->callerIdNum.length() > 0) {
        QDomElement tag = doc.createElement("CallerIdNum");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->callerIdNum);
        tag.appendChild(t);
    }

	// Exten
	if (this->exten.length() > 0) {
		QDomElement tag = doc.createElement("Exten");
		root.appendChild(tag);
		QDomText t = doc.createTextNode(this->exten);
		tag.appendChild(t);
	}

	// Account code
	if (this->accountCode.length() > 0) {
		QDomElement tag = doc.createElement("AccountCode");
		root.appendChild(tag);
		QDomText t = doc.createTextNode(this->accountCode);
		tag.appendChild(t);
	}

	if (this->variables.count() > 0) {
        QDomElement xmlVars = doc.createElement("Variables");
		xmlVars.setAttribute("count", this->variables.count());
        root.appendChild(xmlVars);

		QHash<QString, QString>::const_iterator varIterator = this->variables.constBegin();
		while (varIterator != this->variables.constEnd()) {
			QDomElement tag = doc.createElement(varIterator.key());
            xmlVars.appendChild(tag);
			QDomText t = doc.createTextNode(varIterator.value());
            tag.appendChild(t);
			varIterator++;
        }
    }

	 if ((this->actions != 0) && (this->actions->count() > 0) ) {
        int actionsCount = 0;
        QDomElement xmlActions = doc.createElement("Actions");

		QMap<int, AstCtiAction*>::const_iterator actionsIterator = this->actions->constBegin();
		while (actionsIterator != this->actions->constEnd()) {
			AstCtiAction *action = (AstCtiAction *)actionsIterator.value();
			if (action->getOsType() == ActionOsAll ||
				action->getOsType() == this->clientOperatingSystem) {
                actionsCount++;
				QString elementName = AstCtiAction::getActionName(action->getActionType());

				QDomElement xmlAction = doc.createElement(elementName);

				QDomElement xmlDestination = doc.createElement("Destination");
				QDomText xmlDestinationVal = doc.createTextNode(action->getDestination());
				xmlDestination.appendChild(xmlDestinationVal);
				xmlAction.appendChild(xmlDestination);

				if (!action->getParameters().isEmpty()) {
					QDomElement xmlParameters = doc.createElement("Parameters");
					QDomText xmlParametersVal = doc.createTextNode(action->getParameters());
					xmlParameters.appendChild(xmlParametersVal);
					xmlAction.appendChild(xmlParameters);
				}

				if (!action->getMessageEncoding().isEmpty()) {
					QDomElement xmlEncoding = doc.createElement("Encoding");
					QDomText xmlEncodingVal = doc.createTextNode(action->getMessageEncoding());
					xmlEncoding.appendChild(xmlEncodingVal);
					xmlAction.appendChild(xmlEncoding);
				}

				// Append all to xmlAction
				xmlActions.appendChild(xmlAction);
            }
			actionsIterator++;
        }

		xmlActions.setAttribute("count", QString("%1").arg((int)actionsCount));
		root.appendChild(xmlActions);
	}

    return doc.toString();
}
