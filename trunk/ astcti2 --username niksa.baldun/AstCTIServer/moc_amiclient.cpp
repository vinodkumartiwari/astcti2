/****************************************************************************
** Meta object code from reading C++ file 'amiclient.h'
**
** Created: Mon Mar 15 13:06:00 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "amiclient.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'amiclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AMIClient[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: signature, parameters, type, tag, flags
      31,   11,   10,   10, 0x05,
      55,   50,   10,   10, 0x05,
      88,   10,   10,   10, 0x05,
     120,  104,   10,   10, 0x05,
     172,  152,   10,   10, 0x05,
     213,  206,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     260,   50,   10,   10, 0x0a,
     322,  288,   10,   10, 0x0a,
     373,   10,   10,   10, 0x08,
     398,  390,   10,   10, 0x08,
     437,   10,   10,   10, 0x08,
     452,   10,   10,   10, 0x08,
     493,   50,  469,   10, 0x08,
     524,  518,   10,   10, 0x08,
     572,  563,   10,   10, 0x08,
     626,  614,   10,   10, 0x08,
     687,  675,   10,   10, 0x08,
     729,   10,   10,   10, 0x08,

 // enums: name, flags, count, data

 // enum data: key, value

       0        // eod
};

static const char qt_meta_stringdata_AMIClient[] = {
    "AMIClient\0\0socketError,message\0"
    "error(int,QString)\0data\0"
    "receiveDataFromAsterisk(QString)\0"
    "threadStopped()\0eventId,theCall\0"
    "ctiEvent(AMIEvent,QAstCTICall*)\0"
    "actionId,theCommand\0"
    "ctiResponse(int,AsteriskCommand*)\0"
    "status\0amiConnectionStatusChange(AMIConnectionStatus)\0"
    "sendDataToAsterisk(QString)\0"
    "actionId,commandName,data,channel\0"
    "sendCommandToAsterisk(int,QString,QString,QString)\0"
    "buildTheSocket()\0message\0"
    "parseDataReceivedFromAsterisk(QString)\0"
    "performLogin()\0executeActions()\0"
    "QHash<QString,QString>*\0"
    "hashFromMessage(QString)\0event\0"
    "evaluateEvent(QHash<QString,QString>*)\0"
    "response\0evaluateResponse(QHash<QString,QString>*)\0"
    "socketState\0"
    "socketStateChanged(QAbstractSocket::SocketState)\0"
    "socketError\0socketError(QAbstractSocket::SocketError)\0"
    "socketDisconnected()\0"
};

const QMetaObject AMIClient::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_AMIClient,
      qt_meta_data_AMIClient, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AMIClient::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AMIClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AMIClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AMIClient))
        return static_cast<void*>(const_cast< AMIClient*>(this));
    return QThread::qt_metacast(_clname);
}

int AMIClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: error((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: receiveDataFromAsterisk((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: threadStopped(); break;
        case 3: ctiEvent((*reinterpret_cast< const AMIEvent(*)>(_a[1])),(*reinterpret_cast< QAstCTICall*(*)>(_a[2]))); break;
        case 4: ctiResponse((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< AsteriskCommand*(*)>(_a[2]))); break;
        case 5: amiConnectionStatusChange((*reinterpret_cast< AMIConnectionStatus(*)>(_a[1]))); break;
        case 6: sendDataToAsterisk((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: sendCommandToAsterisk((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 8: buildTheSocket(); break;
        case 9: parseDataReceivedFromAsterisk((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: performLogin(); break;
        case 11: executeActions(); break;
        case 12: { QHash<QString,QString>* _r = hashFromMessage((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QHash<QString,QString>**>(_a[0]) = _r; }  break;
        case 13: evaluateEvent((*reinterpret_cast< QHash<QString,QString>*(*)>(_a[1]))); break;
        case 14: evaluateResponse((*reinterpret_cast< QHash<QString,QString>*(*)>(_a[1]))); break;
        case 15: socketStateChanged((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
        case 16: socketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 17: socketDisconnected(); break;
        default: ;
        }
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void AMIClient::error(int _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AMIClient::receiveDataFromAsterisk(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AMIClient::threadStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void AMIClient::ctiEvent(const AMIEvent & _t1, QAstCTICall * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void AMIClient::ctiResponse(const int & _t1, AsteriskCommand * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void AMIClient::amiConnectionStatusChange(AMIConnectionStatus _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_END_MOC_NAMESPACE
