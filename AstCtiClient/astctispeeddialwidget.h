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

#ifndef ASTCTISPEEDDIALWIDGET_H
#define ASTCTISPEEDDIALWIDGET_H

#include <QtWidgets/QWidget>

namespace Ui {
	class AstCtiSpeedDialWidget;
}

class AstCtiSpeedDialWidget : public QWidget
{
    Q_OBJECT
    Q_ENUMS(CallForward ExtensionState)
    Q_PROPERTY( QString name READ name WRITE setName )
    Q_PROPERTY( QString number READ number WRITE setNumber )
    Q_PROPERTY( bool showNumber READ showNumber WRITE setShowNumber )
    Q_PROPERTY( QString group READ group WRITE setGroup )
    Q_PROPERTY( bool busyLampField READ busyLampField WRITE setBusyLampField )
    Q_PROPERTY( ExtensionState extensionState READ extensionState WRITE setExtensionState )
    Q_PROPERTY( bool doNotDisturb READ doNotDisturb WRITE setDoNotDisturb )
    Q_PROPERTY( bool callWaiting READ callWaiting WRITE setCallWaiting )
    Q_PROPERTY( CallForward callForward READ callForward WRITE setCallForward )
    Q_PROPERTY( QString callForwardNumber READ callForwardNumber WRITE setCallForwardNumber )
    Q_PROPERTY( int voicemail READ voicemail WRITE setVoicemail )

public:
	explicit AstCtiSpeedDialWidget(QWidget* parent = 0);
	~AstCtiSpeedDialWidget();

    //Enums need to be declared within the class
    //Otherwise, properties don't show in the designer

    enum CallForward {
        CallForwardNone = 0,
        CallForwardUnconditional = 1,
        CallForwardOnBusy = 2,
        CallForwardOnNoAnswer = 4
    };

    enum ExtensionState {
        ExtensionStateUnknown,
        ExtensionStateDisconnected,
        ExtensionStateIdle,
        ExtensionStateRinging,
        ExtensionStateBusy
    };

	const QString& name() const;
	const QString& number() const;
	const bool showNumber() const;
	const QString& group() const;
	const bool busyLampField() const;
	const ExtensionState extensionState() const;
	const bool doNotDisturb() const;
	const bool callWaiting() const;
	const CallForward callForward() const;
	const QString& callForwardNumber() const;
	const int voicemail() const;

public slots:
	void setName(const QString& name);
	void setNumber(const QString& number);
	void setShowNumber(const bool showNumber);
	void setGroup(const QString& group);
    void setBusyLampField(const bool blf);
    void setExtensionState(const ExtensionState state);
    void setDoNotDisturb(const bool dnd);
    void setCallWaiting(const bool cw);
    void setCallForward(const CallForward cf);
	void setCallForwardNumber(const QString& cfNumber);
	void setVoicemail(const int vmCount);

signals:
    void dial(const QString& number);
    void takeCall(const QString& number);

protected:
    void paintEvent(QPaintEvent* );
    void resizeEvent(QResizeEvent* );
    bool eventFilter(QObject* , QEvent* e);

private:
	Q_DISABLE_COPY(AstCtiSpeedDialWidget)

    void setCaption();
    void setStatusText();
	const QString extensionStateToString();
	const QString callForwardToString();

	Ui::AstCtiSpeedDialWidget* ui;

    QString m_name;
    QString m_number;
    bool m_showNumber;
    QString m_group;
    bool m_busyLampField;
    ExtensionState m_extensionState;
    bool m_doNotDisturb;
    bool m_callWaiting;
    CallForward m_callForward;
    QString m_CallForwardNumber;
    int m_voicemail;
};

#endif // ASTCTISPEEDDIALWIDGET_H
