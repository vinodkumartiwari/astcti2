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

DEFINES     += ASTCTICALLWIDGET_LIBRARY

HEADERS      = astcticallwidgetplugin.h
SOURCES      = astcticallwidgetplugin.cpp
RESOURCES    = icons.qrc
OTHER_FILES  = astcticallwidget.json
LIBS        += -L.

include(AstCTICallWidget.pri)
