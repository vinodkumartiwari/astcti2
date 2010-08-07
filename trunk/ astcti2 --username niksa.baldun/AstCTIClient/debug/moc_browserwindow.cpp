/****************************************************************************
** Meta object code from reading C++ file 'browserwindow.h'
**
** Created: Fri Mar 19 09:33:01 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../browserwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'browserwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BrowserWindow[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      45,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      63,   14,   14,   14, 0x08,
      84,   14,   14,   14, 0x08,
     105,   14,   14,   14, 0x08,
     128,   14,   14,   14, 0x08,
     149,   14,   14,   14, 0x08,
     172,  168,   14,   14, 0x08,
     207,  198,   14,   14, 0x08,
     239,  233,   14,   14, 0x08,
     274,  269,   14,   14, 0x08,
     308,   14,   14,   14, 0x08,
     335,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BrowserWindow[] = {
    "BrowserWindow\0\0windowClosing(BrowserWindow*)\0"
    "linkClicked(QUrl)\0prevButton_clicked()\0"
    "nextButton_clicked()\0reloadButton_clicked()\0"
    "stopButton_clicked()\0goButton_clicked()\0"
    "url\0webView_linkClicked(QUrl)\0progress\0"
    "webView_loadProgress(int)\0title\0"
    "webView_titleChanged(QString)\0text\0"
    "webView_statusBarMessage(QString)\0"
    "webView_loadFinished(bool)\0"
    "webView_loadStarted()\0"
};

const QMetaObject BrowserWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_BrowserWindow,
      qt_meta_data_BrowserWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BrowserWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BrowserWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BrowserWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BrowserWindow))
        return static_cast<void*>(const_cast< BrowserWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int BrowserWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: windowClosing((*reinterpret_cast< BrowserWindow*(*)>(_a[1]))); break;
        case 1: linkClicked((*reinterpret_cast< QUrl(*)>(_a[1]))); break;
        case 2: prevButton_clicked(); break;
        case 3: nextButton_clicked(); break;
        case 4: reloadButton_clicked(); break;
        case 5: stopButton_clicked(); break;
        case 6: goButton_clicked(); break;
        case 7: webView_linkClicked((*reinterpret_cast< QUrl(*)>(_a[1]))); break;
        case 8: webView_loadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: webView_titleChanged((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: webView_statusBarMessage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: webView_loadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: webView_loadStarted(); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void BrowserWindow::windowClosing(BrowserWindow * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BrowserWindow::linkClicked(QUrl _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
