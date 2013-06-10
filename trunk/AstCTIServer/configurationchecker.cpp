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

#include "configurationchecker.h"
#include "db.h"

ConfigurationChecker::ConfigurationChecker(const bool &debug) :
		QObject()
{
	this->debug = debug;
	this->isRunning = true;
}

ConfigurationChecker::~ConfigurationChecker()
{
	if (this->debug)
		qDebug() << "In ConfigurationChecker::~ConfigurationChecker()";
	this->isRunning = false;
}

void ConfigurationChecker::run()
{
	// Initialize lastTimeStamp before first use
	this->lastTimeStamp = readLastModified();

	long modified;
	while (this->isRunning) {
		modified = readLastModified();
        if (modified > this->lastTimeStamp) {
			if (this->debug)
				qDebug() << "Configuration has changed on" << modified;
			this->lastTimeStamp = modified;
            emit this->newConfiguration();
        }
		this->delay(30);
    }
}

void ConfigurationChecker::stop()
{
	// Even if this function is called from another thread,
	// mutex should not be necessary as it is bool variable, and its value
	// should be changed atomically, and even if the write itself is not atomic
	// and the variable can have an inconsistent value, it can be either
	// false or true, and it will eventually be read as false
	// (since the only value that is written there is false)
	this->isRunning = false;
}

int ConfigurationChecker::readLastModified()
{
	bool ok;
	QVariant result = DB::readScalar("SELECT LAST_UPDATE FROM dbversion LIMIT 1", &ok);
	return ok ? result.toInt() : 0;
}

void ConfigurationChecker::delay(const int secs)
{
	QTime endTime = QTime::currentTime().addSecs(secs);
	while (QTime::currentTime() < endTime && this->isRunning)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 1000);
}
