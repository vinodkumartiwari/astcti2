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
#include <QXmlStreamWriter>

#include "QsLog.h"
#include "astctichannel.h"

//Define static variables
int AstCtiChannel::nextBridgeId = 0;

AstCtiChannel::AstCtiChannel(const QString& uniqueId, QObject* parent) : QObject(parent)
{
	QLOG_TRACE() << "Creating new AstCtiChannel" << uniqueId;

	this->uniqueId = uniqueId;
	//this->actions = 0;
	this->channelId = QStringLiteral("");
	this->channelName = QStringLiteral("");
	this->number = QStringLiteral("");
	this->callerIdNum = QStringLiteral("");
	this->callerIdName = QStringLiteral("");
	this->context = QStringLiteral("");
	this->state = ChannelStateUnknown;
	this->dialedLineNum = QStringLiteral("");
	this->connectedLineNum = QStringLiteral("");
	this->accountCode = QStringLiteral("");
	this->musicOnHoldState = QStringLiteral("Stop");
	this->hangupCause = QStringLiteral("");
	this->associatedLocalChannel = QStringLiteral("");
}

AstCtiChannel::~AstCtiChannel()
{
	QLOG_TRACE() << "Destroying AstCtiChannel" << this->uniqueId;
}

const QString& AstCtiChannel::getUniqueId() const
{
	return this->uniqueId;
}

void AstCtiChannel::setUniqueId(const QString& uniqueId)
{
	this->uniqueId = uniqueId;
}

const QString& AstCtiChannel::getChannelId() const
{
    return this->channelId;
}

void AstCtiChannel::setChannelId(const QString& channelId)
{
	this->channelId = channelId;

	// Channel format is <technology>/<resource>-<identifier> (e.g. SIP/200-0899e2c8)
	// <technology>/<resource> pair represents actual channel name, while identifier
	// is the id of the current channel (there could be multiple simultaneous audio channels).
	// We remove the identifier part by splitting the string on hyphen
	QStringList channelParts = channelId.split('-');
	if (channelParts.size() > 1) {
		// Theoretically, <resource> could contain a hyphen, so we just remove the identifier part
		channelParts.removeLast();
		this->channelName = channelParts.join('-');
	} else {
		this->channelName = channelId;
	}

	// In case of phones, "resource" is a phone number.
	// Actual extension number used to reach this channel DOES NOT have to be the same
	// (i.e. one could use: exten => 222,1,Dial(SIP/200)),
	// but common practice is to make those numbers the same.
	// We get the phone number by splitting the channel name on slash
	channelParts = this->channelName.split('/');
	if (channelParts.size() > 1)
		this->number = channelParts.last();
	else
		this->number = this->channelName;
}

const QString& AstCtiChannel::getChannelName() const
{
    return this->channelName;
}

const QString& AstCtiChannel::getNumber() const
{
	return this->number;
}

const QString& AstCtiChannel::getCalleridNum() const
{
    return this->callerIdNum;
}

void AstCtiChannel::setCalleridNum(const QString& callerIdNum)
{
	this->callerIdNum = callerIdNum;
	this->addVariable(QStringLiteral("CALLERID"), callerIdNum);
}

const QString& AstCtiChannel::getCalleridName() const
{
    return this->callerIdName;
}

void AstCtiChannel::setCalleridName(const QString& callerIdName)
{
	this->callerIdName = callerIdName;
	this->addVariable(QStringLiteral("CALLERIDNAME"), callerIdName);
}

const QString& AstCtiChannel::getContext() const
{
    return this->context;
}

void AstCtiChannel::setContext(const QString& context)
{
    this->context = context;
}

const QString& AstCtiChannel::getDialedLineNum() const
{
	return this->dialedLineNum;
}

void AstCtiChannel::setDialedLineNum(const QString& dialedLineNum)
{
	this->dialedLineNum = dialedLineNum;
}

const QString& AstCtiChannel::getConnectedLineNum() const
{
	return this->connectedLineNum;
}

