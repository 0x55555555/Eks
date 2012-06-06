#include "XScriptObject.h"
#include "XScriptValueV8Internals.h"
#include "XScriptValueDartInternals.h"
#include "XAssert"
#include "XInterface.h"

struct XScriptObjectInternal
  {
  static XScriptObjectInternal *init(XScriptObject *o, const XScriptObject *other=0)
    {
    XScriptObjectInternal *internal = (XScriptObjectInternal*)o;
    new(internal) XScriptObjectInternal;

    if(other)
      {
      const XScriptObjectInternal *otherInternal = (const XScriptObjectInternal*)other;
      internal->_object = otherInternal->_object;
      }

    return internal;
    }

  static void term(XScriptObject *o)
    {
    XScriptObjectInternal *internal = (XScriptObjectInternal*)o;
    (void)internal;
    internal->~XScriptObjectInternal();
    }

  static const XScriptObjectInternal *val(const XScriptObject *o)
    {
    XScriptObjectInternal *internal = (XScriptObjectInternal*)o;
    return internal;
    }

  static XScriptObjectInternal *val(XScriptObject *o)
    {
    XScriptObjectInternal *internal = (XScriptObjectInternal*)o;
    return internal;
    }

  mutable v8::Handle<v8::Object> _object;
  };
xCompileTimeAssert(sizeof(XScriptObject) == sizeof(XScriptObjectInternal));

Dart_Handle& getDartHandle(const XScriptObject &obj)
  {
  struct Internal
    {
    Dart_Handle ptr;
    };

  return ((Internal*)(&obj))->ptr;
  }

XScriptObject::XScriptObject()
  {
#ifdef X_DART
  getDartHandle(_object) = 0;
#else
  XScriptObjectInternal::init(this);
#endif
  }

XScriptObject::~XScriptObject()
  {
#ifdef X_DART
  getDartHandle(_object) = 0;
#else
  XScriptObjectInternal::term(this);
#endif
  }

XScriptObject::XScriptObject(const XScriptObject &other)
  {
#ifdef X_DART
  getDartHandle(_object) = getDartInternal(other);
#else
  XScriptObjectInternal::init(this, &other);
#endif
  }

XScriptObject::XScriptObject(const XScriptValue &other)
  {
#ifdef X_DART
  getDartHandle(_object) = getDartInternal(other);
#else
  const v8::Handle<v8::Value> otherInternal = getV8Internal(other);
  XScriptObjectInternal *internal = XScriptObjectInternal::init(this);
  if(getV8Internal(other)->IsObject())
    {
    internal->_object = v8::Handle<v8::Object>(v8::Object::Cast(*otherInternal));
    }
#endif
  }

XScriptObject::XScriptObject(const XScriptFunction &other)
  {
#ifdef X_DART
  getDartHandle(_object) = getDartInternal(other);
#else
  v8::Handle<v8::Function> otherInternal = getV8Internal(other);
  XScriptObjectInternal *internal = XScriptObjectInternal::init(this);
  internal->_object = otherInternal;
#endif
  }

XScriptObject& XScriptObject::operator=(const XScriptObject &other)
  {
#ifdef X_DART
  getDartHandle(_object) = getDartInternal(other);
#else
  XScriptObjectInternal::term(this);
  XScriptObjectInternal::init(this, &other);
#endif
  return *this;
  }

xsize XScriptObject::internalFieldCount() const
  {
#ifdef X_DART
  return 2;
#else
  const XScriptObjectInternal *internal = XScriptObjectInternal::val(this);
  return internal->_object->InternalFieldCount();
#endif
  }

void *XScriptObject::internalField(xsize idx) const
  {
#ifdef X_DART
  intptr_t val = 0;
  return Dart_GetNativeInstanceField(getDartInternal(*this),
                                     idx,
                                     &val);
  return (void*)val;
#else
  const XScriptObjectInternal *internal = XScriptObjectInternal::val(this);
  return internal->_object->GetPointerFromInternalField(idx);
#endif
  }

XInterfaceBase *XScriptObject::getInterface() const
  {
  typedef XScript::ClassCreator_InternalFields_Base<void> IF;
  xsize tid = 0;
  XScriptValue proto(*this);
  while(!tid && proto.isObject())
    {
    XScriptObject const &obj(proto);
    tid = (obj.internalFieldCount() != (xsize)IF::Count)
      ? 0
      : (xsize)obj.internalField(IF::TypeIDIndex);

    proto = obj.getPrototype();
    }

  return tid != 0 ? findInterface(tid) : 0;
  }

XScriptValue XScriptObject::getPrototype() const
  {
#ifdef X_DART
  return XScriptValue();
#else
  const XScriptObjectInternal *internal = XScriptObjectInternal::val(this);
  return fromHandle(internal->_object->GetPrototype());
#endif
  }

void XScriptObject::setPrototype(XScriptValue proto)
  {
#ifdef X_DART
  xAssertFail();
#else
  const XScriptObjectInternal *internal = XScriptObjectInternal::val(this);
  internal->_object->SetPrototype(getV8Internal(proto));
#endif
  }

XScriptValue XScriptObject::get(const QString &n) const
  {
  XScriptValue key = XScriptConvert::to(n);

#ifdef X_DART
  return fromHandle(Dart_GetField(getDartInternal(*this), getDartInternal(key)));
#else
  const XScriptObjectInternal *internal = XScriptObjectInternal::val(this);
  return fromHandle(internal->_object->Get(getV8Internal(key)));
#endif
  }

void XScriptObject::set(const QString &n, const XScriptValue &v)
  {
  XScriptValue key = XScriptConvert::to(n);

#ifdef X_DART
  Dart_SetField(getDartHandle(_object), getDartInternal(key), getDartInternal(v));
#else
  const XScriptObjectInternal *internal = XScriptObjectInternal::val(this);
  internal->_object->Set(getV8Internal(key), getV8Internal(v));
#endif
  }

bool XScriptObject::isValid() const
  {
#ifdef X_DART
  return _object != 0 && !Dart_IsNull(getDartInternal(*this));
#else
  const XScriptObjectInternal *internal = XScriptObjectInternal::val(this);
  return !internal->_object.IsEmpty() && internal->_object->IsObject();
#endif
  }

XScriptObject XScriptObject::newObject()
  {
  XScriptObject obj;

#ifdef X_DART
  xAssertFail();
#else
  const XScriptObjectInternal *internal = XScriptObjectInternal::val(&obj);
  internal->_object = v8::Object::New();
#endif

  return obj;
  }

v8::Handle<v8::Object> getV8Internal(const XScriptObject &o)
  {
  const XScriptObjectInternal *internal = XScriptObjectInternal::val(&o);
  return internal->_object;
  }

XScriptObject fromObjectHandle(v8::Handle<v8::Object> v)
  {
  XScriptObject o;
  XScriptObjectInternal *internal = XScriptObjectInternal::val(&o);
  internal->_object = v;
  return o;
  }

XScriptObject fromObjectHandle(Dart_Handle v)
  {
  XScriptObject o;
  getDartHandle(o) = v;
  return o;
  }

Dart_Handle getDartInternal(const XScriptObject &o)
  {
  return getDartHandle(o);
  }

