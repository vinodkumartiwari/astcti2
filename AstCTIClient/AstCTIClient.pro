# -------------------------------------------------
# Project created by QtCreator 2008-12-21T22:48:47
# -------------------------------------------------
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
    mainwindow.cpp \
    aboutdialog.cpp \
    browserwindow.cpp \
    cticlientapplication.cpp \
    webview.cpp \
    argumentlist.cpp \
    loginwindow.cpp \
    astcticallxmlparser.cpp \
    compactwindow.cpp \
    passwordwindow.cpp
HEADERS += mainwindow.h \
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
    globalconstants.h
FORMS += mainwindow.ui \
    aboutdialog.ui \
    browserwindow.ui \
    loginwindow.ui \
    compactwindow.ui \
    passwordwindow.ui
RESOURCES += mainresources.qrc
TRANSLATIONS = AstCTIClient_en_US.ts \
    AstCTIClient_it_IT.ts
OTHER_FILES += license.txt
