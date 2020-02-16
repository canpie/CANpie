/****************************************************************************
** Meta object code from reading C++ file 'node_cansocket.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "node_cansocket.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'node_cansocket.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CanSocket_t {
    QByteArrayData data[6];
    char stringdata0[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CanSocket_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CanSocket_t qt_meta_stringdata_CanSocket = {
    {
QT_MOC_LITERAL(0, 0, 9), // "CanSocket"
QT_MOC_LITERAL(1, 10, 24), // "onDisconnectEventHandler"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 21), // "onMessageEventHandler"
QT_MOC_LITERAL(4, 58, 8), // "uint32_t"
QT_MOC_LITERAL(5, 67, 11) // "ulFrameCntV"

    },
    "CanSocket\0onDisconnectEventHandler\0\0"
    "onMessageEventHandler\0uint32_t\0"
    "ulFrameCntV"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CanSocket[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08 /* Private */,
       3,    1,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,

       0        // eod
};

void CanSocket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CanSocket *_t = static_cast<CanSocket *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onDisconnectEventHandler(); break;
        case 1: _t->onMessageEventHandler((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject CanSocket::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CanSocket.data,
      qt_meta_data_CanSocket,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CanSocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CanSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CanSocket.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Napi::ObjectWrap<CanSocket>"))
        return static_cast< Napi::ObjectWrap<CanSocket>*>(this);
    return QObject::qt_metacast(_clname);
}

int CanSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
