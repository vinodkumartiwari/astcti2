/****************************************************************************
** Meta object code from reading C++ file 'compactwindow.h'
**
** Created: Fri Mar 19 10:03:45 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../compactwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'compactwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CompactWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      32,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      48,   41,   14,   14, 0x0a,
      81,   64,   14,   14, 0x0a,
     131,   14,   14,   14, 0x0a,
     143,   14,   14,   14, 0x0a,
     151,   14,   14,   14, 0x0a,
     175,  168,   14,   14, 0x0a,
     197,  187,   14,   14, 0x0a,
     215,  208,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CompactWindow[] = {
    "CompactWindow\0\0changePassword()\0"
    "logOff()\0status\0setStatus(bool)\0"
    "message,severity\0"
    "showMessage(QString,QSystemTrayIcon::MessageIcon)\0"
    "placeCall()\0about()\0minimizeToTray()\0"
    "paused\0pause(bool)\0skipCheck\0quit(bool)\0"
    "reason\0iconActivated(QSystemTrayIcon::ActivationReason)\0"
};

const QMetaObject CompactWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CompactWindow,
      qt_meta_data_CompactWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CompactWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CompactWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CompactWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CompactWindow))
        return static_cast<void*>(const_cast< CompactWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int CompactWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: changePassword(); break;
        case 1: logOff(); break;
        case 2: setStatus((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: showMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QSystemTrayIcon::MessageIcon(*)>(_a[2]))); break;
        case 4: placeCall(); break;
        case 5: about(); break;
        case 6: minimizeToTray(); break;
        case 7: pause((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: quit((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: iconActivated((*reinterpret_cast< QSystemTrayIcon::ActivationReason(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void CompactWindow::changePassword()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CompactWindow::logOff()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
