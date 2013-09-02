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

#include <QVariantList>

#include "QsLog.h"
#include "db.h"
#include "astctiseat.h"

AstCtiSeat::AstCtiSeat(const int id, const QString& mac, const QString& description,
					   QObject* parent)
	: QObject(parent)
{
	QLOG_TRACE() << "Creating new AstCtiSeat" << id << description;

	this->id = id;
	this->mac = mac;
	this->description = description;
	//this->extensions = AstCtiExtensionList();
}

AstCtiSeat::~AstCtiSeat()
{
	QLOG_TRACE() << "Destroying AstCtiSeat" << this->id << this->description;

	qDeleteAll(this->extensions);
}

const int AstCtiSeat::getId() const
{
	return this->id;
}

const QString& AstCtiSeat::getMac() const
{
	return this->mac;
}

const QString& AstCtiSeat::getDescription() const
{
    return this->description;
}

const bool AstCtiSeat::loadExtensions()
{
	QLOG_TRACE() << "Loading extensions for seat" << this->id << this->description;

	this->extensions.clear();

	bool ok;
	QVariantList params;
	params.append(this->id);
	const QVariantTable extenData =
			DB::readTable("SELECT channel,description,can_auto_answer FROM extensions "
						  "WHERE id_seat=? AND enabled=true ORDER BY channel", params, &ok);
	// Order by channel so order will be the same when comparing two seats (see compareExtensions)
	if (ok) {
		const int listSize = extenData.size();
		for (int i = 0; i < listSize; i++) {
			const QVariantList extenRow = extenData.at(i);
			AstCtiExtension* extension = new AstCtiExtension();
			QString channel = extenRow.at(0).toString();
			extension->channelName = channel;
			// We get the extension number by splitting the parsed channel on '/'
			QStringList channelParts = channel.split('/');
			if (!channelParts.isEmpty())
				extension->number = channelParts.last();
			else
				extension->number = channel;
			extension->name = extenRow.at(1).toString();
			extension->canAutoAnswer = extenRow.at(2).toBool();
			this->extensions.append(extension);
		}
	} else {
		QLOG_ERROR() << "Loading extensions failed for seat" << this->id << this->description;
	}

	return ok;
}

const AstCtiExtensionList& AstCtiSeat::getExtensions() const
{
	return this->extensions;
}

const QStringList AstCtiSeat::getExtensionNumbers() const
{
	QStringList extNumbers;
	const int listSize = this->extensions.size();
	for (int i = 0; i < listSize; i++)
		extNumbers.append(this->extensions.at(i)->number);
	return extNumbers;
}

const bool AstCtiSeat::hasExtension(const QString& channelName) const
{
	return this->getExtension(channelName) != 0;
}

const bool AstCtiSeat::hasQueue(const QString& channelName, const QString& queue) const
{
	AstCtiExtension* const extension = this->getExtension(channelName);
	if (extension != 0)
		return extension->queues.contains(queue);

	return false;
}

const bool AstCtiSeat::compareExtensions(const AstCtiExtensionList& newExtensions) const
{
	const int listSize = this->extensions.size();
	if (listSize != newExtensions.size())
		return false;

	for (int i = 0; i < listSize; i++)
		if (this->extensions.at(i)->channelName != newExtensions.at(i)->channelName)
			return false;

	return true;
}

void AstCtiSeat::setExtensionUserAgent(const QString& channelName, const QString& userAgent)
{
	AstCtiExtension* const extension = this->getExtension(channelName);
	if (extension != 0)
		extension->userAgent = userAgent;
}

void AstCtiSeat::setExtensionStatus(const QString& channelName, const AstCtiExtensionStatus status)
{
	AstCtiExtension* const extension = this->getExtension(channelName);
	if (extension != 0)
		extension->status = status;
}

const AstCtiAgentStatus AstCtiSeat::getAgentStatus(const QString &channelName,
												   const QString &queue) const
{
	AstCtiExtension* const extension = this->getExtension(channelName);
	return extension->queues.value(queue);
}

void AstCtiSeat::setAgentStatus(const QString& channelName, const QString& queue,
								const AstCtiAgentStatus status)
{
	AstCtiExtension* const extension = this->getExtension(channelName);
	if (extension != 0)
		if (extension->queues.contains(queue))
			extension->queues.insert(queue, status);
}

AstCtiExtension* const AstCtiSeat::getExtension(const QString& channelName) const
{
	const int listSize = this->extensions.size();
	for (int i = 0; i < listSize; i++) {
		AstCtiExtension* const extension = this->extensions.at(i);
		if (extension->channelName == channelName)
			return extension;
	}

	return 0;
}
