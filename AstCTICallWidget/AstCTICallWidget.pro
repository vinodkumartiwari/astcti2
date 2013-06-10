CONFIG      += qt designer plugin debug_and_release
TEMPLATE    = lib
TARGET  = $$qtLibraryTarget(AstCTICallWidgetPlugin)

HEADERS     = astcticallwidgetplugin.h
SOURCES     = astcticallwidgetplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L.

win32 {
    PLUGINDIR   = $$(APPDATA)/Qt/plugins/designer
    DLLDESTDIR = $$PLUGINDIR
} else {
    PLUGINDIR   = $$(HOME)/.qt/plugins/designer
    DESTDIR = $$PLUGINDIR
}
target.path += $$PLUGINDIR

INSTALLS    += target

include(AstCTICallWidget.pri)
