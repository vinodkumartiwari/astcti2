CONFIG      += qt designer plugin debug_and_release
TARGET      = $$qtLibraryTarget(AstCTISpeedDialWidgetPlugin)
TEMPLATE    = lib

HEADERS     = astctispeeddialwidgetplugin.h
SOURCES     = astctispeeddialwidgetplugin.cpp
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

include(AstCTISpeedDialWidget.pri)
