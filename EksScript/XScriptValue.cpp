#include "XScriptValue.h"
#include "XScriptValueV8Internals.h"
#include "XScriptValueDartInternals.h"
#include "XScriptObject.h"
#include "XUnorderedMap"
#include "QStringList"
#include "XConvertFromScript.h"
#include "XConvertToScript.h"
#include "XInterface.h"
#include "XAssert"

struct XScriptValueInternal
  {
  static XScriptValueInternal *init(XScriptValue *o, const XScriptValue *other=0)
    {
    XScriptValueInternal *internal = (XScriptValueInternal*)o;
    new(internal) XScriptValueInternal;

    if(other)
      {
      const XScriptValueInternal *otherInternal = (const XScriptValueInternal*)other;
      internal->_object = otherInternal->_object;
      }

    return internal;
    }

  static void term(XScriptValue *o)
    {
    XScriptValueInternal *internal = (XScriptValueInternal*)o;
    (void)internal;
    internal->~XScriptValueInternal();
    }

  static const XScriptValueInternal *val(const XScriptValue *o)
    {
    const XScriptValueInternal *internal = (XScriptValueInternal*)o;
    return internal;
    }

  static XScriptValueInternal *val(XScriptValue *o)
    {
    XScriptValueInternal *internal = (XScriptValueInternal*)o;
    return internal;
    }

  mutable v8::Handle<v8::Value> _object;
  };
xCompileTimeAssert(sizeof(XScriptValue) == sizeof(XScriptValueInternal));
xCompileTimeAssert(sizeof(XScriptValue) == sizeof(Dart_Handle));

Dart_Handle& getDartHandle(const XScriptValue &obj)
  {
  struct Internal
    {
    Dart_Handle ptr;
    };

  return ((Internal*)(&obj))->ptr;
  }
Dart_Handle& getDartHandle(const XPersistentScriptValue &obj)
  {
  struct Internal
    {
    Dart_Handle ptr;
    };

  return ((Internal*)(&obj))->ptr;
  }

struct XPersistentScriptValueInternal
  {
  static XPersistentScriptValueInternal *init(XPersistentScriptValue *o, const XPersistentScriptValue *other=0)
    {
    XPersistentScriptValueInternal *internal = (XPersistentScriptValueInternal*)o;
    new(internal) XPersistentScriptValueInternal;

    if(other)
      {
      const XPersistentScriptValueInternal *otherInternal = (const XPersistentScriptValueInternal*)other;
      internal->_object = otherInternal->_object;
      }

    return internal;
    }

  static void term(XPersistentScriptValue *o)
    {
    XPersistentScriptValueInternal *internal = (XPersistentScriptValueInternal*)o;
    (void)internal;
    internal->~XPersistentScriptValueInternal();
    }

  static const XPersistentScriptValueInternal *val(const XPersistentScriptValue *o)
    {
    const XPersistentScriptValueInternal *internal = (XPersistentScriptValueInternal*)o;
    return internal;
    }

  static XPersistentScriptValueInternal *val(XPersistentScriptValue *o)
    {
    XPersistentScriptValueInternal *internal = (XPersistentScriptValueInternal*)o;
    return internal;
    }

  mutable v8::Persistent<v8::Value> _object;
  };
xCompileTimeAssert(sizeof(XPersistentScriptValue) == sizeof(XPersistentScriptValueInternal));

XScriptValue::XScriptValue()
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_Null();
#else
  XScriptValueInternal *internal = XScriptValueInternal::init(this);
  internal->_object = v8::Null();
#endif
  }

XScriptValue::XScriptValue(bool x)
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_NewBoolean(x);
#else
  XScriptValueInternal *internal = XScriptValueInternal::init(this);
  internal->_object = v8::Boolean::New(x);
#endif
  }

XScriptValue::XScriptValue(xuint32 x)
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_NewInteger(x);
#else
  XScriptValueInternal *internal = XScriptValueInternal::init(this);
  internal->_object = v8::Integer::New(x);
