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

#ifndef ASTCTICHANNEL_H
#define ASTCTICHANNEL_H

#include <QObject>
#include <QHash>
#include <QMap>
#include <QDateTime>
#include <QMetaType>

#include "astctiaction.h"

typedef QHash<QString, QString> QStringHash;

//	Value   State            Description
//	0	    Down             Channel is down and available
//	1	    Rsrvd            Channel is down, but reserved
//	2	    OffHook          Channel is off hook
//	3	    Dialing          The channel is in the midst of a dialing operation
//	4	    Ring             The channel is ringing
//	5	    Ringing          The remote endpoint is ringing. Note that for many channel
//                           technologies, this is the same as Ring.
//	6	    Up               A communication path is established between the endpoint
//                           and Asterisk
//	7	    Busy             A busy indication has occurred on the channel
//	8	    Dialing Offhook  Digits (or equivalent) have been dialed while offhook
//	9	    Pre-ring         The channel technology has detected an incoming call
//                           and is waiting for a ringing indication
//	10	    Unknown          The channel is an unknown state
enum AstCtiChannelState {
	ChannelStateDown = 0,
	ChannelStateRsrvd = 1,
	ChannelStateOffHook = 2,
	ChannelStateDialing = 3,
	ChannelStateRing = 4,
	ChannelStateRinging = 5,
	ChannelStateUp = 6,
	ChannelStateBusy = 7,
	ChannelStateDialingOffhook = 8,
	ChannelStatePrering = 9,
	ChannelStateUnknown = 10
};

class AstCtiChannel : public QObject
{
    Q_OBJECT

public:
	explicit AstCtiChannel(const QString& uniqueId, QObject* parent = 0);
	~AstCtiChannel();

	const QString&           getUniqueId() const;
	void                     setUniqueId(const QString& uniqueId);

	const QString&           getChannelId() const;
	void                     setChannelId(const QString& channelId);
	const QString&           getChannelName() const;
	const QString&           getNumber() const;

	const QString&           getCalleridNum() const;
	void                     setCalleridNum(const QString& callerIdNum);
	const QString&           getCalleridName() const;
	void                     setCalleridName(const QString& callerIdName);

	const QString&           getContext() const;
	void                     setContext(const QString& context);
	const QString&           getDialedLineNum() const;
	void                     setDialedLineNum(const QString& dialedLineNum);
	const QString&           getConnectedLineNum() const;
	void                     setConnectedLineNum(const QString& connectedLineNum);

	const QString&           getApplication() const;
	void                     setApplication(const QString& application);
	const QString&           getApplicationData() const;
	void                     setApplicationData(const QString& applicationData);

	const QString&           getQueue() const;
	void                     setQueue(const QString& queue);

	const AstCtiChannelState getState() const;
	void                     setState(const AstCtiChannelState state);
	static QString           channelStateToString(const AstCtiChannelState state);

	const QString&           getAccountCode() const;
	void                     setAccountCode(const QString& accountCode);

	const QString&           getMusicOnHoldState() const;
	void                     setMusicOnHoldState(const QString& state);

	const QString&           getHangupCause() const;
	void                     setHangupCause(const QString& hangupCause);

	const QString&           getAssociatedLocalChannel() const;
	void                     setAssociatedLocalChannel(const QString& localChannel);
	const bool               hasMatchingLocalChannel(const QString& localChannel) const;

	const QDateTime&         getStartTime() const;
	void                     setStartTime(const QDateTime& startTime);

	const int                getBridgeId() const;
	void                     setBridgeId(const int bridgeId);
	static int               getNextBridgeId();

	void                     addVariable(const QString& name, const QString& value);
	const bool               setVariable(const QString& name, const QString& value);

	void                     setActions(const AstCtiActionMap callActions);

	void                     setOperatingSystem(const QString& operatingSystem);

	const QString            toXml(const QString& eventName);

private:
	Q_DISABLE_COPY(AstCtiChannel)

	void                parseActionParameters();

	QString             uniqueId;
	QString             channelId;
	QString             channelName;
	QString             number;
	QString             callerIdNum;
	QString             callerIdName;
	QString             context;
	QString             dialedLineNum;
	QString             connectedLineNum;
	QString             application;
	QString             applicationData;
	QString             queue;
	AstCtiChannelState  state;
	QString             accountCode;
	QString             musicOnHoldState;
	QString             hangupCause;
	QString             associatedLocalChannel;
	QDateTime           startTime;
	int                 bridgeId;
	AstCtiActionOsType  clientOperatingSystem;
	QStringHash         variables;
	AstCtiActionMap     actions;
	static int          nextBridgeId;
};
Q_DECLARE_METATYPE(AstCtiChannel*)

#endif // ASTCTICHANNEL_H
