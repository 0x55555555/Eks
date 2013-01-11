/****************************************************************************
** Meta object code from reading C++ file 'XVector2DWidget'
**
** Created: Mon 31. Dec 11:45:58 2012
**      by: The Qt Meta Object Compiler version 67 (Qt 5.0.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../XVector2DWidget"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'XVector2DWidget' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_XVector2DWidget_t {
    QByteArrayData data[7];
    char stringdata[76];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_XVector2DWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_XVector2DWidget_t qt_meta_stringdata_XVector2DWidget = {
    {
QT_MOC_LITERAL(0, 0, 15),
QT_MOC_LITERAL(1, 16, 12),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 13),
QT_MOC_LITERAL(4, 44, 8),
QT_MOC_LITERAL(5, 53, 11),
QT_MOC_LITERAL(6, 65, 9)
    },
    "XVector2DWidget\0valueChanged\0\0"
    "Eks::Vector2D\0setValue\0setReadOnly\0"
    "setValues\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_XVector2DWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
       4,    1,   37,    2, 0x0a,
       5,    1,   40,    2, 0x0a,
       6,    0,   43,    2, 0x08,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,

       0        // eod
};

void XVector2DWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        XVector2DWidget *_t = static_cast<XVector2DWidget *>(_o);
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast< Eks::Vector2D(*)>(_a[1]))); break;
        case 1: _t->setValue((*reinterpret_cast< Eks::Vector2D(*)>(_a[1]))); break;
        case 2: _t->setReadOnly((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->setValues(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (XVector2DWidget::*_t)(Eks::Vector2D );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&XVector2DWidget::valueChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject XVector2DWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_XVector2DWidget.data,
      qt_meta_data_XVector2DWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *XVector2DWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *XVector2DWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_XVector2DWidget.stringdata))
        return static_cast<void*>(const_cast< XVector2DWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int XVector2DWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void XVector2DWidget::valueChanged(Eks::Vector2D _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
