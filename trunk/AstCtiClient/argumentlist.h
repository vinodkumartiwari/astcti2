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
 * AstCtiClient.  If you copy code from other releases into a copy of GNU
 * AstCtiClient, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCtiClient, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 *
 * Based on example by S. Alan Ezust (sae@mcs.suffolk.edu)
 * http://cartan.cas.suffolk.edu/~zxu/teaching/F-12/C331/slides/argumentlist.html
 */

#ifndef ARGUMENTLIST_H
#define ARGUMENTLIST_H

#include <QStringList>

/** @short Interface for processing command line arguments

An object which provides an interface to the command line arguments of a program. The methods
@ref getSwitch(QString), @ref getSwitchMulti(QString) and @ref getSwitchArg(QString, QString, bool)
allow you to process <b>and remove</b> the switches and switched arguments in the command line,
so that the remaining entries in the string list can be processed as a uniform list.
<br>
Arguments to getSwitch functions are passed without dashes
(e.g "d" instead of "-d" or "debug" instead of "--debug")
<br>
Multiple short switches (single dash) can be grouped together (e.g. -dvt).
For getSwitchArg function, if you set acceptArgWithoutSpace to true,
argument can be provided without space (e.g. -e3 instead of -e 3).
It is not recommended to use this for non-numeric arguments.
Long switches (double dash) must be stand-alone and space is required.
<br>
It also happens to be derived from QStringList,
so you can use any of those methods on this object too.
<br>
This helps you avoid entirely using the C arrays in your application.

<br />
Usage.:
<br />
<pre>
int main(int argc, char** argv) {
	ArgumentList args(argc, argv);
	bool verbose = args.getSwitch("-v");
	int logLevel = args.getSwitchMulti("-d");
	QString outputfile = args.getSwitchArg("-o", "default.out");
	qout << args[0]; // prints the name of the executable
	qout << args[1]; // prints the first argument
}
</pre>

@author Niksa Baldun (niksa.baldun@gmail.com)
@since qt 4.8.4

*/

class ArgumentList : public QStringList
{
  public:
	/**
	Constructs an argument list from the qApp argc() and argv() methods.
	Only works if a @ref QApplication(argc, argv) was already created.
	*/
	ArgumentList();

	/**
	Constructs an argument list from the argc and argv[] parameters.
	@param arguments list of command line arguments, as supplied to QCoreApplication.
	@see argsToStringList()
	*/
	ArgumentList(const QStringList& arguments);

	/**
	Finds <b>and removes</b> a switch from the list, if it exists.
	@param option the switch to search for without dash(es)
	@return true if the switch was found
	*/
	bool getSwitch(const QString& option);

	/**
	Finds <b>and removes</b> all instances of the switch from the list, if it exists.
	@param option the switch to search for without dash(es)
	@return number of occurences of the switch
	*/
	int getSwitchMulti(const QString& option);

	/**
	Finds/removes a switch <b>and its accompanying argument</b>
	from the string list, if the switch is found.
	@param option the switch to search for without dash(es)
	@param defaultValue the return value if option is not found in the stringlist
	@param acceptArgWithoutSpace true if argument can be provided without space, default false
	@return the argument following option, or defaultValue if the option is not found.
	*/
	QString getSwitchArg(const QString& option, const QString& defaultValue=QString(),
						 bool acceptArgWithoutSpace=false);

  private:
	/**
	(Re)loads list of arguments into this object.
	@param arguments list of command line arguments, as supplied to QCoreApplication.
	*/
	void argsToStringlist(const QStringList& arguments);

	/**
	  Checks whether a given argument is a switch
	  @param argument to check
	  @return true if the argument is a switch
	*/
	bool isSwitch(const QString& arg) const;

	/**
	  Checks whether a given argument is a long switch (with double dash)
	  @param argument to check
	  @return true if the argument is a short switch
	*/
	bool isLongSwitch(const QString& arg) const;
};
#endif
