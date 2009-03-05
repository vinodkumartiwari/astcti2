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
 *
 * Author: S. Alan Ezust sae@mcs.suffolk.edu
 * Code taken from: http://cartan.cas.suffolk.edu/oopdocbook/opensource/argumentlist.html
 */

#ifndef ARGUMENTLIST_H
#define ARGUMENTLIST_H

#include <QStringList>


/** @short a simple interface for processing command line arguments

  An object which provides a simple interface to the command
  line arguments of a program. The methods
  @ref getSwitch(QString) and @ref getSwitchArg(QString)
  allow you to process <b>and remove</b> the switches and switched
  arguments in the command line, so that the remaining
  entries in the stringlist can be processed as a
  uniform list.
<br>
  It also happens to be derived from
  QStringList, so you can use any of those
  methods on this object too.
<br>
This helps you avoid entirely using the C arrays
in your application.
 
<br />
Usage.:
<br />
<pre>
int main(int argc, char** argv) {
ArgumentList args(argc, argv);
bool verbose = args.getSwitch("-v");
// get all other switches
QString outputfile = args.getSwitchArg("-o", "default.out");
qout << args[0];  // prints the name of the executable
qout << args[1]; // prints the first unswitched argument
someObject.processEveryFile(args);
}
</pre>
 
@author S. Alan Ezust sae@mcs.suffolk.edu
@since qt 3.2.1

*/

class QArgumentList : public QStringList
{
  public:
    /**
    retrieve argument list from the qApp->argc() and argv() methods.
    Only works if a @ref QApplication(argc, argv) was already created.
    */
    QArgumentList();

    /**
     @param argc number of arguments
     @param argv an array of command line arguments, as supplied
       to main().
       @see argsToStringList()
     */
     
    QArgumentList(int argc, char* argv[]) {
        argsToStringlist(argc, argv);
    }

    QArgumentList(const QStringList& argumentList):
       QStringList(argumentList) {}


    /**
      finds <b>and removes</b> a switch from the string list, if it exists.
      @param option the switch to search for
      @return true if the switch was found
      */
    bool getSwitch(QString option);

    /**
    finds/removes a switch <b>and its accompanying argument</b>
    from the string list, if the switch is found. Does nothing if the
    switch is not found.
    @param option the switch to search for
    @param defaultReturnValue the return value if option is not found in the stringlist
    @return the argument following option, or defaultValue if the option
    is not found.
    */
    QString getSwitchArg(QString option, 
                         QString defaultRetVal=QString());
  private:
    /**
    (Re)loads argument lists into this object. This function is private because
    it is part of the implementation of the class and not intended to be used by
    client code.
    */
    void argsToStringlist(int argc,  char* argv[]);
};
#endif
