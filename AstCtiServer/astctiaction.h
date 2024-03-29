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

#ifndef ASTCTIACTION_H
#define ASTCTIACTION_H

#include <QObject>

enum AstCtiActionType {
    ActionApplication,
    ActionBrowser,
    ActionInternalBrowser,
    ActionTcpMessage,
	ActionUdpMessage
};

enum AstCtiActionOsType {
    ActionOsAll,
    ActionOsLinux,
    ActionOsMacintosh,
	ActionOsWindows
};

class AstCtiAction : public QObject
{
    Q_OBJECT
	Q_ENUMS(AstCtiActionType)
	Q_ENUMS(AstCtiActionOsType)

public:
	explicit AstCtiAction(const int id, const QString& osType, const QString& actionType,
						  const QString& destination, const QString& parameters,
						  const QString& messageEncoding, QObject* parent = 0);
	~AstCtiAction();

	const int                 getId() const;
	const AstCtiActionOsType  getOsType() const;
	const AstCtiActionType    getActionType() const;
	const QString&            getDestination() const;
	const QString&            getParameters() const;
	const QString&            getMessageEncoding() const;
	void                      setParameters(const QString& newParameters);

	static QString            getActionName(const AstCtiActionType actionType);
	static AstCtiActionType   parseActionType(const QString& actionTypeString);
	static AstCtiActionOsType parseOsType(const QString& osTypeString);

private:
	Q_DISABLE_COPY(AstCtiAction)

	int                 id;
	AstCtiActionOsType  osType;
	AstCtiActionType    actionType;
	QString             destination;
	QString             parameters;
	QString             messageEncoding;
};

typedef QHash<int, AstCtiAction*> AstCtiActionHash;
typedef QMap<int, AstCtiAction*> AstCtiActionMap;

#endif // ASTCTIACTION_H
