/****************************************************************************
** Meta object code from reading C++ file 'clientmanager.h'
**
** Created: Mon Mar 15 13:05:59 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "clientmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'clientmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ClientManager[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   15,   14,   14, 0x05,
      51,   42,   14,   14, 0x05,
     103,   85,   14,   14, 0x05,
     139,  133,   14,   14, 0x05,
     161,   15,   14,   14, 0x05,
     186,  183,   14,   14, 0x05,
     213,  183,   14,   14, 0x05,
     241,  183,   14,   14, 0x05,
     266,  183,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     292,   14,   14,   14, 0x0a,
     313,   14,   14,   14, 0x0a,
     334,   15,   14,   14, 0x0a,
     360,   15,   14,   14, 0x0a,
     387,   14,   14,   14, 0x0a,
     407,   14,   14,   14, 0x0a,
     463,  438,   14,   14, 0x0a,
     499,  438,   14,   14, 0x0a,
     599,  536,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ClientManager[] = {
    "ClientManager\0\0data\0dataReceived(QString)\0"
    "exten,cl\0addClient(QString,ClientManager*)\0"
    "oldexten,newexten\0changeClient(QString,QString)\0"
    "exten\0removeClient(QString)\0"
    "notifyServer(QString)\0cl\0"
    "ctiPauseIn(ClientManager*)\0"
    "ctiPauseOut(ClientManager*)\0"
    "ctiLogin(ClientManager*)\0"
    "ctiLogoff(ClientManager*)\0"
    "sockedDataReceived()\0socketDisconnected()\0"
    "sendDataToClient(QString)\0"
    "parseDataReceived(QString)\0"
    "disconnectRequest()\0unlockAfterSuccessfullLogoff()\0"
    "identifier,result,reason\0"
    "pauseInResult(QString,bool,QString)\0"
    "pauseOutResult(QString,bool,QString)\0"
    "identifier,actionId,commandName,responseString,responseMessage\0"
    "ctiResponse(QString,int,QString,QString,QString)\0"
};

const QMetaObject ClientManager::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ClientManager,
      qt_meta_data_ClientManager, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ClientManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ClientManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ClientManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ClientManager))
        return static_cast<void*>(const_cast< ClientManager*>(this));
    return QThread::qt_metacast(_clname);
}

int ClientManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dataReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: addClient((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< ClientManager*(*)>(_a[2]))); break;
        case 2: changeClient((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: removeClient((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: notifyServer((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: ctiPauseIn((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 6: ctiPauseOut((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 7: ctiLogin((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 8: ctiLogoff((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 9: sockedDataReceived(); break;
        case 10: socketDisconnected(); break;
        case 11: sendDataToClient((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: parseDataReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: disconnectRequest(); break;
        case 14: unlockAfterSuccessfullLogoff(); break;
        case 15: pauseInResult((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const bool(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 16: pauseOutResult((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const bool(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 17: ctiResponse((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5]))); break;
        default: ;
        }
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void ClientManager::dataReceived(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ClientManager::addClient(const QString & _t1, ClientManager * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ClientManager::changeClient(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ClientManager::removeClient(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ClientManager::notifyServer(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void ClientManager::ctiPauseIn(ClientManager * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void ClientManager::ctiPauseOut(ClientManager * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ClientManager::ctiLogin(ClientManager * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void ClientManager::ctiLogoff(ClientManager * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_END_MOC_NAMESPACE
