/****************************************************************************
** Meta object code from reading C++ file 'playercontrols.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../playercontrols.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'playercontrols.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PlayerControls_t {
    const uint offsetsAndSize[28];
    char stringdata0[224];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_PlayerControls_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_PlayerControls_t qt_meta_stringdata_PlayerControls = {
    {
QT_MOC_LITERAL(0, 14), // "PlayerControls"
QT_MOC_LITERAL(15, 25), // "changeMediaDirShow_signal"
QT_MOC_LITERAL(41, 0), // ""
QT_MOC_LITERAL(42, 13), // "invert_signal"
QT_MOC_LITERAL(56, 19), // "durationStep_signal"
QT_MOC_LITERAL(76, 15), // "playMode_signal"
QT_MOC_LITERAL(92, 13), // "preOne_signal"
QT_MOC_LITERAL(106, 17), // "playStatus_signal"
QT_MOC_LITERAL(124, 14), // "nextOne_signal"
QT_MOC_LITERAL(139, 16), // "playSpeed_signal"
QT_MOC_LITERAL(156, 13), // "volume_signal"
QT_MOC_LITERAL(170, 19), // "volumeGraphy_signal"
QT_MOC_LITERAL(190, 17), // "fullScreen_signal"
QT_MOC_LITERAL(208, 15) // "showList_signal"

    },
    "PlayerControls\0changeMediaDirShow_signal\0"
    "\0invert_signal\0durationStep_signal\0"
    "playMode_signal\0preOne_signal\0"
    "playStatus_signal\0nextOne_signal\0"
    "playSpeed_signal\0volume_signal\0"
    "volumeGraphy_signal\0fullScreen_signal\0"
    "showList_signal"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlayerControls[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      12,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x06,    1 /* Public */,
       3,    0,   87,    2, 0x06,    2 /* Public */,
       4,    1,   88,    2, 0x06,    3 /* Public */,
       5,    1,   91,    2, 0x06,    5 /* Public */,
       6,    0,   94,    2, 0x06,    7 /* Public */,
       7,    0,   95,    2, 0x06,    8 /* Public */,
       8,    0,   96,    2, 0x06,    9 /* Public */,
       9,    1,   97,    2, 0x06,   10 /* Public */,
      10,    1,  100,    2, 0x06,   12 /* Public */,
      11,    0,  103,    2, 0x06,   14 /* Public */,
      12,    0,  104,    2, 0x06,   15 /* Public */,
      13,    1,  105,    2, 0x06,   16 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void PlayerControls::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlayerControls *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->changeMediaDirShow_signal(); break;
        case 1: _t->invert_signal(); break;
        case 2: _t->durationStep_signal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->playMode_signal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->preOne_signal(); break;
        case 5: _t->playStatus_signal(); break;
        case 6: _t->nextOne_signal(); break;
        case 7: _t->playSpeed_signal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->volume_signal((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 9: _t->volumeGraphy_signal(); break;
        case 10: _t->fullScreen_signal(); break;
        case 11: _t->showList_signal((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PlayerControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::changeMediaDirShow_signal)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::invert_signal)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::durationStep_signal)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::playMode_signal)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::preOne_signal)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::playStatus_signal)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::nextOne_signal)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::playSpeed_signal)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::volume_signal)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::volumeGraphy_signal)) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::fullScreen_signal)) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (PlayerControls::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayerControls::showList_signal)) {
                *result = 11;
                return;
            }
        }
    }
}

const QMetaObject PlayerControls::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_PlayerControls.offsetsAndSize,
    qt_meta_data_PlayerControls,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_PlayerControls_t
, QtPrivate::TypeAndForceComplete<PlayerControls, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<int, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<bool, std::false_type>



>,
    nullptr
} };


const QMetaObject *PlayerControls::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlayerControls::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlayerControls.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int PlayerControls::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void PlayerControls::changeMediaDirShow_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void PlayerControls::invert_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void PlayerControls::durationStep_signal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PlayerControls::playMode_signal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PlayerControls::preOne_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void PlayerControls::playStatus_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void PlayerControls::nextOne_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void PlayerControls::playSpeed_signal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void PlayerControls::volume_signal(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void PlayerControls::volumeGraphy_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void PlayerControls::fullScreen_signal()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void PlayerControls::showList_signal(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 11, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
