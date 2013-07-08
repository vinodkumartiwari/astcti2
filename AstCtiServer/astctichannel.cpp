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

#include <QStringList>
#include <QDomDocument>

#include "QsLog.h"
#include "astctichannel.h"

//Define static variables
int AstCtiChannel::nextBridgeId = 0;

AstCtiChannel::AstCtiChannel(const QString &uniqueId, QObject *parent) : QObject(parent)
{
	QLOG_TRACE() << "Creating new AstCtiChannel" << uniqueId;

	this->uniqueId = uniqueId;
	this->actions = 0;
	this->channel = "";
	this->parsedChannel = "";
	this->channelExten = "";
	this->callerIdNum = "";
	this->callerIdName = "";
	this->context = "";
	this->state = ChannelStateUnknown;
	this->exten = "";
	this->accountCode = "";
	this->associatedLocalChannel = "";
}

AstCtiChannel::~AstCtiChannel()
{
	QLOG_TRACE() << "Destroying AstCtiChannel" << this->uniqueId;
}

QString AstCtiChannel::getUniqueId() const
{
	return this->uniqueId;
}

void AstCtiChannel::setUniqueId(const QString &uniqueId)
{
	this->uniqueId = uniqueId;
}

QString AstCtiChannel::getChannel() const
{
    return this->channel;
}

void AstCtiChannel::setChannel(const QString &channel)
{
    this->channel = channel;

	QStringList channelParts;

	// Channel format is <technology>/<resource>-<identifier> (e.g. SIP/200-0899e2c8)
	// In case of phones, "resource" is a phone extension
	// We remove the identifier part by splitting the string on '-'
	if (channel.contains('-')) {
		channelParts = channel.split('-');
		if (channelParts.count() > 0)
			this->parsedChannel = channelParts.at(0);
	}

	// We get the extension by splitting the parsed channel on '/'
	channelParts = this->parsedChannel.split('/');
	if (channelParts.count() > 1)
		this->channelExten = channelParts.last();
}

QString AstCtiChannel::getParsedChannel() const
{
    return this->parsedChannel;
}

QString AstCtiChannel::getCalleridNum() const
{
    return this->callerIdNum;
}

void AstCtiChannel::setCalleridNum(const QString &callerIdNum)
{
	this->callerIdNum = callerIdNum;
	this->addVariable("CALLERID", callerIdNum);
}

QString AstCtiChannel::getCalleridName() const
{
    return this->callerIdName;
}

void AstCtiChannel::setCalleridName(const QString &callerIdName)
{
	this->callerIdName = callerIdName;
	this->addVariable("CALLERIDNAME", callerIdName);
}

QString AstCtiChannel::getContext() const
{
    return this->context;
}

void AstCtiChannel::setContext(const QString &context)
{
    this->context = context;
}

QString AstCtiChannel::getExten() const
{
	return this->exten;
}

void AstCtiChannel::setExten(const QString &exten)
{
	this->exten = exten;
}

AstCtiChannelState AstCtiChannel::getState() const
{
    return this->state;
}

void AstCtiChannel::setState(const AstCtiChannelState state)
{
    this->state = state;
}

QString AstCtiChannel::getAccountCode() const
{
	return this->accountCode;
}

void AstCtiChannel::setAccountCode(const QString &accountCode)
{
	this->accountCode = accountCode;
}

QString AstCtiChannel::getAssociatedLocalChannel() const
{
	return this->associatedLocalChannel;
}

void AstCtiChannel::setAssociatedLocalChannel(const QString &localChannel)
{
	this->associatedLocalChannel = localChannel;
}

bool AstCtiChannel::hasMatchingLocalChannel(const QString &localChannel)
{
	const int length1 = this->associatedLocalChannel.length();
	const int length2 = localChannel.length();
	if (length1 == length2 && length1 > 2 && length2 > 2) {
		const QStringRef channelName1 = this->associatedLocalChannel.leftRef(length1 - 2);
		const QStringRef channelName2 = localChannel.leftRef(length2 - 2);
		const QStringRef channelHalf1 = this->associatedLocalChannel.rightRef(1);
		const QStringRef channelHalf2 = localChannel.rightRef(1);
		if (channelName1 == channelName2 && channelHalf1 != channelHalf2)
			return true;
	}

	return false;
}

int AstCtiChannel::getBridgeId() const
{
	return this->bridgeId;
}

void AstCtiChannel::setBridgeId(const int bridgeId)
{
	this->bridgeId = bridgeId;
}

int AstCtiChannel::getNextBridgeId()
{
	return ++nextBridgeId;
}

QHash<QString, QString> *AstCtiChannel::getVariables()
{
	return &(this->variables);
}

void AstCtiChannel::addVariable(const QString &name, const QString &value)
{
	//There is no need to remove existing item,
	//insert will replace value if the key already exists
	this->variables.insert(name, value);
}