#endif
  }

XScriptValue::XScriptValue(xint32 x)
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_NewInteger(x);
#else
  XScriptValueInternal *internal = XScriptValueInternal::init(this);
  internal->_object = v8::Integer::New(x);
#endif
  }

XScriptValue::XScriptValue(xuint64 x)
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_NewInteger(x);
#else
  XScriptValueInternal *internal = XScriptValueInternal::init(this);
  internal->_object = v8::Number::New(x);
#endif
  }

XScriptValue::XScriptValue(xint64 x)
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_NewInteger(x);
#else
  XScriptValueInternal *internal = XScriptValueInternal::init(this);
  internal->_object = v8::Number::New(x);
#endif
  }

XScriptValue::XScriptValue(double x)
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_NewDouble(x);
#else
  XScriptValueInternal *internal = XScriptValueInternal::init(this);
  internal->_object = v8::Number::New(x);
#endif
  }

XScriptValue::XScriptValue(float x)
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_NewDouble(x);
#else
  XScriptValueInternal *internal = XScriptValueInternal::init(this);
  internal->_object = v8::Number::New(x);
#endif
  }

XScriptValue::XScriptValue(const QString &str)
  {
#ifdef X_DART
  if(str.length())
    {
    getDartHandle(_object) = Dart_NewString16((const uint16_t*)str.constData(), str.length());
    }
  else
    {
    getDartHandle(_object) = Dart_NewString("");
    }
#else
  v8::Locker l;
  XScriptValueInternal *internal = XScriptValueInternal::init(this);

  if(str.length())
    {
    internal->_object = v8::String::New((const uint16_t*)str.constData(), str.length());
    }
  else
    {
    internal->_object = v8::String::New("");
    }
#endif
  }

XScriptValue::XScriptValue(const XScriptObject &obj)
  {
#ifdef X_DART
  getDartHandle(_object) = getDartInternal(obj);
#else
  XScriptValueInternal *internal = XScriptValueInternal::init(this);
  internal->_object = getV8Internal(obj);
#endif
  }

XScriptValue::XScriptValue(const QVariant& val)
  {
  XScriptValueInternal::init(this);

  switch(val.type())
    {
    case QVariant::String:
      *this = XScriptValue(val.toString());
      break;
    case QVariant::Bool:
      *this = XScriptValue(val.toBool());
      break;
    case QVariant::Int:
      *this = XScriptValue(val.toInt());
      break;
    case QVariant::Double:
      *this = XScriptValue(val.toDouble());
      break;
    case QVariant::List:
      {
#ifdef X_DART
    xAssertFail();
#else
        QVariantList list = val.toList();
        v8::Local<v8::Array> array = v8::Array::New(list.length());
        for(int i = 0, s = list.size(); i < s; ++i)
        {
          array->Set((uint32_t)i, getV8Internal(XScriptValue(list[i])));
        }
        *this = fromHandle(array);
#endif
      }
      break;
    case QVariant::StringList:
      {
#ifdef X_DART
    xAssertFail();
#else
        QStringList list = val.toStringList();
        v8::Local<v8::Array> array = v8::Array::New(list.length());
        for(int i = 0, s = list.size(); i < s; ++i)
        {
          array->Set((uint32_t)i, getV8Internal(XScriptValue(list[i])));
        }
        *this = fromHandle(array);
#endif
      }
      break;
    case QVariant::Map:
      {
#ifdef X_DART
    xAssertFail();
#else
      QVariantMap map = val.toMap();
      v8::Local<v8::Object> obj = v8::Object::New();
      QVariantMap::const_iterator it = map.begin();
      QVariantMap::const_iterator end = map.end();
      for(; it != end; ++it)
        {
        QString name = it.key();
        QVariant val = it.value();
        obj->Set(getV8Internal(XScriptConvert::to(name)), getV8Internal(XScriptValue(val)));
        }
      *this = fromHandle(obj);
#endif
      }
      break;

    default:
      {
      const XInterfaceBase* interface = findInterface(val.userType());
      xAssert(interface);
      if(interface)
        {
        *this = interface->copyValue(val);
        }
      break;
      }
    }
  }

