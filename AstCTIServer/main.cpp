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

#include "coreconstants.h"
#include "ctiserverapplication.h"

// TODO:
// replace, where possible, preprocessor macros with consts
// DOCS STYLE COMMENTS:
// http://www.ph.unimelb.edu.au/~ssk/kde/kdoc/

QAsteriskCTILogger      logger; // Our logger object

void log_handler(QtMsgType type, const char *msg)
{
    logger.write_to_log(type, msg);
}

int main(int argc, char *argv[])
{
    
    QString logo = "";
    logo += "            '.~.-'                                              \n";
    logo += "           -.~~~~~.-'                                           \n";
    logo += "         '-~~~~~~~~~~.-'                                        \n";
    logo += "        '.~~~~~~~~~~~~~~.''                         ''''        \n";
    logo += "      '-~~.-.~~~~~.---.~~~.-'                '''--..~~~.-       \n";
    logo += "     '.~.-''-.~~~.-''-.~~~~~~.-'       ''--..~~~~~~~~~~~.'      \n";
    logo += "   '-~~~~.-'''-..-''-.~~~~~~~~~~.---..~~~~~~~~~.--.~~~~~~-      \n";
    logo += "  '~oooo~~~.--'--'--~~~~~~~~~~~~~~~~~~~~~~~~~~.'   -~~~~~.'     \n";
    logo += "'-~ooooooo~..-'''''-------.~~~~~~~~~~~~~~~~~~~.-''-.~~~~~~-'    \n";
    logo += ".ooooo~.--------'----''''-.~~~~~~~~~~.-''-~~~~~~..---~~~~~.-    \n";
    logo += "ooooooo.---.~o~.--.~~~....~~~~~~~~~~~-'  '----~~-   '.~~ooo.'   \n";
    logo += "'-~ooooo~~oooo~.--.oooooooo~~~~~~~~~.''      '.~.'   -oooooo-   \n";
    logo += "   '-~oooooooo~.---~oooooooo~.-'' -o.'     -~oooo.'  '.ooooo~'  \n";
    logo += "      '.~oooooo~~~~ooooooo.'   ''-.ooo~-   -~oooo~-   -~ooooo.' \n";
    logo += "        ''.oooooooooooooo.'  '.oooooooo~'  '.ooooo.'  '.ooooo~- \n";
    logo += "           '-.oooooooooo~'  '.oooooooooo.   '.o~ooo-   '~ooooo. \n";
    logo += "              '-~ooooooo~'   -oooooooooo~-      -~o~---.~ooooooo\n";
    logo += "                 -~oooooo.'   -.~o~..~oooo.'  ''.~oooooooooooo~~\n";
    logo += "                 '.ooooooo.'         '~ooooooooooooooooo~.--''  \n";
    logo += "                  '~oooooooo.--'''-.~oooooooooooo~..-''         \n";
    logo += "                   -oooooooooooooooooooooo~..-''                \n";
    logo += "                   '.oooooooooooooo~..-''                       \n";
    logo += "                    -~oooooo~..-''                              \n";
    logo += "                     ~ooo~.                                     \n";
    qDebug("%s", qPrintable(logo));

    qDebug() << "AsteriskCTI Server version" << APP_VERSION_LONG;
    qDebug() << "Based on Qt" << QT_VERSION_STR;
    qDebug() << "Built on " __DATE__ " at " __TIME__;
    qDebug() << "Copyright" <<  APP_YEAR << ". All rights reserved.";
    qDebug() << "The program is provided AS IS with NO WARRANTY OF ANY ";
    qDebug() << "KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY";
    qDebug() << "AND FITNESS FOR A PARTICULAR PURPOSE.";

    qInstallMsgHandler(log_handler);

    // QCoreApplication needs to be initialized here
    CtiServerApplication app(argc, argv);
    if (app.build_new_core_tcpserver() == 0)
        return 1;

    return app.exec();
}



