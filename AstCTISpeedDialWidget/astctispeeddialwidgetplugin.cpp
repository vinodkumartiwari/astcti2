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
 * AstCTISpeedDialWidget.  If you copy code from other releases into a copy of GNU
 * AstCTISpeedDialWidget, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTISpeedDialWidget, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include "astctispeeddialwidget.h"
#include "astctispeeddialwidgetplugin.h"

#include <QtCore/QtPlugin>

AstCTISpeedDialWidgetPlugin::AstCTISpeedDialWidgetPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void AstCTISpeedDialWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool AstCTISpeedDialWidgetPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *AstCTISpeedDialWidgetPlugin::createWidget(QWidget *parent)
{
    return new AstCTISpeedDialWidget(parent);
}

QString AstCTISpeedDialWidgetPlugin::name() const
{
    return QLatin1String("AstCTISpeedDialWidget");
}

QString AstCTISpeedDialWidgetPlugin::group() const
{
    return QLatin1String("AsteriskCTI");
}

QIcon AstCTISpeedDialWidgetPlugin::icon() const
{
    return QIcon();
}

QString AstCTISpeedDialWidgetPlugin::toolTip() const
{
    return QLatin1String("AsteriskCTI Speed Dial");
}

QString AstCTISpeedDialWidgetPlugin::whatsThis() const
{
    return QLatin1String("The widget displays and represents a single speed dial or busy lamp field for AsteriskCTI");
}

bool AstCTISpeedDialWidgetPlugin::isContainer() const
{
    return false;
}

QString AstCTISpeedDialWidgetPlugin::domXml() const
{
    return QLatin1String("<ui language=\"c++\" displayname=\"Speed Dial\">\n"
                         "  <widget class=\"AstCTISpeedDialWidget\" name=\"astCTISpeedDial\"/>\n"
                         "  <customwidgets>\n"
                         "    <customwidget>\n"
                         "      <class>AstCTISpeedDialWidget</class>\n"
                         "      <addpagemethod>addPage</addpagemethod>\n"
                         "      <propertyspecifications>\n"
                         "        <stringpropertyspecification name=\"text\" notr=\"true\" type=\"singleline\"/>\n"
                         "        <stringpropertyspecification name=\"dialString\" notr=\"true\" type=\"singleline\"/>\n"
                         "        <stringpropertyspecification name=\"callForwardUnconditional\" notr=\"true\" type=\"singleline\"/>\n"
                         "        <stringpropertyspecification name=\"callForwardBusy\" notr=\"true\" type=\"singleline\"/>\n"
                         "        <stringpropertyspecification name=\"callForwardNoAnswer\" notr=\"true\" type=\"singleline\"/>\n"
                         "      </propertyspecifications>\n"
                         "    </customwidget>\n"
                         "  </customwidgets>\n"
                         "</ui>\n");
}

QString AstCTISpeedDialWidgetPlugin::includeFile() const
{
    return QLatin1String("astctispeeddialwidget.h");
}

Q_EXPORT_PLUGIN2(AstCTISpeedDialWidgetPlugin, AstCTISpeedDialWidgetPlugin)
