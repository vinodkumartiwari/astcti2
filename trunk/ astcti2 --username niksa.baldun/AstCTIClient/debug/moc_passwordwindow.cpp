/****************************************************************************
** Meta object code from reading C++ file 'passwordwindow.h'
**
** Created: Thu Mar 18 15:41:41 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../passwordwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'passwordwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PasswordWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      28,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PasswordWindow[] = {
    "PasswordWindow\0\0accepting()\0rejecting()\0"
};

const QMetaObject PasswordWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PasswordWindow,
      qt_meta_data_PasswordWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PasswordWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PasswordWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PasswordWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PasswordWindow))
        return static_cast<void*>(const_cast< PasswordWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int PasswordWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: accepting(); break;
        case 1: rejecting(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
