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
    mainserver.cpp \
    clientmanager.cpp \
    logger.cpp
HEADERS += mainserver.h \
    clientmanager.h \
    main.h \
    cticonfig.h \
    logger.h
