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

#ifndef ASTCTISEAT_H
#define ASTCTISEAT_H

#include <QObject>
#include <QList>
#include <QXmlStreamWriter>

#include "astctiextension.h"

class AstCtiSeat : public QObject
{
    Q_OBJECT

public:
	explicit AstCtiSeat(const int id, const QString& mac, const QString& description,
						QObject* parent = 0);
	~AstCtiSeat();

	const int                  getId() const;
	const QString&             getMac() const;
	const QString&             getDescription() const;
	const bool                 loadExtensions();
	const AstCtiExtensionList& getExtensions() const;
	const QStringList          getExtensionNumbers() const;
	const bool                 compareExtensions(const AstCtiExtensionList& newExtensions) const;
	const bool                 hasExtension(const QString& channelName) const;
	const bool                 hasQueue(const QString& channelName, const QString& queue) const;
	void                       setExtensionUserAgent(const QString& channelName,
													 const QString& userAgent);
	void                       setExtensionStatus(const QString& channelName,
												  const AstCtiExtensionStatus status);
	const AstCtiAgentStatus    getAgentStatus(const QString& channelName,
											  const QString& queue) const;
	void                       setAgentStatus(const QString& channelName,
											  const QString& queue,
											  const AstCtiAgentStatus status);

private:
	Q_DISABLE_COPY(AstCtiSeat)

	AstCtiExtension* const getExtension(const QString& channelName) const;

	int                  id;
	QString              mac;
	QString              description;
	AstCtiExtensionList  extensions;
};

typedef QHash<int, AstCtiSeat*> AstCtiSeatHash;

#endif // ASTCTISEAT_H