XScriptValue::XScriptValue(void* val)
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_NewInteger((int64_t)val);
#else
  XScriptValueInternal *internal = XScriptValueInternal::init(this);
  internal->_object = v8::External::New(val);
#endif
  }


XScriptValue XScriptValue::newArray()
  {
  XScriptValue obj;
#ifdef X_DART
  getDartHandle(obj._object) = Dart_NewList(0);
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(&obj);
  internal->_object = v8::Array::New();
#endif
  return obj;
  }

XScriptValue XScriptValue::empty()
  {
  XScriptValue obj;
  obj.clear();
  return obj;
  }

XScriptValue::~XScriptValue()
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_Null();
#else
  XScriptValueInternal::term(this);
#endif
  }

XScriptValue::XScriptValue(const XScriptValue &other)
  {
#ifdef X_DART
  getDartHandle(_object) = getDartHandle(other);
#else
  XScriptValueInternal::init(this, &other);
#endif
  }

XScriptValue& XScriptValue::operator=(const XScriptValue &other)
  {
#ifdef X_DART
  getDartHandle(_object) = getDartHandle(other);
#else
  XScriptValueInternal::term(this);
  XScriptValueInternal::init(this, &other);
#endif
  return *this;
  }

void XScriptValue::clear()
  {
#ifdef X_DART
  getDartHandle(_object) = 0;
#else
  XScriptValueInternal *internal = XScriptValueInternal::val(this);
  internal->_object.Clear();
#endif
  }

bool XScriptValue::isValid() const
  {
#ifdef X_DART
  Dart_Handle h = getDartInternal(*this);
  return h != 0 && !Dart_IsNull(h);
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  return !internal->_object.IsEmpty() &&
      !internal->_object->IsNull() &&
      !internal->_object->IsUndefined();
#endif
  }

bool XScriptValue::isObject() const
  {
#ifdef X_DART
  // everything is an object!
  // probably not so useful.
  return true;
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  return internal->_object->IsObject();
#endif
  }
bool XScriptValue::isBoolean() const
  {
#ifdef X_DART
  return Dart_IsBoolean(getDartInternal(*this));
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  return internal->_object->IsBoolean();
#endif
  }

bool XScriptValue::isNumber() const
  {
#ifdef X_DART
  return Dart_IsNumber(getDartInternal(*this));
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  return internal->_object->IsNumber();
#endif
  }

bool XScriptValue::isString() const
  {
#ifdef X_DART
  return Dart_IsString(getDartInternal(*this));
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  return internal->_object->IsString();
#endif
  }

bool XScriptValue::isInteger() const
  {
#ifdef X_DART
  return Dart_IsInteger(getDartInternal(*this));
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  return internal->_object->IsInt32() || internal->_object->IsUint32();
#endif
  }

bool XScriptValue::isArray() const
  {
#ifdef X_DART
  return  Dart_IsList(getDartInternal(*this));
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  return internal->_object->IsArray();
#endif
  }

xsize XScriptValue::length() const
  {
#ifdef X_DART
  intptr_t len = 0;
  Dart_ListLength(getDartInternal(*this), &len);
  return len;
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  v8::Handle<v8::Array> arr = internal->_object.As<v8::Array>();
  return arr->Length();
#endif
  }

XScriptValue XScriptValue::at(xsize id) const
  {
#ifdef X_DART
  return fromHandle(Dart_ListGetAt(getDartInternal(*this), id));
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  v8::Handle<v8::Array> arr = internal->_object.As<v8::Array>();
  return fromHandle(arr->Get(id));
#endif
  }

void XScriptValue::set(xsize id, const XScriptValue &val)
  {
#ifdef X_DART
  Dart_ListSetAt(getDartHandle(_object), id, getDartInternal(val));
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  v8::Handle<v8::Array> arr = internal->_object.As<v8::Array>();
  arr->Set(id, getV8Internal(val));
#endif
  }

