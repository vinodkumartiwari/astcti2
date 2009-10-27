/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Tue Oct 27 20:41:27 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      19,   12,   11,   11, 0x08,
      68,   11,   11,   11, 0x08,
      82,   11,   11,   11, 0x08,
      99,   11,   11,   11, 0x0a,
     113,   11,   11,   11, 0x0a,
     125,   11,   11,   11, 0x0a,
     133,   11,   11,   11, 0x0a,
     143,   11,   11,   11, 0x0a,
     151,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0reason\0"
    "iconActivated(QSystemTrayIcon::ActivationReason)\0"
    "showMessage()\0messageClicked()\0"
    "UpdateTimer()\0configure()\0about()\0"
    "aboutQt()\0login()\0pause()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

const QMetaObject *MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: iconActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        case 1: showMessage(); break;
        case 2: messageClicked(); break;
        case 3: UpdateTimer(); break;
        case 4: configure(); break;
        case 5: about(); break;
        case 6: aboutQt(); break;
        case 7: login(); break;
        case 8: pause(); break;
        default: ;
        }
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
