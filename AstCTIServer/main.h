#ifndef MAIN_H
#define MAIN_H
#include <QtCore/QCoreApplication>
#include <QSettings>
#include "cticonfig.h"
#include "logger.h"

#define ASTCTISRV_RELEASE "0.1a"
#define DEFAULT_SERVER_PORT   5000
#define DEFAULT_READ_TIMEOUT    15000
void writeSetting( QSettings *settings, const QString & key, const  QVariant & defvalue);

#endif // MAIN_H