void *XScriptValue::toExternal() const
  {
#ifdef X_DART
  return (void*)toInteger();
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  return internal->_object.As<v8::External>()->Value();
#endif
  }

double XScriptValue::toNumber() const
  {
#ifdef X_DART
  double dbl = 0.0f;
  Dart_DoubleValue(getDartInternal(*this), &dbl);
  return dbl;
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  return internal->_object.As<v8::Number>()->Value();
#endif
  }

xint64 XScriptValue::toInteger() const
  {
#ifdef X_DART
  xint64 igr = 0;
  Dart_IntegerToInt64(getDartInternal(*this), &igr);
  return igr;
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  if(internal->_object->IsNumber())
    {
    return internal->_object.As<v8::Number>()->Value();
    }
  return 0;
#endif
  }

bool XScriptValue::toBoolean() const
  {
#ifdef X_DART
  bool b = 0;
  Dart_BooleanValue(getDartInternal(*this), &b);
  return b;
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  if(internal->_object->IsBoolean())
    {
    return internal->_object->ToBoolean()->Value();
    }
  return false;
#endif
  }

QString XScriptValue::toString() const
  {
#ifdef X_DART
  intptr_t len;
  QString val;
  Dart_Handle str = getDartInternal(*this);
  if(!Dart_IsString(str))
    {
    str = Dart_ToString(str);
    }

  Dart_StringLength(str, &len);
  if(len > 0)
  {
    val.resize(len);
    Dart_StringGet16(str, (uint16_t*)val.data(), &len);
    }

  return val;
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
  if(!internal->_object.IsEmpty())
    {
    v8::Handle<v8::String> str = internal->_object->ToString();
    if(!str.IsEmpty())
      {
      QString strOut;
      strOut.resize(str->Length());

      xCompileTimeAssert(sizeof(QChar) == sizeof(uint16_t));
      str->Write((uint16_t*)strOut.data(), 0, strOut.length());
      return strOut;
      }
    else
      {
      //xAssertFail();
      //v8::String::AsciiValue exception_str(internal->_object);
      //return *exception_str;
      }
    }
  return QString();
#endif
  }

QVariant XScriptValue::toVariant(int typeHint) const
  {
#ifndef X_DART
  const XScriptValueInternal *internal = XScriptValueInternal::val(this);
#endif
  if(typeHint == QVariant::Bool || isBoolean())
    {
    return toBoolean();
    }
  else if(typeHint == QVariant::Int || isInteger())
    {
    return toInteger();
    }
  else if(typeHint == QVariant::Double || isNumber())
    {
    return toNumber();
    }
  else if(typeHint == QVariant::String || isString())
    {
    return toString();
    }
  else if (typeHint == qMetaTypeId<XScriptObject>())
    {
    return QVariant::fromValue(XScriptConvert::from<XScriptObject>(fromHandle(
#ifdef X_DART
getDartInternal(*this)
#else
internal->_object
#endif
      )));
    }
  else if (typeHint == qMetaTypeId<XScriptFunction>())
    {
    return QVariant::fromValue(XScriptConvert::from<XScriptFunction>(fromHandle(
#ifdef X_DART
      getDartInternal(*this)
#else
      internal->_object
#endif
      )));
    }
  else if(isArray())
    {
    QVariantList list;

    for(xsize i = 0, s = length(); i < s; ++i)
      {
      list << at(i).toVariant(QVariant::Invalid);
      }
    return list;
    }

  if(isObject())
    {
    XScriptObject object(*this);
    XInterfaceBase* interface = object.getInterface();
    if(interface)
      {
      return interface->toVariant(*this, typeHint);
      }
    }

  XInterfaceBase *interface = typeHint != QVariant::Invalid ? findInterface(typeHint) : 0;
  if(interface)
    {
    return interface->toVariant(XScriptValue(), typeHint);
    }

  if(typeHint == QVariant::Map || isObject())
    {
    QVariantMap map;
#ifdef X_DART
    xAssertFail();
#else
    if(internal->_object->IsObject())
      {
      v8::Handle<v8::Object> obj = internal->_object.As<v8::Object>();
      v8::Handle<v8::Array> propertyNames = obj->GetOwnPropertyNames();
      for(uint32_t i = 0, s = propertyNames->Length(); i < s; ++i)
        {
        v8::Handle<v8::Value> key = propertyNames->Get(i);
        v8::Handle<v8::Value> value = obj->Get(key);

        map.insert(XScriptConvert::from<QString>(fromHandle(key)),
                   fromHandle(value).toVariant(QVariant::Invalid));
        }
      }
#endif
    return map;
    }

#ifdef X_DART
  xAssert(Dart_IsNull(getDartInternal(*this)));
#else
  xAssert(internal->_object->IsNull());
#endif
  return QVariant();
  }

