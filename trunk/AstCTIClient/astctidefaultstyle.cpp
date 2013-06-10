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

#include <QtGui/QStyleFactory>
#include <QPushButton>
#include <QToolButton>
#include <QPainter>
#include <QComboBox>
#include <QDebug>

#include "astctidefaultstyle.h"

class AstCTIDefaultStylePrivate
{
public:
    explicit AstCTIDefaultStylePrivate();
    void init();

public:
    const QImage lineeditImage;
    const QImage lineeditImage_disabled;
    const QPixmap extButtonPixmap;
    const QPixmap closeButtonPixmap;
//    StyleAnimator animator;
};

AstCTIDefaultStylePrivate::AstCTIDefaultStylePrivate() :
    lineeditImage(QLatin1String(":/core/images/inputfield.png")),
    lineeditImage_disabled(QLatin1String(":/core/images/inputfield_disabled.png")),
    extButtonPixmap(QLatin1String(":/core/images/extension.png")),
    closeButtonPixmap(QLatin1String(":/core/images/closebutton.png"))
{
}

AstCTIDefaultStyle::AstCTIDefaultStyle()
    : QCommonStyle(),
    d(new AstCTIDefaultStylePrivate())
{
}

AstCTIDefaultStyle::~AstCTIDefaultStyle()
{
    delete d;
    d = 0;
}

void AstCTIDefaultStyle::polish(QPalette &palette)
{
//    QLinearGradient windowGradient;
//    windowGradient.setStart(0, 0);
//    windowGradient.setFinalStop(0, 1);
//    windowGradient.setColorAt(0, QColor(83, 83, 163, 255));
//    windowGradient.setColorAt(0.399, QColor(77, 77, 163, 255));
//    windowGradient.setColorAt(0.4, QColor(56, 56, 163, 255));
//    windowGradient.setColorAt(1, QColor(39, 39, 163, 255));
    QColor windowColor(189,221,236);
    QColor baseColor(212,212,212);
    QColor highlightColor(37,187,219);
    QColor textColor(64,70,104);

    palette = QPalette(windowColor);

    palette.setBrush(QPalette::Window, windowColor);
    palette.setBrush(QPalette::WindowText, textColor);
    palette.setBrush(QPalette::BrightText, textColor);
    palette.setBrush(QPalette::Base, baseColor);
    palette.setBrush(QPalette::AlternateBase, highlightColor);
    palette.setBrush(QPalette::Highlight, highlightColor);

    palette.setBrush(QPalette::Disabled, QPalette::WindowText, palette.windowText().color().dark());
    palette.setBrush(QPalette::Disabled, QPalette::Base, palette.base().color().dark());
}

void AstCTIDefaultStyle::polish(QWidget *widget)
{
    //Generate paint events when the mouse pointer enters or leaves the widget
    if (qobject_cast<QAbstractButton *>(widget)
     || qobject_cast<QComboBox *>(widget)
     || qobject_cast<QTabBar *>(widget))
        widget->setAttribute(Qt::WA_Hover, true);
}

void AstCTIDefaultStyle::unpolish(QWidget *widget)
{
    //Undo any modification done to the widget in polish()
    if (qobject_cast<QAbstractButton *>(widget)
     || qobject_cast<QComboBox *>(widget)
     || qobject_cast<QTabBar *>(widget))
        widget->setAttribute(Qt::WA_Hover, false);
}

// int AstCTIDefaultStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
// {
//     switch (metric) {
//     case PM_ComboBoxFrameWidth:
//         return 8;
//     case PM_ScrollBarExtent:
//         return QCommonStyle::pixelMetric(metric, option, widget) + 4;
//     default:
//         return QCommonStyle::pixelMetric(metric, option, widget);
//     }
// }

int AstCTIDefaultStyle::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    //Disable dithering of disabled text nad enable etching
    switch (hint) {
    case SH_DitherDisabledText:
        return int(false);
    case SH_EtchDisabledText:
        return int(true);
    default:
        return QCommonStyle::styleHint(hint, option, widget, returnData);
    }
}

