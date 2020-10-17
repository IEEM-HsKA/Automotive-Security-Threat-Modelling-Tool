/****************************************************************************
** Meta object code from reading C++ file 'nodectrl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../nodectrl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nodectrl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_NodeCtrl_t {
    QByteArrayData data[15];
    char stringdata0[176];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NodeCtrl_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NodeCtrl_t qt_meta_stringdata_NodeCtrl = {
    {
QT_MOC_LITERAL(0, 0, 8), // "NodeCtrl"
QT_MOC_LITERAL(1, 9, 6), // "remove"
QT_MOC_LITERAL(2, 16, 0), // ""
QT_MOC_LITERAL(3, 17, 14), // "inputConnected"
QT_MOC_LITERAL(4, 32, 18), // "zodiac::PlugHandle"
QT_MOC_LITERAL(5, 51, 7), // "myInput"
QT_MOC_LITERAL(6, 59, 11), // "otherOutput"
QT_MOC_LITERAL(7, 71, 15), // "outputConnected"
QT_MOC_LITERAL(8, 87, 8), // "myOutput"
QT_MOC_LITERAL(9, 96, 10), // "otherInput"
QT_MOC_LITERAL(10, 107, 17), // "inputDisconnected"
QT_MOC_LITERAL(11, 125, 18), // "outputDisconnected"
QT_MOC_LITERAL(12, 144, 15), // "requestRenaming"
QT_MOC_LITERAL(13, 160, 7), // "oldName"
QT_MOC_LITERAL(14, 168, 7) // "newName"

    },
    "NodeCtrl\0remove\0\0inputConnected\0"
    "zodiac::PlugHandle\0myInput\0otherOutput\0"
    "outputConnected\0myOutput\0otherInput\0"
    "inputDisconnected\0outputDisconnected\0"
    "requestRenaming\0oldName\0newName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NodeCtrl[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    2,   45,    2, 0x08 /* Private */,
       7,    2,   50,    2, 0x08 /* Private */,
      10,    2,   55,    2, 0x08 /* Private */,
      11,    2,   60,    2, 0x08 /* Private */,
      12,    2,   65,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Bool,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    5,    6,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    8,    9,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    5,    6,
    QMetaType::Void, 0x80000000 | 4, 0x80000000 | 4,    8,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   13,   14,

       0        // eod
};

void NodeCtrl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NodeCtrl *_t = static_cast<NodeCtrl *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: { bool _r = _t->remove();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 1: _t->inputConnected((*reinterpret_cast< zodiac::PlugHandle(*)>(_a[1])),(*reinterpret_cast< zodiac::PlugHandle(*)>(_a[2]))); break;
        case 2: _t->outputConnected((*reinterpret_cast< zodiac::PlugHandle(*)>(_a[1])),(*reinterpret_cast< zodiac::PlugHandle(*)>(_a[2]))); break;
        case 3: _t->inputDisconnected((*reinterpret_cast< zodiac::PlugHandle(*)>(_a[1])),(*reinterpret_cast< zodiac::PlugHandle(*)>(_a[2]))); break;
        case 4: _t->outputDisconnected((*reinterpret_cast< zodiac::PlugHandle(*)>(_a[1])),(*reinterpret_cast< zodiac::PlugHandle(*)>(_a[2]))); break;
        case 5: _t->requestRenaming((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject NodeCtrl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NodeCtrl.data,
      qt_meta_data_NodeCtrl,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *NodeCtrl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NodeCtrl::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NodeCtrl.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NodeCtrl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