XScriptValue fromHandle(v8::Handle<v8::Value> v)
  {
  XScriptValue o;
  XScriptValueInternal *internal = XScriptValueInternal::val(&o);
  internal->_object = v;
  return o;
  }

XScriptValue fromHandle(Dart_Handle h)
  {
  XScriptValue v;
  getDartHandle(v) = h;
  return v;
  }

v8::Handle<v8::Value> getV8Internal(const XScriptValue &o)
  {
  const XScriptValueInternal *internal = XScriptValueInternal::val(&o);
  return internal->_object;
  }

Dart_Handle getDartInternal(const XScriptValue& v)
  {
  return getDartHandle(v);
  }

v8::Handle<v8::Value> *getV8Internal(const XScriptValue *o)
  {
  return (v8::Handle<v8::Value> *)o;
  }

Dart_Handle *getDartInternal(const XScriptValue *o)
  {
  return (Dart_Handle*)o;
  }

XPersistentScriptValue::XPersistentScriptValue()
  {
#ifdef X_DART
  getDartHandle(_object) = 0;
#else
  XPersistentScriptValueInternal::init(this);
#endif
  }

XPersistentScriptValue::XPersistentScriptValue(const XScriptValue &val)
  {
#ifdef X_DART
  getDartHandle(_object) = Dart_NewPersistentHandle(getDartInternal(val));
#else
  const XPersistentScriptValueInternal *internal = XPersistentScriptValueInternal::init(this);

  const XScriptValueInternal *other = XScriptValueInternal::val(&val);
  internal->_object = v8::Persistent<v8::Value>::New(other->_object);
#endif
  }

XScriptValue XPersistentScriptValue::asValue() const
  {
  XScriptValue val;
#ifdef X_DART
  getDartHandle(val._object) = getDartHandle(*this);
#else
  const XScriptValueInternal *internal = XScriptValueInternal::val(&val);
  const XPersistentScriptValueInternal *other = XPersistentScriptValueInternal::val(this);

  internal->_object = other->_object;
#endif
  return val;
  }

void XPersistentScriptValue::makeWeak(void *data, WeakDtor cb)
  {
#ifdef X_DART
  Dart_Handle oldhandle = getDartHandle(_object);
  getDartHandle(_object) = Dart_NewPrologueWeakPersistentHandle(
                             oldhandle,
                             data,
                             (Dart_WeakPersistentHandleFinalizer)cb);
#else
  const XPersistentScriptValueInternal *internal = XPersistentScriptValueInternal::val(this);
  internal->_object.MakeWeak(data, (v8::WeakReferenceCallback)cb);
#endif
  }

void XPersistentScriptValue::dispose()
  {
#ifdef X_DART
  Dart_DeletePersistentHandle(getDartHandle(_object));
#else
  const XPersistentScriptValueInternal *internal = XPersistentScriptValueInternal::val(this);
  internal->_object.Dispose();
  internal->_object.Clear();
#endif
  }
