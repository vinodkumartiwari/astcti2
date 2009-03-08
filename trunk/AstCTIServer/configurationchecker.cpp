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

ConfigurationChecker::ConfigurationChecker(const QString& path) :
       last_configuration("")
{
    this->configurationPath = path;
    this->watcher = new QFileSystemWatcher();

    connect(this->watcher, SIGNAL(directoryChanged(QString)), this, SLOT(check_configuration_dir(QString)));
    this->watcher->addPath(path);
}

ConfigurationChecker::~ConfigurationChecker()
{
    qDebug() << "In ConfigurationChecker::~ConfigurationChecker()";
    /*if (this->last_configuration !=0 )
    {
        delete(this->last_configuration);
    }*/
}

QString ConfigurationChecker::load_first_configuration()
{
    QString last_config_filename = read_last_modified_configuration_file();
    if ( (last_config_filename != "") &
        (this->last_configuration == "") )
    {
        this->last_configuration = last_config_filename;
    }
    return last_config_filename;
}


void ConfigurationChecker::check_configuration_dir(const QString& path)
{
    Q_UNUSED(path);
    QString last_config_filename = this->read_last_modified_configuration_file();

    if (last_config_filename == "") return;
    if (this->last_configuration != 0)
    {
        if (this->last_configuration != last_config_filename)
        {            
            this->last_configuration = last_config_filename;
            QFileInfo* info = new QFileInfo(last_config_filename);
            emit this->new_configuration(info);
        }
    }
}

QString ConfigurationChecker::read_last_modified_configuration_file()
{
    QDir dir(this->configurationPath);
    if (!dir.exists()) return 0;
    QStringList fileFilter;
    fileFilter << "*.db3";
    QFileInfoList list = dir.entryInfoList(fileFilter, QDir::Files | QDir::NoDotAndDotDot, QDir::Time);
    if (list.size() > 0)
    {
        QFileInfo fi = list[0];
        return fi.absoluteFilePath();
    }
    return "";
}