//QIcon AstCTIDefaultStyle::standardIconImplementation(StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const
//{
//    QIcon icon;
//    switch (standardIcon) {
//    case QStyle::SP_TitleBarCloseButton:
//    case QStyle::SP_ToolBarHorizontalExtensionButton:
//        return QIcon(standardPixmap(standardIcon, option, widget));
//    default:
//        icon = baseStyle()->standardIcon(standardIcon, option, widget);
//    }
//    return icon;
//}

void AstCTIDefaultStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
    case PE_FrameFocusRect:
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(painter->pen().brush(), 1, Qt::DotLine));
        painter->drawRect(option->rect);
        painter->restore();
        break;
    case PE_FrameTabWidget:
        {
            painter->save();
            QColor highlightColor(37,187,219);
            painter->fillRect(option->rect, QBrush(highlightColor));
            painter->restore();
        }
        break;
    case PE_PanelButtonTool:
        {
            if (option->state & (State_HasFocus | State_Sunken | State_MouseOver)) {
                bool inTitleBar(false);
                if (widget != 0) {
                    QVariant prop = widget->property("styleData");
                    if (prop.isValid())
                        inTitleBar = prop.toString().contains("InTitleBar");
                }
                if (inTitleBar) {
                    QPainterPath path;
                    path.addRoundedRect(QRectF(option->rect), 4, 4);
                    QPen pen;
                    if (option->state == State_Sunken)
                        pen = QPen(Qt::lightGray);
                    else
                        pen = QPen(Qt::white);

                    painter->save();
                    painter->setRenderHint(QPainter::Antialiasing, true);
                    painter->setPen(pen);
                    painter->drawPath(path);
                    painter->restore();
                } else {
                    QCommonStyle::drawPrimitive(element, option, painter, widget);
                }
            }
        }
        break;
//    case PE_PanelButtonCommand:
//        {
//            int delta = (option->state & State_MouseOver) ? 64 : 0;
//            QColor slightlyOpaqueBlack(0, 0, 0, 63);
//            QColor semiTransparentWhite(255, 255, 255, 127 + delta);
//            QColor semiTransparentBlack(0, 0, 0, 127 - delta);
//            int x, y, width, height;
//            option->rect.getRect(&x, &y, &width, &height);
//            QPainterPath roundRect = roundRectPath(option->rect);
//            int radius = qMin(width, height) / 2;

//            QBrush brush;
//            bool darker;

//            const QStyleOptionButton *buttonOption =  qstyleoption_cast<const QStyleOptionButton *>(option);
//            if (buttonOption && (buttonOption->features & QStyleOptionButton::Flat)) {
//                brush = option->palette.background();
//                darker = (option->state & (State_Sunken | State_On));
//            } else {
//                if (option->state & (State_Sunken | State_On)) {
//                    brush = option->palette.mid();
//                    darker = !(option->state & State_Sunken);
//                } else {
//                    brush = option->palette.button();
//                    darker = false;
//                }
//            }

//            painter->save();
//            painter->setRenderHint(QPainter::Antialiasing, true);
//            painter->fillPath(roundRect, brush);
//            if (darker)
//                painter->fillPath(roundRect, slightlyOpaqueBlack);

//            int penWidth;
//            if (radius < 10)
//                penWidth = 3;
//            else if (radius < 20)
//                penWidth = 5;
//            else
//                penWidth = 7;

//            QPen topPen(semiTransparentWhite, penWidth);
//            QPen bottomPen(semiTransparentBlack, penWidth);

//            if (option->state & (State_Sunken | State_On))
//                qSwap(topPen, bottomPen);

//            int x1 = x;
//            int x2 = x + radius;
//            int x3 = x + width - radius;
//            int x4 = x + width;

//            if (option->direction == Qt::RightToLeft) {
//                qSwap(x1, x4);
//                qSwap(x2, x3);
//            }

