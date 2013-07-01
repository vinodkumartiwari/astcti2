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

#ifndef ASTCTICALL_H
#define ASTCTICALL_H

#include <QObject>
#include <QHash>
#include <QMap>
#include <QMetaType>

#include "astctiaction.h"

class AstCtiCall : public QObject
{
    Q_OBJECT

public:
	explicit AstCtiCall(const QString &uniqueId, QObject *parent=0);
	~AstCtiCall();

    QString                 &getChannel();
	void                    setChannel(const QString &channel);
    QString                 &getParsedChannel();
	void                    setParsedChannel(const QString &parsedChannel);

    QString                 &getDestChannel();
	void                    setDestChannel(const QString &channel);
    QString                 &getParsedDestChannel();
	void                    setParsedDestChannel(const QString &parsedChannel);

    QString                 &getCalleridNum();
	void                    setCalleridNum(const QString &callerIdNum);
    QString                 &getCalleridName();
	void                    setCalleridName(const QString &callerIdName);

	QString                 &getUniqueId();
	void                    setUniqueId(const QString &uniqueId);
    QString                 &getDestUniqueId();
	void                    setDestUniqueId(const QString &uniqueId);

    QString                 &getContext();
	void                    setContext(const QString &context);

	QString                 &getState();
	void                    setState(const QString &state);

	QString                 &getExten();
	void                    setExten(const QString &exten);

	QString                 &getAccountCode();
	void                    setAccountCode(const QString &accountCode);

	void                    addVariable(const QString &name, const QString &value);
	void                    setVariable(const QString &name, const QString &value);

	void                    setActions(QMap<int, AstCtiAction*> *callActions);
    void                    setOperatingSystem(QString operatingSystem);

    QString                 toXml();
    
private:
	Q_DISABLE_COPY(AstCtiCall)
	QString                         channel;
    QString                         parsedChannel;
    QString                         destChannel;
    QString                         parsedDestChannel;
    QString                         callerIdNum;
    QString                         callerIdName;
    QString                         uniqueId;
    QString                         destUniqueId;
    QString                         context;
	QString                         state;
	QString                         exten;
	QString                         accountCode;
	AstCtiActionOsType              clientOperatingSystem;

	QHash<QString, QString>         variables;
	QMap<int, AstCtiAction*>       *actions;

	QString                         parseChannel(const QString &channel);
    void                            parseActionsParameters();
};
Q_DECLARE_METATYPE(AstCtiCall*)

#endif // ASTCTICALL_H
