# Introduction #
AsteriskCTI depends on QtSingleCoreApplication. You may grab LGPL version from here:

[ftp://ftp.qt.nokia.com/qt/solutions/lgpl/qtsingleapplication-2.6_1-opensource.tar.gz](ftp://ftp.qt.nokia.com/qt/solutions/lgpl/qtsingleapplication-2.6_1-opensource.tar.gz)

Download and extract in a folder outside your astcti2 sources. Ex.

```
$ ls -la
drwxr-xr-x  4 user user 4096 2010-12-11 23:50 .
drwxr-xr-x 74 user user 4096 2010-12-12 00:26 ..
drwxr-xr-x 13 user user 4096 2010-12-11 23:55 astcti2
drwxr-xr-x  7 user user 4096 2009-12-16 11:43 qtsingleapplication-2.6_1-opensource
```

Then make sure your project references are updated to follow the right path. Check the includes in files AstCTIServer.pro and AstCTIClient.pro:

```
include(../../qtsingleapplication-2.6_1-opensource/src/qtsinglecoreapplication.pri)
```