//            QPolygon topHalf;
//            topHalf << QPoint(x1, y)
//                    << QPoint(x4, y)
//                    << QPoint(x3, y + radius)
//                    << QPoint(x2, y + height - radius)
//                    << QPoint(x1, y + height);

//            painter->setClipPath(roundRect);
//            painter->setClipRegion(topHalf, Qt::IntersectClip);
//            painter->setPen(topPen);
//            painter->drawPath(roundRect);

//            QPolygon bottomHalf = topHalf;
//            bottomHalf[0] = QPoint(x4, y + height);

//            painter->setClipPath(roundRect);
//            painter->setClipRegion(bottomHalf, Qt::IntersectClip);
//            painter->setPen(bottomPen);
//            painter->drawPath(roundRect);

//            painter->setPen(option->palette.foreground().color());
//            painter->setClipping(false);
//            painter->drawPath(roundRect);

//            painter->restore();

//        }
//        break;
    default:
        QCommonStyle::drawPrimitive(element, option, painter, widget);
    }
}

void AstCTIDefaultStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (widget != 0 && widget->isWindow()) {
        int x, y, width, height;
        option->rect.getRect(&x, &y, &width, &height);
        QPainterPath roundRect = roundRectPath(option->rect);

        int penWidth = 2;

        QColor borderColor(64,70,104);
        QPen pen(borderColor, penWidth);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setClipPath(roundRect);
        painter->setPen(pen);
        painter->drawPath(roundRect);
        painter->restore();
        return;
    }

    switch (element) {
    case CE_TabBarTabShape:
        if (const QStyleOptionTabV3 *tabOption = qstyleoption_cast<const QStyleOptionTabV3 *>(option)) {
            if (tabOption->state & (State_Selected | State_MouseOver) && tabOption->state & State_Enabled) {
                painter->save();
                QColor highlightColor(37,187,219);
                painter->fillRect(option->rect, QBrush(highlightColor));
                painter->restore();
            }
        }
        break;
    case CE_PushButtonLabel:
        {
            QStyleOptionButton myButtonOption;
            const QStyleOptionButton *buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option);
            if (buttonOption) {
                myButtonOption = *buttonOption;
                if (myButtonOption.palette.currentColorGroup() != QPalette::Disabled) {
                    if (myButtonOption.state & (State_Sunken | State_On)) {
                        myButtonOption.palette.setBrush(QPalette::ButtonText, myButtonOption.palette.brightText());
                    }
                }
            }
            QCommonStyle::drawControl(element, &myButtonOption, painter, widget);
        }
        break;
    default:
        QCommonStyle::drawControl(element, option, painter, widget);
    }
}

void AstCTIDefaultStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    QCommonStyle::drawComplexControl(control, option, painter, widget);
}

void AstCTIDefaultStyle::setTexture(QPalette &palette, QPalette::ColorRole role, const QPixmap &pixmap)
{
    for (int i = 0; i < QPalette::NColorGroups; ++i) {
        QColor color = palette.brush(QPalette::ColorGroup(i), role).color();
        palette.setBrush(QPalette::ColorGroup(i), role, QBrush(color, pixmap));
    }
}

QPainterPath AstCTIDefaultStyle::roundRectPath(const QRect &rect)
{
    //int radius = qMin(rect.width(), rect.height()) / 2;
    int radius = 10;
    int diam = 2 * radius;

    int x1, y1, x2, y2;
    rect.getCoords(&x1, &y1, &x2, &y2);

    QPainterPath path;
    path.moveTo(x2, y1 + radius);
    path.arcTo(QRect(x2 - diam, y1, diam, diam), 0.0, +90.0);
    path.lineTo(x1 + radius, y1);
    path.arcTo(QRect(x1, y1, diam, diam), 90.0, +90.0);
    path.lineTo(x1, y2 - radius);
    path.arcTo(QRect(x1, y2 - diam, diam, diam), 180.0, +90.0);
    path.lineTo(x1 + radius, y2);
    path.arcTo(QRect(x2 - diam, y2 - diam, diam, diam), 270.0, +90.0);
    path.closeSubpath();
    return path;
}
