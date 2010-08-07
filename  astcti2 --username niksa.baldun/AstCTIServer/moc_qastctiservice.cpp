/****************************************************************************
** Meta object code from reading C++ file 'qastctiservice.h'
**
** Created: Mon Mar 15 13:06:04 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qastctiservice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qastctiservice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAstCTIService[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      23,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   15,   42,   15, 0x0a,
      63,   54,   15,   15, 0x0a,
      83,   54,   15,   15, 0x0a,
     103,   54,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QAstCTIService[] = {
    "QAstCTIService\0\0result\0loadComplete(bool)\0"
    "bool\0load()\0bMayLoad\0loadOperators(bool)\0"
    "loadVariables(bool)\0loadActions(bool)\0"
};

const QMetaObject QAstCTIService::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QAstCTIService,
      qt_meta_data_QAstCTIService, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QAstCTIService::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QAstCTIService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QAstCTIService::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAstCTIService))
        return static_cast<void*>(const_cast< QAstCTIService*>(this));
    return QObject::qt_metacast(_clname);
}

int QAstCTIService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: loadComplete((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 1: { bool _r = load();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: loadOperators((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 3: loadVariables((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        case 4: loadActions((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QAstCTIService::loadComplete(const bool & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
