include(../../qtsingleapplication-2.6-opensource/src/qtsingleapplication.pri)
include(../AstCTICallWidget/AstCTICallWidget.pri)
include(../AstCTISpeedDialWidget/AstCTISpeedDialWidget.pri)

TARGET = AstCTIClient
TEMPLATE = app
QT += webkit \
    xml \
    network
CONFIG += qt \
    warn_on

contains(QT_BUILD_PARTS, tools):CONFIG += uitools
else:DEFINES += QT_NO_UITOOLS

SOURCES += main.cpp \
    aboutdialog.cpp \
    browserwindow.cpp \
    cticlientapplication.cpp \
    webview.cpp \
    argumentlist.cpp \
    loginwindow.cpp \
    astcticallxmlparser.cpp \
    compactwindow.cpp \
    passwordwindow.cpp \
    managerwindow.cpp \
    cticlientwindow.cpp \
    astctidefaultstyle.cpp
HEADERS += \
    aboutdialog.h \
    coreconstants.h \
    browserwindow.h \
    cticlientapplication.h \
    webview.h \
    argumentlist.h \
    cticonfig.h \
    loginwindow.h \
    astcticommand.h \
    astcticallxmlparser.h \
    astcticall.h \
    compactwindow.h \
    passwordwindow.h \
    managerwindow.h \
    cticlientwindow.h \
    astctidefaultstyle.h
FORMS += \
    aboutdialog.ui \
    browserwindow.ui \
    loginwindow.ui \
    compactwindow.ui \
    passwordwindow.ui \
    managerwindow.ui
RESOURCES += mainresources.qrc
TRANSLATIONS = AstCTIClient_en_US.ts \
    AstCTIClient_it_IT.ts
OTHER_FILES += license.txt

!equals($${PWD}, $${OUT_PWD}) {
    # Shadow building is enabled
    # Specify files for copying
    LICENSE_SOURCE = $${PWD}/license.txt
	LICENSE_DEST_DEBUG = $${OUT_PWD}/debug
	LICENSE_DEST_RELEASE = $${OUT_PWD}/release

    # Replace '/' with '\' in Windows paths
    win32 {
        LICENSE_SOURCE = $${replace(LICENSE_SOURCE, /, \\)}
		LICENSE_DEST_DEBUG = $${replace(LICENSE_DEST_DEBUG, /, \\)}
		LICENSE_DEST_RELEASE = $${replace(LICENSE_DEST_RELEASE, /, \\)}
	}

    # COPY_FILE is a variable that qmake automatically creates in Makefile
	COPY_LICENSE = $(COPY_FILE) $$quote($$LICENSE_SOURCE) $$quote($$LICENSE_DEST_DEBUG) && \
				   $(COPY_FILE) $$quote($$LICENSE_SOURCE) $$quote($$LICENSE_DEST_RELEASE)

    QMAKE_PRE_LINK += $$COPY_LICENSE
}
