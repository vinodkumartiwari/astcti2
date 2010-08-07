/****************************************************************************
** Meta object code from reading C++ file 'qastctiapplication.h'
**
** Created: Sat Oct 24 19:33:05 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qastctiapplication.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qastctiapplication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAstCTIApplication[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      27,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      51,   19,   46,   19, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QAstCTIApplication[] = {
    "QAstCTIApplication\0\0result\0"
    "loadComplete(bool)\0bool\0load()\0"
};

const QMetaObject QAstCTIApplication::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QAstCTIApplication,
      qt_meta_data_QAstCTIApplication, 0 }
};

const QMetaObject *QAstCTIApplication::metaObject() const
{
    return &staticMetaObject;
}

void *QAstCTIApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAstCTIApplication))
        return static_cast<void*>(const_cast< QAstCTIApplication*>(this));
    return QObject::qt_metacast(_clname);
}

int QAstCTIApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: loadComplete((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 1: { bool _r = load();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QAstCTIApplication::loadComplete(const bool & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