void AstCtiChannel::setConnectedLineNum(const QString& connectedLineNum)
{
	this->connectedLineNum = connectedLineNum;
}

const QString& AstCtiChannel::getApplication() const
{
	return this->application;
}

void AstCtiChannel::setApplication(const QString& application)
{
	this->application = application;
}

const QString& AstCtiChannel::getApplicationData() const
{
	return this->applicationData;
}

void AstCtiChannel::setApplicationData(const QString& applicationData)
{
	this->applicationData = applicationData;
}

const QString& AstCtiChannel::getQueue() const
{
	return this->queue;
}

void AstCtiChannel::setQueue(const QString& queue)
{
	this->queue = queue;
}

const AstCtiChannelState AstCtiChannel::getState() const
{
    return this->state;
}

void AstCtiChannel::setState(const AstCtiChannelState state)
{
    this->state = state;
}

QString AstCtiChannel::channelStateToString(const AstCtiChannelState state)
{
	//We use a variable to exploit NRVO
	QString stateName;

	switch (state) {
	case ChannelStateDown:
		stateName = QStringLiteral("Down");
		break;
	case ChannelStateRsrvd:
		stateName = QStringLiteral("Rsrvd");
		break;
	case ChannelStateOffHook:
		stateName = QStringLiteral("OffHook");
		break;
	case ChannelStateDialing:
		stateName = QStringLiteral("Dialing");
		break;
	case ChannelStateRing:
		stateName = QStringLiteral("Ring");
		break;
	case ChannelStateRinging:
		stateName = QStringLiteral("Ringing");
		break;
	case ChannelStateUp:
		stateName = QStringLiteral("Up");
		break;
	case ChannelStateBusy:
		stateName = QStringLiteral("Busy");
		break;
	case ChannelStateDialingOffhook:
		stateName = QStringLiteral("DialingOffhook");
		break;
	case ChannelStatePrering:
		stateName = QStringLiteral("Prering");
		break;
	case ChannelStateUnknown:
		stateName = QStringLiteral("Unknown");
		break;
	default:
		stateName = QStringLiteral("");
		break;
	}

	return stateName;
}

const QString& AstCtiChannel::getAccountCode() const
{
	return this->accountCode;
}

void AstCtiChannel::setAccountCode(const QString& accountCode)
{
	this->accountCode = accountCode;
}

const QString& AstCtiChannel::getMusicOnHoldState() const
{
	return this->musicOnHoldState;
}

void AstCtiChannel::setMusicOnHoldState(const QString& state)
{
	this->musicOnHoldState = state;
}

const QString& AstCtiChannel::getHangupCause() const
{
	return this->hangupCause;
}

void AstCtiChannel::setHangupCause(const QString& hangupCause)
{
	this->hangupCause = hangupCause;
}

const QString& AstCtiChannel::getAssociatedLocalChannel() const
{
	return this->associatedLocalChannel;
}

void AstCtiChannel::setAssociatedLocalChannel(const QString& localChannel)
{
	this->associatedLocalChannel = localChannel;
}

const bool AstCtiChannel::hasMatchingLocalChannel(const QString& localChannel) const
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

const QDateTime& AstCtiChannel::getStartTime() const
{
	return this->startTime;
}

void AstCtiChannel::setStartTime(const QDateTime &startTime)
{
	this->startTime = startTime;
}

const int AstCtiChannel::getBridgeId() const
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

void AstCtiChannel::addVariable(const QString& name, const QString& value)
{
	//There is no need to remove existing item,
	//insert will replace value if the key already exists
	this->variables.insert(name, value);
}

const bool AstCtiChannel::setVariable(const QString& name, const QString& value)
{
	if (this->variables.contains(name)) {
		this->variables.insert(name, value);
		return true;
	}

	return false;
}

void AstCtiChannel::setActions(const AstCtiActionMap callActions) {

    this->actions = callActions;
}

