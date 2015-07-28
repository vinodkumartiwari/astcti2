## General QTCreator Setup ##
**Please follow these steps to get QTCreator up and running to develop AsteriskCTI.**

  1. Download the right QTCreator LGPL version from [Nokia](http://qt.nokia.com/downloads).
  1. Run the setup. If - like me - you are on GNU/Linux, you need to make the downloaded .bin file executable. Do a `chmod u+x qt-sdk-linux-[your-version-here].bin` and then a `./qt-sdk-linux-[your-version-here].bin`
  1. Once your QTCreator is installed, you need to build the MySQL Driver. Please check that you've mysql client libraries and development files installed before continue.

## MySQL Driver ##
### Windows ###

  * Download MySQL Server from [MySQL Website](http://www.mysql.com/downloads/). Note, i tested the win32 version and it seems to work fine.
  * Install MySQL to your C:\ drive in the C:\mysql path. Please include development files when install.
  * Open a dos prompt and `cd %QTDIR%\src\plugins\sqldrivers\mysql`
  * Edit the file mysql.pro and add these lines: `INCLUDEPATH+=c:\MYSQL\INCLUDE LIBS+=C:\MYSQL\LIB\OPT\libmysql.lib`
  * Execute `qmake` and then `mingw32-make`
  * Check that in `%QTDIR%\plugins\sqldrivers` you have the following files: `libqsqlmysql4.a, libqsqlmysqld4.a, libqsqlmysql4.dll, libqsqlmysqld4.dll`

### GNU/Linux ###
  * open a terminal and chdir into your qtsdk directory `qtsdk/src/plugins/sqldrivers/mysql`
  * Execute `qmake -o Makefile "INCLUDEPATH+=/usr/include" "LIBS+=-L/usr/lib -lmysqlclient_r" mysql.pro && make`
  * Change the above line to match path for your includes and libraries and make sure to have libmysqlclient-dev libraries installed.
  * Copy the `libqsqlmysql.so` file in the `qtsdk/qt/plugins/sqldriver` directory.

### OSX ###
_I assume you don't want to install a full instance of MySQL Server on OSX. Therefore we will use the source code of MySQL Server to extract the libraries and header files needed. However, these instructions are readily applicable to the case where you want to install MySQL from the dmg file. In this case, you should change INCLUDEPATH and LIBS._

  * Download and extract MySQL Server sources for OSX (you should download a .tar.gz file and extract it). I got them in my ~/Downloads folder.
  * From QT Download page, grab the QT sources (see under LGPL / QT: Framework Only) and extract them. I got them also in my ~/Downloads folder.
  * Now we should copy MySQL libraries to /usr/local/lib. Open Terminal.app and do a sudo cp -PR /Users/YOURUSERHERE/Downloads/mysql-5.5.12-osx10.6-x86\_64/lib /usr/local/lib/ (Please change path according to downloaded version)
  * We are ready to build MySQL driver. From QTCreator, open the driver project from /Users/YOURUSERHERE/Downloads/qt-everywhere-opensource-src-4.7.3/src/plugins/sqldrivers/mysql (Change path according to downloaded version).
  * When the MySQL driver project is opened in QTCreator, edit the project file and add the following lines:
```
INCLUDEPATH+=/Users/YOURUSERHERE/Downloads/mysql-5.5.12-osx10.6-x86_64/include
LIBS+=-L/usr/local/lib -lmysqlclient_r
```
  * Build the project with debug and release configurations.
  * Once builded, you should find the builded libraries under /Users/YOURUSERHERE/Downloads/qt-everywhere-opensource-src-4.7.3/src/plugins/sqldrivers/mysql-build-desktop.
  * Copy the two libraries (libqsqlmysql.dylib and libqsqlmysql\_debug.dylib) in your QT Sdk path under the directory plugins/sqldrivers. (Mine is in ~/QtSDK/Desktop/Qt/473/gcc/plugins/sqldrivers).

That's all.