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

#include <QtWidgets/QStyleOption>
#include <QtGui/QPainter>

#include "astcticallwidget.h"
#include "ui_astcticallwidget.h"

AstCtiCallWidget::AstCtiCallWidget(QWidget *parent) :
    QWidget(parent),
	ui(new Ui::AstCtiCallWidget)
{
    ui->setupUi(this);

    m_callDirection = AstCtiCallWidget::CallDirectionOutgoing;
    m_callState = AstCtiCallWidget::CallStateNone;
    m_canAnswer = false;
    m_canHold = false;
    m_canConference = false;
    m_canTransfer = false;
    m_canRecord = false;
    m_canEditContact = false;

    connect(ui->callToolButton, SIGNAL(clicked()), this, SIGNAL(answerHangup()));
    connect(ui->holdToolButton, SIGNAL(clicked()), this, SIGNAL(holdResume()));
    connect(ui->conferenceToolButton, SIGNAL(clicked()), this, SIGNAL(conference()));
    connect(ui->transferToolButton, SIGNAL(clicked()), this, SIGNAL(transfer()));
    connect(ui->recordToolButton, SIGNAL(clicked()), this, SIGNAL(record()));
    connect(ui->callToolButton, SIGNAL(clicked()), this, SLOT(editContactClicked()));
}

AstCtiCallWidget::~AstCtiCallWidget()
{
    delete ui;
}

void AstCtiCallWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QString AstCtiCallWidget::callerIdNumber() const
{
    return ui->numberLabel->text();
}

void AstCtiCallWidget::setCallerIdNumber(const QString &cid)
{
    if (cid == ui->numberLabel->text())
        return;

    if (cid.isEmpty()) {
        ui->numberLabel->setText(trUtf8("???"));
    } else {
        ui->numberLabel->setText(cid);
    }
}

QString AstCtiCallWidget::callerIdName() const
{
    return ui->nameLabel->text();
}

void AstCtiCallWidget::setCallerIdName(const QString &cid)
{
    if (cid == ui->nameLabel->text())
        return;
    ui->nameLabel->setText(cid);

    if (cid.isEmpty()) {
        ui->nameLabel->hide();
    } else {
        ui->nameLabel->show();
    }
}

QString AstCtiCallWidget::remarks() const
{
    return ui->remarksLabel->text();
}

void AstCtiCallWidget::setRemarks(const QString &rms)
{
    if (rms == ui->remarksLabel->text())
        return;
    ui->remarksLabel->setText(rms);

    if (rms.isEmpty()) {
        ui->remarksLabel->hide();
    } else {
        ui->remarksLabel->show();
    }
}

AstCtiCallWidget::CallDirection AstCtiCallWidget::callDirection() const
{
    return this->m_callDirection;
}

void AstCtiCallWidget::setCallDirection(const CallDirection direction)
{
    if (direction == this->m_callDirection)
        return;
    this->m_callDirection = direction;

    if (direction == CallDirectionIncoming)
        ui->callDirectionLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/res/direction-incoming.png")));
    else
        ui->callDirectionLabel->setPixmap(QPixmap(QString::fromUtf8(":/res/res/direction-outgoing.png")));
}

AstCtiCallWidget::CallState AstCtiCallWidget::callState() const
{
    return this->m_callState;
}

void AstCtiCallWidget::setCallState(const CallState state)
{
    this->m_callState = state;

    enableAnswer();
    enableHold();
    enableConference();
    enableTransfer();
    enableRecord();
    enableEditContact();

    //Redraw control to reflect state change
    this->style()->polish(this);
}

bool AstCtiCallWidget::canAnswer() const
{
    return this->m_canAnswer;
}

void AstCtiCallWidget::setCanAnswer(const bool value)
{
    this->m_canAnswer = value;
    enableAnswer();
}

bool AstCtiCallWidget::canHold() const
{
    return this->m_canHold;
}

void AstCtiCallWidget::setCanHold(const bool value)
{
    this->m_canHold = value;
    enableHold();
}

bool AstCtiCallWidget::canConference() const
{
    return this->m_canConference;
}

void AstCtiCallWidget::setCanConference(const bool value)
{
    this->m_canConference = value;
    enableConference();
}

bool AstCtiCallWidget::canTransfer() const
{
    return this->m_canTransfer;
}

void AstCtiCallWidget::setCanTransfer(const bool value)
{
    this->m_canTransfer = value;
    enableTransfer();
}

bool AstCtiCallWidget::canRecord() const
{
    return this->m_canRecord;
}

void AstCtiCallWidget::setCanRecord(const bool value)
{
    this->m_canRecord = value;
    enableRecord();
}

bool AstCtiCallWidget::canEditContact() const
{
    return this->m_canEditContact;
}

void AstCtiCallWidget::setCanEditContact(const bool value)
{
    this->m_canEditContact = value;
    enableEditContact();
}