void AstCtiChannel::setOperatingSystem(const QString& operatingSystem)
{
	this->clientOperatingSystem = AstCtiAction::parseOsType(operatingSystem);
}

void AstCtiChannel::parseActionParameters()
{
	if (this->actions.size() > 0 && this->variables.size() > 0) {
		for (AstCtiActionMap::const_iterator i = this->actions.constBegin();
			 i != this->actions.constEnd();
			 i++) {
			QString parameters = i.value()->getParameters();

			for (QStringHash::const_iterator j = this->variables.constBegin();
				 j != this->variables.constEnd();
				 j++) {

				QString varName = QString("{%1}").arg(j.key());
				parameters = parameters.replace(varName, j.value());
            }
			((AstCtiAction*)i.value())->setParameters(parameters);
        }
    }
}

const QString AstCtiChannel::toXml(const QString& eventName)
{
	QString xmlString;

	// Do the parameter substitution
	this->parseActionParameters();

	QXmlStreamWriter writer(&xmlString);

	writer.writeStartDocument();

	writer.writeStartElement(QStringLiteral("AsteriskChannel"));
	writer.writeAttribute(QStringLiteral("UniqueId"), this->uniqueId);

	// XML object is sent to client in response to some event
	// so we let the client know which event it is
	if (!eventName.isEmpty())
		writer.writeTextElement(QStringLiteral("Event"), eventName);

	writer.writeTextElement(QStringLiteral("Channel"), this->channelId);
	writer.writeTextElement(QStringLiteral("ParsedChannel"), this->channelName);
	writer.writeTextElement(QStringLiteral("ChannelExten"), this->number);
	writer.writeTextElement(QStringLiteral("CallerIdNum"), this->callerIdNum);
	writer.writeTextElement(QStringLiteral("CallerIdName"), this->callerIdName);
	writer.writeTextElement(QStringLiteral("Context"), this->context);
	writer.writeTextElement(QStringLiteral("DialedLineNum"), this->dialedLineNum);
	writer.writeTextElement(QStringLiteral("ConnectedLineNum"), this->dialedLineNum);
	writer.writeTextElement(QStringLiteral("Queue"), this->queue);
	writer.writeTextElement(QStringLiteral("State"), channelStateToString(this->state));
	writer.writeTextElement(QStringLiteral("AccountCode"), this->accountCode);
	writer.writeTextElement(QStringLiteral("MusicOnHoldState"), this->musicOnHoldState);
	writer.writeTextElement(QStringLiteral("HangupCause"), this->hangupCause);
	writer.writeTextElement(QStringLiteral("BridgeId"), QString::number(this->bridgeId));

	if (this->variables.size() > 0) {
		writer.writeStartElement(QStringLiteral("Variables"));

		for (QStringHash::const_iterator i = this->variables.constBegin();
			 i != this->variables.constEnd();
			 i++)
			writer.writeTextElement(i.key(), i.value());

		writer.writeEndElement(); // Variables
	}

	 if (this->actions.size() > 0) {
		 writer.writeStartElement(QStringLiteral("Actions"));

		for (AstCtiActionMap::const_iterator i = this->actions.constBegin();
			 i != this->actions.constEnd();
			 i++) {
			AstCtiAction* action = i.value();
			if (action->getOsType() == ActionOsAll ||
				action->getOsType() == this->clientOperatingSystem) {

				QString elementName = AstCtiAction::getActionName(action->getActionType());

				writer.writeStartElement(elementName);

				writer.writeTextElement(QStringLiteral("Destination"), action->getDestination());
				if (!action->getParameters().isEmpty())
					writer.writeTextElement(QStringLiteral("Parameters"), action->getParameters());
				if (!action->getMessageEncoding().isEmpty())
					writer.writeTextElement(QStringLiteral("Encoding"),
											action->getMessageEncoding());

				writer.writeEndElement(); // Action
			}
		}

		writer.writeEndElement(); // Actions
	}

	 writer.writeEndElement(); // AsteriskChannel
	 writer.writeEndDocument();

	 return xmlString;
}
