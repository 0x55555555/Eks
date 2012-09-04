#include "XScriptObject.h"
#include "XScriptEngine.h"
#include "XAssert"
#include "XInterface.h"

namespace XScript
{

Object::Object()
  {
  currentInterface()->newObject(this);
  }

Object::Object(const Object &other)
  {
  currentInterface()->newObject(this, &other);
  }

Object::Object(const Value &other)
  {
  currentInterface()->newObject(this, &other);
  }

/*

Object::Object(const Function &other)
  {
#ifdef X_DART
  Dart_Handle& thsHndl = getDartHandle(_object);
  Dart_Handle othHndl = getDartInternal(other);

  thsHndl = othHndl;
#else
  v8::Handle<v8::Function> otherInternal = getV8Internal(other);
  ObjectInternal *internal = ObjectInternal::init(this);
  internal->_object = otherInternal;
#endif
  }*/

Object::~Object()
  {
  currentInterface()->destroy(this);
  }

Object& Object::operator=(const Object &other)
  {
  currentInterface()->destroy(this);
  currentInterface()->newObject(this, &other);

  return *this;
  }

//xsize Object::internalFieldCount() const
//  {
//#ifdef X_DART
//  return kNumEventHandlerFields;
//#else
//  const ObjectInternal *internal = ObjectInternal::val(this);
//  return internal->_object->InternalFieldCount();
//#endif
//  }

void *Object::internalField(ObjectInternalField idx) const
  {
  return currentInterface()->internalField(this, idx);
  }

/*
InterfaceBase *Object::getInterface() const
  {
  xsize tid = 0;
  Value proto(*this);
  while(!tid && proto.isObject())
    {
    Object const &obj(proto);
    tid = obj.internalField(ObjectInternalField::TypeId);

    proto = obj.getPrototype();
    }

  return findInterface(tid);
  }*/

#if 0
Value Object::getPrototype() const
  {
#ifdef X_DART
  return XScriptValue();
#else
  const ObjectInternal *internal = ObjectInternal::val(this);
  return fromHandle(internal->_object->GetPrototype());
#endif
  }

void Object::setPrototype(XScriptValue proto)
  {
#ifdef X_DART
  xAssertFail();
#else
  const ObjectInternal *internal = ObjectInternal::val(this);
  internal->_object->SetPrototype(getV8Internal(proto));
#endif
  }
#endif

Value Object::get(const QString &n) const
  {
  Value ret;
  currentInterface()->get(&ret, this, n);

  return ret;
  }

void Object::set(const QString &n, const Value &v)
  {
  currentInterface()->set(this, n, &v);
  }

bool Object::isValid() const
  {
  return currentInterface()->isValid(this);
  }

Object Object::newMap()
  {
  Object obj;
  currentInterface()->newMap(&obj);
  return obj;
  }

}
