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

ConfigurationChecker::ConfigurationChecker(const QString& path)
{
    this->configurationPath = path;
    this->watcher = new QFileSystemWatcher();

    connect(this->watcher, SIGNAL(directoryChanged(QString)), this, SLOT(configurationChanged(QString)));
    this->watcher->addPath(path);
}

ConfigurationChecker::~ConfigurationChecker()
{
    if (this->lastConfiguration !=0 )
    {
        delete(this->lastConfiguration);
    }
}

QFileInfo* ConfigurationChecker::loadFirstConfiguration()
{
    QFileInfo *result = readLastModifiedConfigurationFile();
    if (this->lastConfiguration == 0)
    {
        this->lastConfiguration = result;
    }
    return result;
}


void ConfigurationChecker::configurationChanged(const QString& path)
{
    Q_UNUSED(path);
    QFileInfo* info = this->readLastModifiedConfigurationFile();
    if (info == 0) return;
    if (this->lastConfiguration != 0)
    {
        if (this->lastConfiguration->absoluteFilePath() != info->absoluteFilePath())
        {            
            delete(this->lastConfiguration);
            this->lastConfiguration = info;
            emit this->newConfiguration(info);
        }
    }
}

QFileInfo* ConfigurationChecker::readLastModifiedConfigurationFile()
{
    QDir dir(this->configurationPath);
    if (!dir.exists()) return 0;
    QStringList fileFilter;
    fileFilter << "*.db3";
    QFileInfoList list = dir.entryInfoList(fileFilter, QDir::Files | QDir::NoDotAndDotDot, QDir::Time);
    if (list.size() > 0)
    {
        return new QFileInfo(list[0]);
    }
    return 0;
}
