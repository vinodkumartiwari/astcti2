# -------------------------------------------------
# Project created by QtCreator 2008-12-27T09:05:04
# -------------------------------------------------
QT += sql \
    xml \
    network
QT -= gui
TARGET = AstCTIServer
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    database.cpp \
    mainserver.cpp \
    clientmanager.cpp
HEADERS += database.h \
    mainserver.h \
    clientmanager.h \
    main.h \
    cticonfig.h
