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
 * %PROJECTNAME%.  If you copy code from other releases into a copy of GNU
 * %PROJECTNAME%, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for %PROJECTNAME%, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#ifndef MANAGERWINDOW_H
#define MANAGERWINDOW_H

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QStackedLayout>
#include <QtWidgets/QTabBar>

#include "cticlientwindow.h"

const int borderWidth = 5;

namespace Ui {
    class ManagerWindow;
}

class ManagerWindow : public CtiClientWindow
{
    Q_OBJECT

public:
	explicit ManagerWindow(AstCtiConfiguration* config);
    ~ManagerWindow();

public slots:
	virtual void newConfig(AstCtiConfiguration* config);
	virtual void receiveChannelEvent(AstCtiChannel* channel);

	void showHelp();
    void showMaxRestore();

protected:
    void changeEvent(QEvent* e);
    void resizeEvent(QResizeEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* );
    bool eventFilter(QObject* object, QEvent* e);

private:
    Ui::ManagerWindow* ui;
    QVBoxLayout* mainLayout;
    QWidget* titleDummyWidget;
    QStackedLayout* titleStackedLayout;
//    QHBoxLayout* accountsLayout;
//    QTabBar* accountsTabBar;

    void connectSlots();
    void enableControls(bool enable);
    void writeSettings();
    void readSettings();

    bool resizeLeft;
    bool resizeRight;
    bool resizeUp;
    bool resizeDown;
};

#endif // MANAGERWINDOW_H
