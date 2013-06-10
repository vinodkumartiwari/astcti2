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

AsteriskCTILogger logger; // Our logger object

void logHandler(QtMsgType type, const char *msg)
{
    logger.writeToLog(type, msg);
}

int main(int argc, char *argv[])
{
    // QCoreApplication needs to be initialized here
    CtiServerApplication application("AsteriskCTIServer", argc, argv);

    if (application.isRunning()) {
         //Wake up running instance and exit
        application.sendMessage(QString(), 4000);
        return 0;
     } else {
		QString logo =
		"            '.~.-'                                              \n"
		"           -.~~~~~.-'                                           \n"
		"         '-~~~~~~~~~~.-'                                        \n"
		"        '.~~~~~~~~~~~~~~.''                         ''''        \n"
		"      '-~~.-.~~~~~.---.~~~.-'                '''--..~~~.-       \n"
		"     '.~.-''-.~~~.-''-.~~~~~~.-'       ''--..~~~~~~~~~~~.'      \n"
		"   '-~~~~.-'''-..-''-.~~~~~~~~~~.---..~~~~~~~~~.--.~~~~~~-      \n"
		"  '~oooo~~~.--'--'--~~~~~~~~~~~~~~~~~~~~~~~~~~.'   -~~~~~.'     \n"
		"'-~ooooooo~..-'''''-------.~~~~~~~~~~~~~~~~~~~.-''-.~~~~~~-'    \n"
		".ooooo~.--------'----''''-.~~~~~~~~~~.-''-~~~~~~..---~~~~~.-    \n"
		"ooooooo.---.~o~.--.~~~....~~~~~~~~~~~-'  '----~~-   '.~~ooo.'   \n"
		"'-~ooooo~~oooo~.--.oooooooo~~~~~~~~~.''      '.~.'   -oooooo-   \n"
		"   '-~oooooooo~.---~oooooooo~.-'' -o.'     -~oooo.'  '.ooooo~'  \n"
		"      '.~oooooo~~~~ooooooo.'   ''-.ooo~-   -~oooo~-   -~ooooo.' \n"
		"        ''.oooooooooooooo.'  '.oooooooo~'  '.ooooo.'  '.ooooo~- \n"
		"           '-.oooooooooo~'  '.oooooooooo.   '.o~ooo-   '~ooooo. \n"
		"              '-~ooooooo~'   -oooooooooo~-      -~o~---.~ooooooo\n"
		"                 -~oooooo.'   -.~o~..~oooo.'  ''.~oooooooooooo~~\n"
		"                 '.ooooooo.'         '~ooooooooooooooooo~.--''  \n"
		"                  '~oooooooo.--'''-.~oooooooooooo~..-''         \n"
		"                   -oooooooooooooooooooooo~..-''                \n"
		"                   '.oooooooooooooo~..-''                       \n"
		"                    -~oooooo~..-''                              \n"
		"                     ~ooo~.                                     \n";
        qDebug("%s", qPrintable(logo));

        qDebug() << "AsteriskCTI Server version" << APP_VERSION_LONG;
        qDebug() << "Based on Qt" << QT_VERSION_STR;
        qDebug() << "Built on " __DATE__ " at " __TIME__;
        qDebug() << "Copyright" <<  APP_YEAR << ". All rights reserved.";
        qDebug() << "The program is provided AS IS with NO WARRANTY OF ANY ";
        qDebug() << "KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY";
        qDebug() << "AND FITNESS FOR A PARTICULAR PURPOSE.";

        qInstallMsgHandler(logHandler);

        if (application.buildNewCoreTcpServer() == 0) {
            return 1;
        }
        return application.exec();
    }
}

