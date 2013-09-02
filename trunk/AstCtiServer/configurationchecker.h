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
 * AstCtiServer.  If you copy code from other releases into a copy of GNU
 * AstCtiServer, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCtiServer, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */
#ifndef CONFIGURATIONCHECKER_H
#define CONFIGURATIONCHECKER_H

#include <QObject>

#include "astcticonfiguration.h"

#define defaultCtiServerAddress             "0.0.0.0"
#define defaultCtiServerPort                5000
#define defaultCtiConnectTimeout            1500
#define defaultCtiReadTimeout               15000
#define defaultCtiSocketCompressionLevel    7

#define defaultAmiHost                      "localhost"
#define defaultAmiPort                      5038
#define defaultAmiUser                      "manager"
#define defaultAmiSecret                    "password"
#define defaultAmiConnectTimeout            1500
#define defaultAmiConnectRetryAfter         2

#define defaultAsteriskVersion              "1.8"
#define defaultAutoAnswerContext            "cti-auto-answer"

class ConfigurationChecker : public QObject
{
    Q_OBJECT

public:
	explicit ConfigurationChecker(QObject* parent = 0);
    ~ConfigurationChecker();

	void                stop();

public slots:
	void                run();

signals:
	void				newConfiguration(AstCtiConfiguration* newConfig);

private:
	Q_DISABLE_COPY(ConfigurationChecker)

	const long          readLastModified();
	const QVariant      readSetting(const QString& name, const QVariant& defaultValue);
	const bool          readConfiguration();
	const QString       timestampToString(const long timestamp);
	void                delay(const int secs);

	int                 lastTimeStamp;
	bool                isRunning;
};

#endif // CONFIGURATIONCHECKER_H
