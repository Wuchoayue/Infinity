/****************************************************************************
** Meta object code from reading C++ file 'playlistview.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../playlistview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'playlistview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PlayListView_t {
    const uint offsetsAndSize[16];
    char stringdata0[73];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_PlayListView_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_PlayListView_t qt_meta_stringdata_PlayListView = {
    {
QT_MOC_LITERAL(0, 12), // "PlayListView"
QT_MOC_LITERAL(13, 11), // "changeMedia"
QT_MOC_LITERAL(25, 0), // ""
QT_MOC_LITERAL(26, 4), // "path"
QT_MOC_LITERAL(31, 8), // "preMedia"
QT_MOC_LITERAL(40, 7), // "noMedia"
QT_MOC_LITERAL(48, 9), // "haveMedia"
QT_MOC_LITERAL(58, 14) // "changePlayList"

    },
    "PlayListView\0changeMedia\0\0path\0preMedia\0"
    "noMedia\0haveMedia\0changePlayList"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlayListView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   44,    2, 0x06,    1 /* Public */,
       4,    1,   47,    2, 0x06,    3 /* Public */,
       5,    0,   50,    2, 0x06,    5 /* Public */,
       6,    0,   51,    2, 0x06,    6 /* Public */,
       7,    0,   52,    2, 0x06,    7 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PlayListView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlayListView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->changeMedia((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->preMedia((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->noMedia(); break;
        case 3: _t->haveMedia(); break;
        case 4: _t->changePlayList(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PlayListView::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayListView::changeMedia)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PlayListView::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayListView::preMedia)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PlayListView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayListView::noMedia)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PlayListView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayListView::haveMedia)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PlayListView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PlayListView::changePlayList)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject PlayListView::staticMetaObject = { {
    QMetaObject::SuperData::link<QListView::staticMetaObject>(),
    qt_meta_stringdata_PlayListView.offsetsAndSize,
    qt_meta_data_PlayListView,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_PlayListView_t
, QtPrivate::TypeAndForceComplete<PlayListView, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>



>,
    nullptr
} };


const QMetaObject *PlayListView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlayListView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlayListView.stringdata0))
        return static_cast<void*>(this);
    return QListView::qt_metacast(_clname);
}

int PlayListView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void PlayListView::changeMedia(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PlayListView::preMedia(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PlayListView::noMedia()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void PlayListView::haveMedia()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void PlayListView::changePlayList()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
