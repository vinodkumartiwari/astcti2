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
 * AstCTICallWidget.  If you copy code from other releases into a copy of GNU
 * AstCTICallWidget, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTICallWidget, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#ifndef ASTCTICALLWIDGET_H
#define ASTCTICALLWIDGET_H

#if defined(ASTCTICALLWIDGET_LIBRARY)
#  define ASTCTICALLWIDGET_EXPORT Q_DECL_EXPORT
#else
#  define ASTCTICALLWIDGET_EXPORT Q_DECL_IMPORT
#endif

#include <QtWidgets/QWidget>
#include <QtDesigner/QDesignerExportWidget>

namespace Ui
{
    class AstCTICallWidget;
}

class ASTCTICALLWIDGET_EXPORT AstCTICallWidget : public QWidget
{
    Q_DISABLE_COPY(AstCTICallWidget)
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
    explicit AstCTICallWidget(QWidget *parent = 0);
    ~AstCTICallWidget();

    //Enums need to be declared within the class
    //Otherwise, properties don't show in the designer

    enum CallDirection {
        CallDirectionIncoming,
        CallDirectionOutgoing
    };

    enum CallState {
        CallStateNone,
        CallStateRinging,
        CallStateBusy,
        CallStateInCall,
        CallStateOnHold
    };

    QString callerIdNumber() const;
    QString callerIdName() const;
    QString remarks() const;
    CallDirection callDirection() const;
    CallState callState() const;
    bool canAnswer() const;
    bool canHold() const;
    bool canConference() const;
    bool canTransfer() const;
    bool canRecord() const;
    bool canEditContact() const;

public slots:
    void setCallerIdNumber(const QString &cid);
    void setCallerIdName(const QString &cid);
    void setRemarks(const QString &rems);
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
    void editContact(const QString &number, const QString &name);

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::AstCTICallWidget *ui;

    CallDirection m_callDirection;
    CallState m_callState;
    bool m_canAnswer;
    bool m_canHold;
    bool m_canConference;
    bool m_canTransfer;
    bool m_canRecord;
    bool m_canEditContact;

    void enableAnswer();
    void enableHold();
    void enableConference();
    void enableTransfer();
    void enableRecord();
    void enableEditContact();

private slots:
    void editContactClicked();
};

#endif // ASTCTICALLWIDGET_H
