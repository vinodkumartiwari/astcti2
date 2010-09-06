# -------------------------------------------------
# Project created by QtCreator 2008-12-27T09:05:04
# -------------------------------------------------
include(../../qtsingleapplication-2.6-opensource/src/qtsinglecoreapplication.pri)

QT += sql \
    xml \
    network
QT -= gui
TARGET = AstCTIServer
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    clientmanager.cpp \
    logger.cpp \
    amiclient.cpp \
    argumentlist.cpp \
    ctiserverapplication.cpp \
    qastctioperator.cpp \
    qastctiseat.cpp \
    qastctiservice.cpp \
    qastctivariable.cpp \
    qastctiaction.cpp \
    qastctiservices.cpp \
    qastctiservicesoperators.cpp \
    qastctiservicesvariables.cpp \
    configurationchecker.cpp \
    coretcpserver.cpp \
    qastctioperators.cpp \
    qastcticall.cpp \
    qastctioperatorservices.cpp \
    qastctiactions.cpp \
    db.cpp
HEADERS += clientmanager.h \
    cticonfig.h \
    logger.h \
    amiclient.h \
    coreconstants.h \
    argumentlist.h \
    ctiserverapplication.h \
    qastctioperator.h \
    qastctiseat.h \
    qastctiservice.h \
    qastctivariable.h \
    qastctiaction.h \
    qastctiservices.h \
    qastctiservicesoperators.h \
    qastctiservicesvariables.h \
    configurationchecker.h \
    coretcpserver.h \
    qastctioperators.h \
    qastcticall.h \
    qastctioperatorservices.h \
    qastctiactions.h \
    db.h

!equals($${PWD}, $${OUT_PWD}) {
    # Shadow building is enabled
    # Specify files for copying
    SETTINGS_SOURCE = $${PWD}/settings.ini
    SETTINGS_DEST = $${OUT_PWD}

    # Replace '/' with '\' in Windows paths
    win32 {
        SETTINGS_SOURCE = $${replace(SETTINGS_SOURCE, /, \)}
        SETTINGS_DEST = $${replace(SETTINGS_DEST, /, \)}
    }

    # COPY_FILE is a variable that qmake automatically creates in Makefile
    COPY_SETTINGS = $(COPY_FILE) $$quote($$SETTINGS_SOURCE) $$quote($$SETTINGS_DEST)

    QMAKE_PRE_LINK += $$COPY_SETTINGS
}
