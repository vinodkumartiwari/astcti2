/****************************************************************************
** Meta object code from reading C++ file 'coretcpserver.h'
**
** Created: Mon Mar 15 13:06:08 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "coretcpserver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'coretcpserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CoreTcpServer[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   15,   14,   14, 0x05,
      48,   14,   14,   14, 0x05,
      66,   14,   14,   14, 0x05,
      93,   90,   14,   14, 0x05,
     126,   90,   14,   14, 0x05,
     160,   90,   14,   14, 0x05,
     191,   90,   14,   14, 0x05,
     223,   14,   14,   14, 0x05,
     270,  245,   14,   14, 0x05,
     315,  245,   14,   14, 0x05,
     424,  361,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     482,  473,   14,   14, 0x0a,
     499,  473,   14,   14, 0x0a,
     528,  519,   14,   14, 0x09,
     580,  562,   14,   14, 0x09,
     616,  610,   14,   14, 0x09,
     638,   15,   14,   14, 0x09,
     675,  660,   14,   14, 0x09,
     702,  695,   14,   14, 0x09,
     766,  749,   14,   14, 0x09,
     826,  805,   14,   14, 0x09,
     867,   90,   14,   14, 0x09,
     900,   90,   14,   14, 0x09,
     934,   90,   14,   14, 0x09,
     965,   90,   14,   14, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_CoreTcpServer[] = {
    "CoreTcpServer\0\0data\0sendDataFromServer(QString)\0"
    "serverIsClosing()\0amiClientDisconnected()\0"
    "cl\0amiClientPauseIn(ClientManager*)\0"
    "amiClientPauseOut(ClientManager*)\0"
    "amiClientLogin(ClientManager*)\0"
    "amiClientLogoff(ClientManager*)\0"
    "ctiClientLogoffSent()\0identifier,result,reason\0"
    "ctiClientPauseInResult(QString,bool,QString)\0"
    "ctiClientPauseOutResult(QString,bool,QString)\0"
    "identifier,actionId,commandName,responseString,responseMessage\0"
    "ctiResponse(QString,int,QString,QString,QString)\0"
    "username\0addUser(QString)\0removeUser(QString)\0"
    "exten,cl\0addClient(QString,ClientManager*)\0"
    "oldexten,newexten\0changeClient(QString,QString)\0"
    "exten\0removeClient(QString)\0"
    "notifyClient(QString)\0closeTheSocket\0"
    "stopTheServer(bool)\0status\0"
    "amiConnectionStatusChange(AMIConnectionStatus)\0"
    "eventid,the_call\0"
    "receiveCtiEvent(AMIEvent,QAstCTICall*)\0"
    "actionId,the_command\0"
    "receiveCtiResponse(int,AsteriskCommand*)\0"
    "ctiClientPauseIn(ClientManager*)\0"
    "ctiClientPauseOut(ClientManager*)\0"
    "ctiClientLogin(ClientManager*)\0"
    "ctiClientLogoff(ClientManager*)\0"
};

const QMetaObject CoreTcpServer::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_CoreTcpServer,
      qt_meta_data_CoreTcpServer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CoreTcpServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CoreTcpServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CoreTcpServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CoreTcpServer))
        return static_cast<void*>(const_cast< CoreTcpServer*>(this));
    return QTcpServer::qt_metacast(_clname);
}

int CoreTcpServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sendDataFromServer((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: serverIsClosing(); break;
        case 2: amiClientDisconnected(); break;
        case 3: amiClientPauseIn((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 4: amiClientPauseOut((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 5: amiClientLogin((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 6: amiClientLogoff((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 7: ctiClientLogoffSent(); break;
        case 8: ctiClientPauseInResult((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const bool(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 9: ctiClientPauseOutResult((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const bool(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 10: ctiResponse((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5]))); break;
        case 11: addUser((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: removeUser((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: addClient((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< ClientManager*(*)>(_a[2]))); break;
        case 14: changeClient((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 15: removeClient((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: notifyClient((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: stopTheServer((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: amiConnectionStatusChange((*reinterpret_cast< AMIConnectionStatus(*)>(_a[1]))); break;
        case 19: receiveCtiEvent((*reinterpret_cast< const AMIEvent(*)>(_a[1])),(*reinterpret_cast< QAstCTICall*(*)>(_a[2]))); break;
        case 20: receiveCtiResponse((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< AsteriskCommand*(*)>(_a[2]))); break;
        case 21: ctiClientPauseIn((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 22: ctiClientPauseOut((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 23: ctiClientLogin((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        case 24: ctiClientLogoff((*reinterpret_cast< ClientManager*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 25;
    }
    return _id;
}

// SIGNAL 0
void CoreTcpServer::sendDataFromServer(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CoreTcpServer::serverIsClosing()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void CoreTcpServer::amiClientDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void CoreTcpServer::amiClientPauseIn(ClientManager * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CoreTcpServer::amiClientPauseOut(ClientManager * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void CoreTcpServer::amiClientLogin(ClientManager * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CoreTcpServer::amiClientLogoff(ClientManager * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void CoreTcpServer::ctiClientLogoffSent()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void CoreTcpServer::ctiClientPauseInResult(const QString & _t1, const bool & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void CoreTcpServer::ctiClientPauseOutResult(const QString & _t1, const bool & _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void CoreTcpServer::ctiResponse(const QString & _t1, const int _t2, const QString & _t3, const QString & _t4, const QString & _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_END_MOC_NAMESPACE
