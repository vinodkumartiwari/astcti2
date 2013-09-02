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

#ifndef ASTCTICALLWIDGET_H
#define ASTCTICALLWIDGET_H

#include <QtWidgets/QWidget>
#include <QtDesigner/QDesignerExportWidget>

namespace Ui
{
	class AstCtiCallWidget;
}

class AstCtiCallWidget : public QWidget
{
    Q_OBJECT
    Q_ENUMS(CallDirection CallState)
    Q_PROPERTY( QString callerIdNumber READ callerIdNumber WRITE setCallerIdNumber )
    Q_PROPERTY( QString callerIdName READ callerIdName WRITE setCallerIdName )
    Q_PROPERTY( QString remarks READ remarks WRITE setRemarks )
    Q_PROPERTY( CallDirection callDirection READ callDirection WRITE setCallDirection )
    Q_PROPERTY( CallState callState READ callState WRITE setCallState )
    Q_PROPERTY( bool canAnswer READ canAnswer WRITE setCanAnswer )
    Q_PROPERTY( bool canHold READ canHold WRITE setCanHold )
    Q_PROPERTY( bool canConference READ canConference WRITE setCanConference )
    Q_PROPERTY( bool canTransfer READ canTransfer WRITE setCanTransfer )
    Q_PROPERTY( bool canRecord READ canRecord WRITE setCanRecord )
    Q_PROPERTY( bool canEditContact READ canEditContact WRITE setCanEditContact )

public:
	explicit AstCtiCallWidget(QWidget* parent = 0);
	~AstCtiCallWidget();

    //Enums need to be declared within the class
    //Otherwise, properties don't show in the designer

    enum CallDirection {
        CallDirectionIncoming,
        CallDirectionOutgoing
    };

    enum CallState {
        CallStateNone,
		CallStateDialing,
		CallStateRinging,
        CallStateBusy,
        CallStateInCall,
        CallStateOnHold
    };

	const QString callerIdNumber() const;
	const QString callerIdName() const;
	const QString remarks() const;
	const CallDirection callDirection() const;
	const CallState callState() const;
	const bool canAnswer() const;
	const bool canHold() const;
	const bool canConference() const;
	const bool canTransfer() const;
	const bool canRecord() const;
	const bool canEditContact() const;

	static CallState callStateFromString(const QString& state);

public slots:
	void setCallerIdNumber(const QString& callerIdNumber);
	void setCallerIdName(const QString& callerIdName);
	void setRemarks(const QString& remarks);
    void setCallDirection(const CallDirection direction);
    void setCallState(const CallState state);
    void setCanAnswer(const bool value);
    void setCanHold(const bool value);
    void setCanConference(const bool value);
    void setCanTransfer(const bool value);
    void setCanRecord(const bool value);
    void setCanEditContact(const bool value);

signals:
    void answerHangup();
    void holdResume();
    void conference();
    void transfer();
    void record();
    void editContact(const QString& number, const QString& name);

protected:
    void paintEvent(QPaintEvent* );

private:
	Q_DISABLE_COPY(AstCtiCallWidget)

	void enableAnswer();
	void enableHold();
	void enableConference();
	void enableTransfer();
	void enableRecord();
	void enableEditContact();

	Ui::AstCtiCallWidget* ui;

    CallDirection m_callDirection;
    CallState m_callState;
    bool m_canAnswer;
    bool m_canHold;
    bool m_canConference;
    bool m_canTransfer;
    bool m_canRecord;
    bool m_canEditContact;

private slots:
    void editContactClicked();
};

#endif // ASTCTICALLWIDGET_H
