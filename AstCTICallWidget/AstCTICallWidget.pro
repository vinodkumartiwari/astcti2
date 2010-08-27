CONFIG      += qt designer plugin debug_and_release
TARGET      = $$qtLibraryTarget(AstCTICallWidgetPlugin)
TEMPLATE    = lib

HEADERS     = astcticallwidgetplugin.h
SOURCES     = astcticallwidgetplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L.

# Slash before path is necessary, otherwise qmake treats it as relative path
PLUGINDIR   = /$(HOME)/.qt/plugins/designer
win32:      DLLDESTDIR = $$PLUGINDIR
else:       DESTDIR = $$PLUGINDIR
target.path += $$PLUGINDIR

INSTALLS    += target

include(AstCTICallWidget.pri)
