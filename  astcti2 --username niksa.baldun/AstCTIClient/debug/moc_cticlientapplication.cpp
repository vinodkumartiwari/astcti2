/****************************************************************************
** Meta object code from reading C++ file 'cticlientapplication.h'
**
** Created: Fri Mar 19 10:03:43 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../cticlientapplication.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cticlientapplication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CtiClientApplication[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   22,   21,   21, 0x05,
      73,   60,   21,   21, 0x05,
     126,  115,   21,   21, 0x05,
     155,   21,   21,   21, 0x05,
     179,  171,   21,   21, 0x05,
     216,  199,   21,   21, 0x05,
     272,  265,   21,   21, 0x05,
     301,  291,   21,   21, 0x05,

 // slots: signature, parameters, type, tag, flags
     337,  319,   21,   21, 0x0a,
     366,   21,   21,   21, 0x0a,
     380,   21,   21,   21, 0x0a,
     397,   21,   21,   21, 0x0a,
     413,  406,   21,   21, 0x0a,
     453,  449,   21,   21, 0x0a,
     484,  476,   21,   21, 0x0a,
     513,   21,   21,   21, 0x08,
     531,   21,   21,   21, 0x08,
     564,  552,   21,   21, 0x08,
     625,  613,   21,   21, 0x08,
     667,   21,   21,   21, 0x08,
     681,   21,   21,   21, 0x08,
     700,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CtiClientApplication[] = {
    "CtiClientApplication\0\0astCTICall\0"
    "eventReceived(AstCTICall*)\0servicesList\0"
    "servicesReceived(QHash<QString,QString>*)\0"
    "queuesList\0queuesReceived(QStringList*)\0"
    "pauseAccepted()\0message\0pauseError(QString)\0"
    "message,severity\0"
    "newMessage(QString,QSystemTrayIcon::MessageIcon)\0"
    "status\0statusChange(bool)\0skipCheck\0"
    "closeWindow(bool)\0username,password\0"
    "loginAccept(QString,QString)\0loginReject()\0"
    "changePassword()\0logOff()\0window\0"
    "browserWindowClosed(BrowserWindow*)\0"
    "url\0newBrowserWindow(QUrl)\0process\0"
    "applicationClosed(QProcess*)\0"
    "socketConnected()\0socketDisconnected()\0"
    "socketState\0"
    "socketStateChanged(QAbstractSocket::SocketState)\0"
    "socketError\0socketError(QAbstractSocket::SocketError)\0"
    "receiveData()\0idleTimerElapsed()\0"
    "connectTimerElapsed()\0"
};

const QMetaObject CtiClientApplication::staticMetaObject = {
    { &QtSingleApplication::staticMetaObject, qt_meta_stringdata_CtiClientApplication,
      qt_meta_data_CtiClientApplication, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CtiClientApplication::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CtiClientApplication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CtiClientApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CtiClientApplication))
        return static_cast<void*>(const_cast< CtiClientApplication*>(this));
    return QtSingleApplication::qt_metacast(_clname);
}

int CtiClientApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QtSingleApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: eventReceived((*reinterpret_cast< AstCTICall*(*)>(_a[1]))); break;
        case 1: servicesReceived((*reinterpret_cast< QHash<QString,QString>*(*)>(_a[1]))); break;
        case 2: queuesReceived((*reinterpret_cast< QStringList*(*)>(_a[1]))); break;
        case 3: pauseAccepted(); break;
        case 4: pauseError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: newMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QSystemTrayIcon::MessageIcon(*)>(_a[2]))); break;
        case 6: statusChange((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: closeWindow((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: loginAccept((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 9: loginReject(); break;
        case 10: changePassword(); break;
        case 11: logOff(); break;
        case 12: browserWindowClosed((*reinterpret_cast< BrowserWindow*(*)>(_a[1]))); break;
        case 13: newBrowserWindow((*reinterpret_cast< QUrl(*)>(_a[1]))); break;
        case 14: applicationClosed((*reinterpret_cast< QProcess*(*)>(_a[1]))); break;
        case 15: socketConnected(); break;
        case 16: socketDisconnected(); break;
        case 17: socketStateChanged((*reinterpret_cast< QAbstractSocket::SocketState(*)>(_a[1]))); break;
        case 18: socketError((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 19: receiveData(); break;
        case 20: idleTimerElapsed(); break;
        case 21: connectTimerElapsed(); break;
        default: ;
        }
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void CtiClientApplication::eventReceived(AstCTICall * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CtiClientApplication::servicesReceived(QHash<QString,QString> * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CtiClientApplication::queuesReceived(QStringList * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CtiClientApplication::pauseAccepted()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void CtiClientApplication::pauseError(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void CtiClientApplication::newMessage(const QString & _t1, QSystemTrayIcon::MessageIcon _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CtiClientApplication::statusChange(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void CtiClientApplication::closeWindow(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