bool AstCtiChannel::setVariable(const QString &name, const QString &value)
{
	if (this->variables.contains(name)) {
		this->variables.insert(name, value);
		return true;
	}

	return false;
}

QMap<int, AstCtiAction*> *AstCtiChannel::getActions()
{
	return this->actions;
}

void AstCtiChannel::setActions(QMap<int, AstCtiAction*> *callActions) {

    this->actions = callActions;
}

void AstCtiChannel::setOperatingSystem(const QString &operatingSystem)
{
	this->clientOperatingSystem = AstCtiAction::parseOsType(operatingSystem);
}

void AstCtiChannel::parseActionParameters()
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

QString AstCtiChannel::toXml()
{
	QDomDocument xmlDoc("AstCtiChannel");

	QDomElement xmlChannel = xmlDoc.createElement("Channel");
	xmlChannel.setAttribute("UniqueId", this->uniqueId);
	xmlDoc.appendChild(xmlChannel);

	// Here we go for parameter substitution
	this->parseActionParameters();

	QDomElement xmlElement;
	QDomText xmlText;

	// Channel
	xmlElement = xmlDoc.createElement("Channel");
	xmlChannel.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(this->channel);
	xmlElement.appendChild(xmlText);

	// ParsedChannel
	xmlElement = xmlDoc.createElement("ParsedChannel");
	xmlChannel.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(this->parsedChannel);
	xmlElement.appendChild(xmlText);

	// Callerid Num
	xmlElement = xmlDoc.createElement("CallerIdNum");
	xmlChannel.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(this->calleridNum);
	xmlElement.appendChild(xmlText);

	// Callerid Name
	xmlElement = xmlDoc.createElement("CallerIdName");
	xmlChannel.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(this->calleridName);
	xmlElement.appendChild(xmlText);

	// Context
	xmlElement = xmlDoc.createElement("Context");
	xmlChannel.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(this->context);
	xmlElement.appendChild(xmlText);

	// Exten
	xmlElement = xmlDoc.createElement("Exten");
	xmlChannel.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(this->exten);
	xmlElement.appendChild(xmlText);

	// State
	xmlElement = xmlDoc.createElement("State");
	xmlChannel.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(QString::number(this->state));
	xmlElement.appendChild(xmlText);

	// Account code
	xmlElement = xmlDoc.createElement("AccountCode");
	xmlChannel.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(this->accountCode);
	xmlElement.appendChild(xmlText);

	// Bridge ID
	xmlElement = xmlDoc.createElement("BridgeId");
	xmlChannel.appendChild(xmlElement);
	xmlText = xmlDoc.createTextNode(QString::number(this->bridgeId));
	xmlElement.appendChild(xmlText);

	if (this->variables->size() > 0) {
		QDomElement xmlVars = xmlDoc.createElement("Variables");
		xmlVars.setAttribute("Count", this->variables->size());
		xmlChannel.appendChild(xmlVars);

		QHash<QString, QString>::const_iterator varIterator = this->variables.constBegin();
		while (varIterator != this->variables.constEnd()) {
			xmlElement = xmlDoc.createElement(varIterator.key());
			xmlVars.appendChild(xmlElement);
			xmlText = xmlDoc.createTextNode(varIterator.value());
			xmlElement.appendChild(xmlText);
			varIterator++;
		}
	}

	 if (this->actions != 0 && this->actions->size() > 0) {
		int actionsCount = 0;
		QDomElement xmlActions = xmlDoc.createElement("Actions");

		QMap<int, AstCtiAction*>::const_iterator actionsIterator = this->actions->constBegin();
		while (actionsIterator != this->actions->constEnd()) {
			AstCtiAction *action = actionsIterator.value();
			if (action->getOsType() == ActionOsAll ||
				action->getOsType() == this->clientOperatingSystem) {
				actionsCount++;
				QString elementName = AstCtiAction::getActionName(action->getActionType());

				QDomElement xmlAction = xmlDoc.createElement(elementName);

				xmlElement = xmlDoc.createElement("Destination");
				xmlAction.appendChild(xmlElement);
				xmlText = xmlDoc.createTextNode(action->getDestination());
				xmlElement.appendChild(xmlText);

				if (!action->getParameters().isEmpty()) {
					xmlElement = xmlDoc.createElement("Parameters");
					xmlAction.appendChild(xmlElement);
					xmlText = xmlDoc.createTextNode(action->getParameters());
					xmlElement.appendChild(xmlText);
				}

				if (!action->getMessageEncoding().isEmpty()) {
					xmlElement = xmlDoc.createElement("Encoding");
					xmlAction.appendChild(xmlElement);
					xmlText = xmlDoc.createTextNode(action->getMessageEncoding());
					xmlElement.appendChild(xmlText);
				}

				xmlActions.appendChild(xmlAction);
			}
			actionsIterator++;
		}

		xmlActions.setAttribute("Count", actionsCount);
		xmlChannel.appendChild(xmlActions);
	}

	return xmlDoc.toString();
}
