# -------------------------------------------------
# Project created by QtCreator 2008-12-21T22:48:47
# -------------------------------------------------
TARGET = AstCTIClient
TEMPLATE = app
QT += webkit
SOURCES += main.cpp \
    mainwindow.cpp \
    aboutdialog.cpp \
    browserdialog.cpp
HEADERS += mainwindow.h \
    aboutdialog.h \
    coreconstants.h \
    browserdialog.h
FORMS += mainwindow.ui \
    aboutdialog.ui \
    browserdialog.ui
RESOURCES += mainresources.qrc
TRANSLATIONS = AstCTIClient_en_US.ts \
    AstCTIClient_it_IT.ts
OTHER_FILES += license.txt
