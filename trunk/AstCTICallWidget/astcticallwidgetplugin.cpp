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

#include "astcticallwidget.h"
#include "astcticallwidgetplugin.h"

#include <QtCore/QtPlugin>

AstCTICallWidgetPlugin::AstCTICallWidgetPlugin(QObject *parent)
    : QObject(parent)
{
	initialized = false;
}

void AstCTICallWidgetPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
	if (initialized)
        return;

    // Add extension registrations, etc. here

	initialized = true;
}

bool AstCTICallWidgetPlugin::isInitialized() const
{
	return initialized;
}

QWidget *AstCTICallWidgetPlugin::createWidget(QWidget *parent)
{
    return new AstCTICallWidget(parent);
}

QString AstCTICallWidgetPlugin::name() const
{
    return QLatin1String("AstCTICallWidget");
}

QString AstCTICallWidgetPlugin::group() const
{
	return QLatin1String("AsteriskCTI");
}

QIcon AstCTICallWidgetPlugin::icon() const
{
    return QIcon();
}

QString AstCTICallWidgetPlugin::toolTip() const
{
    return QLatin1String("Visual representation of a telephone call");
}

QString AstCTICallWidgetPlugin::whatsThis() const
{
    return QLatin1String("The widget displays and represents a single active telephone call.");
}

bool AstCTICallWidgetPlugin::isContainer() const
{
    return false;
}

QString AstCTICallWidgetPlugin::domXml() const
{
	return QLatin1String(
		"<ui language=\"c++\" displayname=\"Telephone Call\">\n"
		"  <widget class=\"AstCTICallWidget\" name=\"astCTICall\"/>\n"
		"  <customwidgets>\n"
		"    <customwidget>\n"
		"      <class>AstCTICallWidget</class>\n"
		"      <addpagemethod>addPage</addpagemethod>\n"
		"      <propertyspecifications>\n"
		"        <stringpropertyspecification name=\"callerIdNumber\" notr=\"true\" type=\"singleline\"/>\n"
		"        <stringpropertyspecification name=\"callerIdName\" notr=\"true\" type=\"singleline\"/>\n"
		"        <stringpropertyspecification name=\"remarks\" notr=\"true\" type=\"singleline\"/>\n"
		"      </propertyspecifications>\n"
		"    </customwidget>\n"
		"  </customwidgets>\n"
		"</ui>\n");
}

QString AstCTICallWidgetPlugin::includeFile() const
{
    return QLatin1String("astcticallwidget.h");
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(AstCtiCallWidgetPlugin, AstCtiCallWidgetPlugin)
#endif // QT_VERSION < 0x050000
