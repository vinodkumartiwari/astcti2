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

#include <QtCore>
#include <QtCore/QCoreApplication>

#include "coretcpserver.h"
#include "astcticonfiguration.h"
#include "argumentlist.h"
#include "configurationchecker.h"
#include "astctiaction.h"
#include "astctiservice.h"
#include "qtsinglecoreapplication.h"

#define defaultSqlHost                      "localhost"
#define defaultSqlUser                      "asteriskcti"
#define defaultSqlPassWord                  "asteriskcti"
#define defaultSqlPort                      3306
#define defaultSqlDatabase                  "asteriskcti"

#define exitCodeSuccess                     0
#define exitCodeFailure                     1

class CtiServerApplication : public QtSingleCoreApplication
{
    Q_OBJECT

public:
    CtiServerApplication(const QString &appId, int &argc, char **argv);
    ~CtiServerApplication();
    static CtiServerApplication *instance();

	bool                  getCanStart();
	bool                  start();

public slots:
	void                  reloadSettings(AstCtiConfiguration *newConfig);

private:
	Q_DISABLE_COPY(CtiServerApplication)
	bool                  canStart;
	CoreTcpServer        *coreTcpServer;
	ConfigurationChecker *configChecker;
	bool                  buildCoreTcpServer(AstCtiConfiguration *config);
	QString               readDatabaseVersion();
	bool                  createDatabaseConnection(const QString &iniFilePath);
};

#endif // CTISERVERAPPLICATION_H
