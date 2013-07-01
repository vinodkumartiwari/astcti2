QT          += widgets
CONFIG      += plugin release
TARGET       = $$qtLibraryTarget($$TARGET)
TEMPLATE     = lib

greaterThan(QT_MAJOR_VERSION, 4) {
	QT      += designer
} else {
	CONFIG  += designer
}

target.path  = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

DEFINES     += ASTCTISPEEDDIALWIDGET_LIBRARY

HEADERS      = astctispeeddialwidgetplugin.h
SOURCES      = astctispeeddialwidgetplugin.cpp
RESOURCES    = icons.qrc
OTHER_FILES  = astctispeeddialwidget.json
LIBS        += -L.

include(AstCTISpeedDialWidget.pri)