void AstCtiCallWidget::enableAnswer()
{
    switch (this->m_callState) {
    case CallStateRinging:
        ui->callToolButton->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/call-start.png"))));
        ui->callToolButton->setEnabled(this->m_canAnswer);
        if (this->m_canAnswer)
            ui->callToolButton->setToolTip("Answer");
        else
            ui->callToolButton->setToolTip("The phone does not support soft answering");
        break;
    case CallStateBusy:
    case CallStateInCall:
    case CallStateOnHold:
        ui->callToolButton->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/call-stop.png"))));
        ui->callToolButton->setEnabled(true);
        ui->callToolButton->setToolTip("Hang up");
        break;
    default:
        ui->callToolButton->setIcon(QIcon(QPixmap(QString::fromUtf8(":/res/res/call-start.png"))));
        ui->callToolButton->setEnabled(true);
        ui->callToolButton->setToolTip("Answer");
        break;
    }
}

void AstCtiCallWidget::enableHold()
{
    switch (this->m_callState) {
    case CallStateBusy:
        ui->holdToolButton->setEnabled(false);
        ui->holdToolButton->setToolTip("Call cannot be put on hold in busy state");
        break;
    case CallStateRinging:
        ui->holdToolButton->setEnabled(false);
        ui->holdToolButton->setToolTip("Call cannot be put on hold in ringing state");
        break;
    case CallStateInCall:
        ui->holdToolButton->setEnabled(this->m_canHold);
        if (this->m_canHold)
            ui->holdToolButton->setToolTip("Hold");
        else
            ui->holdToolButton->setToolTip("Hold is not supported or not allowed");
        ui->holdToolButton->setDown(false);
        break;
    case CallStateOnHold:
        ui->holdToolButton->setEnabled(true);
        ui->holdToolButton->setToolTip("Resume");
        ui->holdToolButton->setDown(true);
        break;
    default:
        ui->holdToolButton->setEnabled(true);
        ui->holdToolButton->setToolTip("Hold");
        break;
    }
}

void AstCtiCallWidget::enableConference()
{
    switch (this->m_callState) {
    case CallStateBusy:
        ui->conferenceToolButton->setEnabled(false);
        ui->conferenceToolButton->setToolTip("Call cannot be added to conference while busy");
        break;
    case CallStateRinging:
        ui->conferenceToolButton->setEnabled(false);
        ui->conferenceToolButton->setToolTip("Call cannot be added to conference while in ringing state");
        break;
    case CallStateInCall:
    case CallStateOnHold:
        ui->conferenceToolButton->setEnabled(this->m_canConference);
        if (this->m_canConference)
            ui->conferenceToolButton->setToolTip("Add to conference");
        else
            ui->conferenceToolButton->setToolTip("Conferencing is not supported or not allowed");
        break;
    default:
        ui->conferenceToolButton->setEnabled(true);
        ui->conferenceToolButton->setToolTip("Add to conference");
        break;
    }
}

void AstCtiCallWidget::enableTransfer()
{
    switch (this->m_callState) {
    case CallStateBusy:
        ui->transferToolButton->setEnabled(false);
        ui->transferToolButton->setToolTip("Transfer is not allowed while busy");
        break;
    case CallStateRinging:
    case CallStateInCall:
    case CallStateOnHold:
        ui->transferToolButton->setEnabled(this->m_canTransfer);
        if (this->m_canTransfer)
            ui->transferToolButton->setToolTip("Transfer");
        else
            ui->transferToolButton->setToolTip("Transfer is not supported or not allowed");
        break;
    default:
        ui->transferToolButton->setEnabled(true);
        ui->transferToolButton->setToolTip("Transfer");
        break;
    }
}

void AstCtiCallWidget::enableRecord()
{
    switch (this->m_callState) {
    case CallStateBusy:
        ui->recordToolButton->setEnabled(false);
        ui->recordToolButton->setToolTip("Recording is not allowed while busy");
        break;
    case CallStateRinging:
        ui->recordToolButton->setEnabled(this->m_canRecord);
        if (this->m_canRecord)
            ui->recordToolButton->setToolTip("Start recording immediately after answer");
        else
            ui->recordToolButton->setToolTip("Recording is not supported or not allowed");
        break;
    case CallStateInCall:
    case CallStateOnHold:
        ui->recordToolButton->setEnabled(this->m_canRecord);
        if (this->m_canRecord)
            if (ui->recordToolButton->isDown())
                ui->recordToolButton->setToolTip("Stop recording");
            else
                ui->recordToolButton->setToolTip("Record call");
        else
            ui->recordToolButton->setToolTip("Recording is not supported or not allowed");
        break;
    default:
        ui->recordToolButton->setEnabled(true);
        ui->recordToolButton->setToolTip("Record");
        break;
    }
}

void AstCtiCallWidget::enableEditContact()
{
    switch (this->m_callState) {
    case CallStateBusy:
    case CallStateRinging:
    case CallStateInCall:
    case CallStateOnHold:
        ui->contactsToolButton->setEnabled(this->m_canEditContact);
        if (this->m_canEditContact)
            ui->contactsToolButton->setToolTip("Edit contact information");
        else
            ui->contactsToolButton->setToolTip("Working with address book is not supported or not allowed");
        break;
    default:
        ui->contactsToolButton->setEnabled(true);
        ui->contactsToolButton->setToolTip("Edit contact information");
        break;
    }
}

void AstCtiCallWidget::editContactClicked()
{
    emit editContact(ui->numberLabel->text(), ui->nameLabel->text());
}
