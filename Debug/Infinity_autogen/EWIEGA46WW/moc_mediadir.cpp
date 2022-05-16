/****************************************************************************
** Meta object code from reading C++ file 'mediadir.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../mediadir.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mediadir.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MediaDir_t {
    const uint offsetsAndSize[14];
    char stringdata0[93];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_MediaDir_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_MediaDir_t qt_meta_stringdata_MediaDir = {
    {
QT_MOC_LITERAL(0, 8), // "MediaDir"
QT_MOC_LITERAL(9, 25), // "on_playAll_button_clicked"
QT_MOC_LITERAL(35, 0), // ""
QT_MOC_LITERAL(36, 6), // "pathes"
QT_MOC_LITERAL(43, 23), // "addToPlayList_triggered"
QT_MOC_LITERAL(67, 4), // "path"
QT_MOC_LITERAL(72, 20) // "close_button_clicked"

    },
    "MediaDir\0on_playAll_button_clicked\0\0"
    "pathes\0addToPlayList_triggered\0path\0"
    "close_button_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MediaDir[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   32,    2, 0x06,    1 /* Public */,
       4,    1,   35,    2, 0x06,    3 /* Public */,
       6,    0,   38,    2, 0x06,    5 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QStringList,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,

       0        // eod
};

void MediaDir::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MediaDir *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->on_playAll_button_clicked((*reinterpret_cast< std::add_pointer_t<QList<QString>>>(_a[1]))); break;
        case 1: _t->addToPlayList_triggered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->close_button_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MediaDir::*)(QList<QString> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MediaDir::on_playAll_button_clicked)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MediaDir::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MediaDir::addToPlayList_triggered)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MediaDir::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MediaDir::close_button_clicked)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject MediaDir::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_MediaDir.offsetsAndSize,
    qt_meta_data_MediaDir,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_MediaDir_t
, QtPrivate::TypeAndForceComplete<MediaDir, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QList<QString>, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<QString, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>



>,
    nullptr
} };


const QMetaObject *MediaDir::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MediaDir::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MediaDir.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MediaDir::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void MediaDir::on_playAll_button_clicked(QList<QString> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MediaDir::addToPlayList_triggered(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MediaDir::close_button_clicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
