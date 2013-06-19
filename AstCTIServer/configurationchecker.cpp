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

#include "QsLog.h"
#include "configurationchecker.h"
#include "db.h"

ConfigurationChecker::ConfigurationChecker(QObject *parent) : QObject(parent)
{
	QLOG_INFO() << "Creating ConfigurationChecker";
	this->isRunning = true;
}

ConfigurationChecker::~ConfigurationChecker()
{
	QLOG_INFO() << "Closing ConfigurationChecker";
	this->isRunning = false;
}

void ConfigurationChecker::run()
{
	// Initialize lastTimeStamp before first use
	this->lastTimeStamp = 0;

	while (this->isRunning) {
		long modified = this->readLastModified();
        if (modified > this->lastTimeStamp) {
			QLOG_INFO() << "Configuration has changed on" << this->timestampToString(modified);

			if (this->readConfiguration())
				this->lastTimeStamp = modified;
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

long ConfigurationChecker::readLastModified()
{
	//If the conversion fails, toLongLong() returns 0, which is what we want
	return this->readSetting("last_update", 0).toLongLong();
}

bool ConfigurationChecker::readConfiguration()
{
	QLOG_INFO() << "Reading configuration settings from database";

	AstCtiConfiguration *config = new AstCtiConfiguration();

	config->amiHost =
			readSetting("ami_host",                     defaultAmiHost).toString();
	config->amiPort =
			readSetting("ami_port",                     defaultAmiPort).toInt();
	config->amiUser =
			readSetting("ami_user",                     defaultAmiUser).toString();
	config->amiSecret =
			readSetting("ami_secret",                   defaultAmiSecret).toString();
	config->amiConnectTimeout =
			readSetting("ami_connect_timeout",          defaultAmiConnectTimeout).toInt();
	config->amiReadTimeout =
			readSetting("ami_read_timeout",             defaultAmiReadTimeout).toInt();
	config->amiConnectRetryAfter =
			readSetting("ami_connect_retry_after",      defaultAmiConnectRetryAfter).toInt();
	config->ctiServerAddress =
			readSetting("cti_server_address",           defaultCtiServerAddress).toString();
	config->ctiServerPort =
			readSetting("cti_server_port",              defaultCtiServerPort).toInt();
	config->ctiConnectTimeout =
			readSetting("cti_connect_timeout",          defaultCtiConnectTimeout).toInt();
	config->ctiReadTimeout =
			readSetting("cti_read_timeout",             defaultCtiReadTimeout).toInt();
	config->ctiCompressionLevel =
			readSetting("cti_socket_compression_level", defaultCtiSocketCompressionLevel).toInt();

	bool ok;

	const QList<QVariantList> seatData = DB::readTable("SELECT ID_SEAT,SEAT_MAC,DESCRIPTION "
													   "FROM seats WHERE ENABLED=1", &ok);
	if (ok) {
		const int listSize = seatData.size();
		for (int i = 0; i < listSize; i++) {
			const QVariantList seatRow = seatData.at(i);
			const int id = seatRow.at(0).toInt();
			AstCtiSeat *seat = new AstCtiSeat(id, seatRow.at(1).toString(),
											  seatRow.at(2).toString());
			ok = seat->loadExtensions();
			if (!ok) {
				delete seat;
				break;
			}

			if (seat->getExtensions().size() > 0) {
				config->seats.insert(id, seat);
			} else {
				QLOG_WARN() << "Seat ID" << id << "does not have any extensions.";
				delete seat;
			}
		}
	}

	if (ok) {
		const QList<QVariantList> actionData =
				DB::readTable("SELECT ID_ACTION,ACTION_OS_TYPE,ACTION_TYPE,ACTION_DESTINATION,"
							  "PARAMETERS,ENCODING FROM actions WHERE ENABLED=1", &ok);
		if (ok) {
			const int listSize = actionData.size();
			for (int i = 0; i < listSize; i++) {
				const QVariantList actionRow = actionData.at(i);
				const int id = actionRow.at(0).toInt();
				AstCtiAction *action =
						new AstCtiAction(id, actionRow.at(1).toString(),
										 actionRow.at(2).toString(), actionRow.at(3).toString(),
										 actionRow.at(4).toString(), actionRow.at(5).toString());
				config->actions.insert(id, action);
			}
		}
	}

	if (ok) {
		const QList<QVariantList> serviceData =
				DB::readTable("SELECT ID_SERVICE,SERVICE_NAME,SERVICE_CONTEXT_TYPE,"
							  "SERVICE_QUEUE_NAME FROM services WHERE ENABLED=1", &ok);
		if (ok) {
			const int listSize = serviceData.size();
			for (int i = 0; i < listSize; i++) {
				const QVariantList serviceRow = serviceData.at(i);
				const int id = serviceRow.at(0).toInt();
				AstCtiService *service =
						new AstCtiService(id, serviceRow.at(1).toString(),
										  serviceRow.at(2).toString(), serviceRow.at(3).toString());
				ok = service->loadVariables();
				if (ok)
					ok = service->loadActions(&(config->actions));
				if (!ok) {
					delete service;
					break;
				}

				config->services.insert(id, service);
			}
		}
	}

	if (ok) {
		const QList<QVariantList> operatorData =
				DB::readTable("SELECT ID_OPERATOR,FULL_NAME,USERNAME,PASS_WORD,"
							  "BEGIN_IN_PAUSE,ID_SEAT "
							  "FROM operators WHERE ENABLED=1", &ok);
		if (ok) {
			const int listSize = operatorData.size();
			for (int i = 0; i < listSize; i++) {
				const QVariantList operatorRow = operatorData.at(i);
				const QString username = operatorRow.at(2).toString();
				AstCtiOperator *op =
						new AstCtiOperator(operatorRow.at(0).toInt(), operatorRow.at(1).toString(),
										   username, operatorRow.at(3).toString(),
										   operatorRow.at(4).toBool(), operatorRow.at(5).toInt());
				ok = op->loadServices(&(config->services));
				if (!ok) {
					delete op;
					break;
				}

				config->operators.insert(username, op);
			}
		}
	}

	if (ok) {
		//Move config object to main thread so it can be deleted there
		config->moveToThread(QCoreApplication::instance()->thread());
		emit this->newConfiguration(config);
	} else {
		delete config;
		QLOG_ERROR() << "Reading configuration failed.";
		//Let the main thread know that reading configuration has failed
		emit this->newConfiguration(0);
	}

	return ok;
}

QVariant ConfigurationChecker::readSetting(const QString &name, const QVariant &defaultValue)
{
	QVariantList params;
	params.append(name);
	bool ok;
	QVariant value = DB::readScalar("SELECT val FROM server_settings WHERE name=?", params, &ok);
	if (ok)
		return value;
	else
		//TODO: We return the default value in case of an error.
		//TODO: Perhaps execution should be stopped in that case
		return defaultValue;
}

QString ConfigurationChecker::timestampToString(const long timestamp) {
	QDateTime datetime;
	datetime.setTime_t(timestamp);
	return datetime.toString(Qt::SystemLocaleShortDate);
}

void ConfigurationChecker::delay(const int secs)
{
	QTime endTime = QTime::currentTime().addSecs(secs);
	while (QTime::currentTime() < endTime && this->isRunning)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}