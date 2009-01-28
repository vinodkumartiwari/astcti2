# -------------------------------------------------
# Project created by QtCreator 2008-12-21T22:48:47
# -------------------------------------------------
TARGET = AstCTIClient
TEMPLATE = app
QT += webkit network
CONFIG += qt warn_on
contains(QT_BUILD_PARTS, tools): CONFIG += uitools
else: DEFINES += QT_NO_UITOOLS

SOURCES += main.cpp \
    mainwindow.cpp \
    aboutdialog.cpp \
    browserwindow.cpp \
    cticlientapplication.cpp \
    webview.cpp
HEADERS += mainwindow.h \
    aboutdialog.h \
    coreconstants.h \
    browserwindow.h \
    cticlientapplication.h \
    webview.h
FORMS += mainwindow.ui \
    aboutdialog.ui \
    browserwindow.ui
RESOURCES += mainresources.qrc
TRANSLATIONS = AstCTIClient_en_US.ts \
    AstCTIClient_it_IT.ts
OTHER_FILES += license.txt
