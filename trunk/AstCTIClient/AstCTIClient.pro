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
    managerwindow.cpp
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
    managerwindow.h
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
    win32 {
        COPY = copy /y
    }
    unix {
        COPY = /bin/cp
    }
    mac {
        COPY = cp -f
    }

    # Specify files for copying
    LICENSE_SOURCE = $${PWD}/license.txt
    LICENSE_DEST = $${OUT_PWD}

    # Replace '/' with '\' in Windows paths
    win32 {
        LICENSE_SOURCE = $${replace(LICENSE_SOURCE, /, \)}
        LICENSE_DEST = $${replace(LICENSE_DEST, /, \)}
    }

    COPY_LICENSE = $$COPY $$LICENSE_SOURCE $$LICENSE_DEST

    QMAKE_POST_LINK += $$COPY_LICENSE
}
