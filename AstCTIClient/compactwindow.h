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
 * AstCTIClient.  If you copy code from other releases into a copy of GNU
 * AstCTIClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTIClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#ifndef COMPACTWINDOW_H
#define COMPACTWINDOW_H

#include <QtGui/QDialog>
#include <QSystemTrayIcon>

#include "globalconstants.h"
#include "aboutdialog.h"
#include "browserwindow.h"

const QString statusMessageOK = "Conection to server OK";
const QString statusMessageNotOK = "Conection to server has been lost. Trying to reconnect...";

namespace Ui {
    class CompactWindow;
}

class CompactWindow : public QDialog {
    Q_OBJECT
public:
    CompactWindow(const QString &userName);
    ~CompactWindow();

public slots:
    void setStatus(bool status);
    void showMessage(const QString &message, QSystemTrayIcon::MessageIcon severity);
    void placeCall();
    void about();
    void minimizeToTray();
    void pause(bool paused);
    void quit(bool skipCheck);

signals:
    void changePassword();
    void logOff();

protected:
    void keyPressEvent (QKeyEvent *event);
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *e);
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::CompactWindow *m_ui;

    bool canClose;
    QPoint offset;
    QString userName;
    QSystemTrayIcon *trayIcon;

    void createTrayIcon();
    void connectSlots();
    void enableControls(bool enable);
    void writeSettings();
    void readSettings();

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // COMPACTWINDOW_H
