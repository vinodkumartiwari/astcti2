# -------------------------------------------------
# Project created by QtCreator 2008-12-27T09:05:04
# -------------------------------------------------
include(../QtSingleApplication/qtsinglecoreapplication.pri)
include(../QsLog/QsLog.pri)

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
    amiclient.cpp \
    argumentlist.cpp \
    ctiserverapplication.cpp \
    configurationchecker.cpp \
    coretcpserver.cpp \
    db.cpp \
    amicommand.cpp \
    astcticonfiguration.cpp \
    astctiaction.cpp \
    astcticall.cpp \
    astctioperator.cpp \
    astctiseat.cpp \
    astctiservice.cpp
HEADERS += clientmanager.h \
    amiclient.h \
    coreconstants.h \
    argumentlist.h \
    ctiserverapplication.h \
    configurationchecker.h \
    coretcpserver.h \
    db.h \
    amicommand.h \
    astcticonfiguration.h \
    astctiaction.h \
    astcticall.h \
    astctioperator.h \
    astctiseat.h \
    astctiservice.h

!equals($${PWD}, $${OUT_PWD}) {
    # Shadow building is enabled
    # Specify files for copying
	LICENSE_SOURCE = $${PWD}/../LICENSE.txt
	LICENSE_QSLOG_SOURCE = $${PWD}/../QsLog/LICENSE.txt
	build_pass:CONFIG(debug, debug|release) {
		SETTINGS_SOURCE = $${PWD}/settings.ini
		SETTINGS_DEST = $${OUT_PWD}/debug
		LICENSE_DEST = $${OUT_PWD}/debug
		LICENSE_QSLOG_DEST = $${OUT_PWD}/debug/LICENSE_QSLOG.txt
	} else:build_pass {
		SETTINGS_SOURCE = $${PWD}/settings.ini.dist
		SETTINGS_DEST = $${OUT_PWD}/release/settings.ini
		LICENSE_DEST = $${OUT_PWD}/release
		LICENSE_QSLOG_DEST = $${OUT_PWD}/release/LICENSE_QSLOG.txt
	}

    # Replace '/' with '\' in Windows paths
    win32 {
		SETTINGS_SOURCE = $${replace(SETTINGS_SOURCE, /, \\)}
		SETTINGS_DEST = $${replace(SETTINGS_DEST, /, \\)}
		LICENSE_SOURCE = $${replace(LICENSE_SOURCE, /, \\)}
		LICENSE_DEST = $${replace(LICENSE_DEST, /, \\)}
		LICENSE_QSLOG_SOURCE = $${replace(LICENSE_QSLOG_SOURCE, /, \\)}
		LICENSE_QSLOG_DEST = $${replace(LICENSE_QSLOG_DEST, /, \\)}
	}

    # COPY_FILE is a variable that qmake automatically creates in Makefile
	COPY_FILES = $(COPY_FILE) $$quote($$SETTINGS_SOURCE) $$quote($$SETTINGS_DEST) && \
				 $(COPY_FILE) $$quote($$LICENSE_SOURCE) $$quote($$LICENSE_DEST) && \
				 $(COPY_FILE) $$quote($$LICENSE_QSLOG_SOURCE) $$quote($$LICENSE_QSLOG_DEST)

	QMAKE_PRE_LINK += $$COPY_FILES
}
