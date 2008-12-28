#ifndef MAIN_H
#define MAIN_H
#include <QtCore/QCoreApplication>
#include <QSettings>
#include "cticonfig.h"

void writeSetting( QSettings *settings, const QString & key, const  QVariant & defvalue);

#endif // MAIN_H
