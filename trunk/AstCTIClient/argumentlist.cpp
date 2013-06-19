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
 * Based on example by S. Alan Ezust (sae@mcs.suffolk.edu)
 * http://cartan.cas.suffolk.edu/~zxu/teaching/F-12/C331/slides/argumentlist.html
 */

#include <QCoreApplication>
#include "argumentlist.h"

ArgumentList::ArgumentList()
{
	if (qApp != NULL) {
		// qApp is a global pointer to the current qApplication
		this->argsToStringlist(qApp->argc(), qApp->argv());
	}
}

ArgumentList::ArgumentList(int argc, char *argv[])
{
	this->argsToStringlist(argc, argv);
}

bool ArgumentList::getSwitch(const QString &option)
{
	if (option.isEmpty())
		return false;

	bool isLongSwitch = option.length() > 1;
	QString fullOption = isLongSwitch ? QString("--%1").arg(option) : QString("-%1").arg(option);

	const int listSize = this->size();
	for (int i = 0; i < listSize; i++) {
		if (fullOption == this->at(i)) {
			this->removeAt(i);
			return true;
		}
		if (!isLongSwitch) {
			QString arg = this->at(i);
			if (this->isSwitch(arg) && !this->isLongSwitch(arg)) {
				if (arg.contains(option.at(0))) {
					arg.replace(option, "");
					if (arg.length() == 1)
						this->removeAt(i);
					else
						this->replace(i, arg);
					return true;
				}
			}
		}
	}

	return false;
}

int ArgumentList::getSwitchMulti(const QString &option)
{
	int count = 0;

	if (option.length() == 1) {
		//Must be a short switch
		const int listSize = this->size();
		for (int i = 0; i < listSize; i++) {
			QString arg = this->at(i);
			if (this->isSwitch(arg) && !this->isLongSwitch(arg)) {
				int localCount = arg.count(option.at(0));
				if (localCount > 0) {
					arg.replace(option, "");
					if (arg.length() == 1)
						this->removeAt(i);
					else
						this->replace(i, arg);
					count += localCount;
				}
			}
		}
	}

	return count;
}

QString ArgumentList::getSwitchArg(const QString &option, const QString &defaultValue,
								   bool acceptArgWithoutSpace)
{
	if (option.isEmpty() || this->isEmpty())
		return defaultValue;

	bool isLongSwitch = option.length() > 1;
	QString fullOption = isLongSwitch ? QString("--%1").arg(option) : QString("-%1").arg(option);

	QString retVal = defaultValue;

	const int listSize = this->size();
	for (int i = 0; i < listSize; i++) {
		if (fullOption == this->at(i)) {
			if (i < listSize - 1) {
				if (!this->isSwitch(this->at(i + 1))) {
					retVal = this->at(i + 1);
					this->removeAt(i + 1);
					this->removeAt(i);
				}
			}
			break;
		}
		if (!isLongSwitch) {
			QString arg = this->at(i);
			if (this->isSwitch(arg) && !this->isLongSwitch(arg)) {
				int charIndex = arg.indexOf(option);
				if (charIndex > 0) {
					if (charIndex == arg.length() - 1) {
						//Option is at the last position in argument, so we search for next one
						if (i < listSize - 1) {
							if (!this->isSwitch(this->at(i + 1))) {
								retVal = this->at(i + 1);
								this->removeAt(i + 1);
								arg.chop(1);
								this->replace(i, arg);
							}
						}
					} else {
						//Option is a the middle of an argument, so we take the remainder
						if (acceptArgWithoutSpace) {
							retVal = arg.mid(charIndex);
							if (charIndex == 1) {
								this->removeAt(i);
							} else {
								arg.chop(arg.length() - charIndex);
								this->replace(i, arg);
							}
						}
					}
					break;
				}
			}
		}
	}

	return retVal;
}

void ArgumentList::argsToStringlist(int argc, char *argv[])
{
	//We start at 1 because first argument is the path of application executable,
	//which we don't care about
	for (int i = 1; i < argc; i++)
		this->append(argv[i]);
}

bool ArgumentList::isSwitch(const QString &arg) const
{
	return arg.startsWith('-');
}

bool ArgumentList::isLongSwitch(const QString &arg) const
{
	return arg.startsWith("--");
}
