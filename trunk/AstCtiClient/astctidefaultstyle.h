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

#ifndef ASTCTIDEFAULTSTYLE_H
#define ASTCTIDEFAULTSTYLE_H

#include <QProxyStyle>
#include <QPalette>

class AstCtiDefaultStylePrivate;
class QPainterPath;

class AstCtiDefaultStyle : public QCommonStyle
{
    Q_OBJECT

public:
	explicit AstCtiDefaultStyle();
	~AstCtiDefaultStyle();

    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0) const;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = 0) const;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget = 0) const;

//    QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize &size, const QWidget* widget) const;
//    QRect subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget) const;
//    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex* opt, SubControl sc, const QWidget* widget) const;

//    SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex* option, const QPoint &pos, const QWidget* widget = 0) const;
//    QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption* opt, const QWidget* widget = 0) const;
    int styleHint(StyleHint hint, const QStyleOption* option = 0, const QWidget* widget = 0, QStyleHintReturn* returnData = 0) const;
    QRect itemRect(QPainter* p, const QRect &r, int flags, bool enabled, const QPixmap* pixmap, const QString& text, int len = -1) const;
//    QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption* opt) const;

//    int pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const;

//    QPalette standardPalette() const;

    void polish(QWidget* widget);
    void polish(QPalette &pal);

    void unpolish(QWidget* widget);
//    void unpolish(QApplication* app);

//protected slots:
//    QIcon standardIconImplementation(StandardPixmap standardIcon, const QStyleOption* option, const QWidget* widget) const;

private:
    AstCtiDefaultStylePrivate* d;
    static void setTexture(QPalette &palette, QPalette::ColorRole role, const QPixmap &pixmap);
    static QPainterPath roundRectPath(const QRect &rect);
	Q_DISABLE_COPY(AstCtiDefaultStyle)
};

#endif // ASTCTIDEFAULTSTYLE_H
