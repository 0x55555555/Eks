#include "XScriptObject.h"
#include "XScriptValue.h"
#include "XScriptEngine.h"
#include "XAssert"
#include "XScriptInterface.h"

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

void *Object::internalField(ObjectInternalField idx) const
  {
  return currentInterface()->internalField(this, idx);
  }


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
