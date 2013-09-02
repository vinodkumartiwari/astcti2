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

#include "QsLog.h"
#include "clientmanager.h"

/*!
    ClientManager Constructor
 */
ClientManager::ClientManager(QTcpSocket* const socket, QObject* parent) : QObject(parent)
{
	QLOG_TRACE() << "Creating new ClientManager";

	this->socket                = socket;
	this->activeOperator        = 0;
    this->activeSeat            = 0;
	this->localIdentifier       = QStringLiteral("");
	this->clientOperatingSystem = QStringLiteral("");
	this->ctiUsername           = QStringLiteral("");
    this->blockSize             = 0;
    this->compressionLevel      = -1;
	this->isAuthenticated       = false;
}

ClientManager::~ClientManager()
{
	QLOG_TRACE() << "Destroying ClientManager" << this->localIdentifier;

	// this->activeSeat does not need to be deleted as it is not allocated in this class
	// this->activeOperator does not need to be deleted as it is not allocated in this class
}

QTcpSocket* const ClientManager::getSocket() const
{
	return this->socket;
}

AstCtiOperator* const ClientManager::getActiveOperator() const
{
	return this->activeOperator;
}

void ClientManager::setActiveOperator(AstCtiOperator* const op)
{
	this->activeOperator = op;
}

AstCtiSeat* const ClientManager::getActiveSeat() const
{
	return this->activeSeat;
}

void ClientManager::setActiveSeat(AstCtiSeat* const seat)
{
	this->activeSeat = seat;

	// Assign queues to extensions so we can track agent status on each queue
	if (seat != 0 && this->activeOperator != 0) {
		AstCtiServiceRevHash services = this->activeOperator->getServices();

		AstCtiExtensionList extensions = seat->getExtensions();
		const int listSize = extensions.size();
		for (int i = 0; i < listSize; i++) {
			AstCtiExtension* extension = extensions.at(i);
			extension->queues.clear();

			for (AstCtiServiceRevHash::const_iterator i = services.constBegin();
				 i != services.constEnd();
				 i++) {
				AstCtiService* service = i.key();
				if (service->isQueue())
					extension->queues.insert(service->getQueueName(), AgentStatusLoggedOut);
			}
		}
	}
}

const QString& ClientManager::getClientOperatingSystem() const
{
	return this->clientOperatingSystem;
}

void ClientManager::setClientOperatingSystem(const QString& os)
{
	this->clientOperatingSystem = os;
}

const QString& ClientManager::getCtiUsername() const
{
	return this->ctiUsername;
}

void ClientManager::setCtiUsername(const QString& username)
{
	this->ctiUsername = username;
}

const bool ClientManager::getIsAuthenticated() const
{
	return this->isAuthenticated;
}

void ClientManager::setIsAuthenticated(const bool isAuthenticated)
{
	this->isAuthenticated = isAuthenticated;
}

const int ClientManager::getCompressionLevel() const
{
	return this->compressionLevel;
}

void ClientManager::setCompressionLevel(const int compressionLevel)
{
	this->compressionLevel = compressionLevel;
}

const QString& ClientManager::getLocalIdentifier() const
{
	return this->localIdentifier;
}

void ClientManager::setLocalIdentifier(const QString& localIdentifier)
{
	this->localIdentifier = localIdentifier;
}

void ClientManager::setIdleTimeout(const int miliseconds)
{
	this->idleTimer.setInterval(miliseconds);
	this->idleTimer.start();
}

void ClientManager::idleTimerElapsed()
{
	this->idleTimer.stop();

	//Disconnection will delete client and remove it from clients hash
	this->socket->disconnectFromHost();
}

void ClientManager::resetIdleTimer()
{
	this->idleTimer.start();
}
