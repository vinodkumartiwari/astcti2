/* Copyright (C) 2007-2009 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * This exception applies only to the code released under the name GNU
 * AstCTIServer.  If you copy code from other releases into a copy of GNU
 * AstCTIServer, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTIServer, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#ifndef CTISERVERAPPLICATION_H
#define CTISERVERAPPLICATION_H

#include <QCoreApplication>
#include <QFile>
#include <QtDebug>
#include <QSettings>
#include <QtCore>
#include <QtCore/QCoreApplication>
#include <QFileInfo>
#include <QtSql>

#include "coretcpserver.h"
#include "cticonfig.h"
#include "logger.h"
#include "argumentlist.h"
#include "configurationchecker.h"
#include "qastctiapplication.h"
#include "qastctioperators.h"
#include "qastctiservicesoperators.h"
#include "qastctiservice.h"
#include "qastctiservices.h"

#define DefaultServerPort           5000
#define DefaultReadTimeout          15000
#define DefaultCompressionLevel     0
#define DefaultRtConfig            "rtconfig/"

#define DefaultAmiHost              "localhost"
#define DefaultAmiPort              5038
#define DefaultAmiUser              "manager"
#define DefaultAmiSecret            "password"
#define DefaultAmiConnectTimeout    5
#define DefaultAmiConnectRetries    0

#define ExitCodeSuccess             0
#define ExitCodeNoIniConfig         1
#define ExitCodeNoSqliteConifg      2

class CtiServerApplication : public QCoreApplication
{
    Q_OBJECT

public:
    CtiServerApplication(int &argc, char **argv);
    ~CtiServerApplication();
    static CtiServerApplication *instance();
    QAstCTIOperator         *getOperatorByUsername(const QString &username);

    QAstCTIServices         *getServices();

    QAstCTIConfiguration    config; // Main configuration struct

public slots:
    CoreTcpServer           *buildNewCoreTcpServer();
    void                    reloadSqlDatabase(QFileInfo *databaseFile);

private:
    bool                    canStart;
    CoreTcpServer           *coreTcpServer;
    ConfigurationChecker    *configChecker;
    bool                    isConfigLoading;

    bool                    buildSqlDatabase(const QString &databaseFile);
    void                    destroySqlDatabase();
    QString                 readDatabaseVersion();

    bool                    readSettingsFile(const QString configFile, QAstCTIConfiguration *config);
    void                    writeSettingsFile(QSettings *settings, const QString &key, const  QVariant &defValue);

protected:
    QAstCTIServices         *services;
    QAstCTIOperators        *ctiOperators;


};

#endif // CTISERVERAPPLICATION_H
