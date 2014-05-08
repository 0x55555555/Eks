#include "XScriptValue.h"
#include "XScriptObject.h"
#include "Containers/XUnorderedMap.h"
#include "QStringList"
#include "XConvertFromScript.h"
#include "XConvertToScript.h"
#include "XInterface.h"
#include "Utilities/XAssert.h"

namespace XScript
{

Value::Value()
  {
  currentInterface()->newValue(this);
  }

Value::Value(bool x)
  {
  currentInterface()->newValue(this, x);
  }

Value::Value(xuint32 x)
  {
  currentInterface()->newValue(this, x);
  }

Value::Value(xint32 x)
  {
  currentInterface()->newValue(this, x);
  }

Value::Value(xuint64 x)
  {
  currentInterface()->newValue(this, x);
  }

Value::Value(xint64 x)
  {
  currentInterface()->newValue(this, x);
  }

Value::Value(double x)
  {
  currentInterface()->newValue(this, x);
  }

Value::Value(float x)
  {
  currentInterface()->newValue(this, x);
  }

Value::Value(const Eks::String &str)
  {
  currentInterface()->newValue(this, str);
  }

Value::Value(const Object &obj)
  {
  currentInterface()->newValue(this, &obj);
  }

Value::Value(const Function &fn)
  {
  currentInterface()->newValue(this, &fn);
  }

Value::Value(const QVariant& val)
  {
  switch(val.type())
    {
    case QVariant::String:
      currentInterface()->newValue(this, val.toString().toUtf8().data());
      break;
    case QVariant::Bool:
      currentInterface()->newValue(this, val.toBool());
      break;
    case QVariant::Int:
      currentInterface()->newValue(this, val.toInt());
      break;
    case QVariant::Double:
    currentInterface()->newValue(this, val.toDouble());
      break;
    case QVariant::List:
      {
      currentInterface()->newValue(this, val.toList());
      }
      break;
    case QVariant::StringList:
      {
      currentInterface()->newValue(this, val.toStringList());
      }
      break;
    case QVariant::Map:
      {
      currentInterface()->newValue(this, val.toMap());
      }
      break;

    default:
      {
      const InterfaceBase* interface = findInterface(val.userType());
      xAssert(interface);
      if(interface)
        {
        Value cpy = interface->copyValue(val);
        currentInterface()->newValue(this, &cpy);
        }
      break;
      }
    }
  }

Value::Value(void* val)
  {
  currentInterface()->newValue(this, val);
  }


Value Value::newArray()
  {
  Value val;
  currentInterface()->newArray(&val);
  return val;
  }

Value Value::newEmpty()
  {
  Value val;
  currentInterface()->newEmpty(&val);
  return val;
  }

Value::~Value()
  {
  currentInterface()->destroy(this);
  }

Value::Value(const Value &other)
  {
  currentInterface()->newValue(this, &other);
  }

Value& Value::operator=(const Value &other)
  {
  currentInterface()->destroy(this);
  currentInterface()->newValue(this, &other);
  return *this;
  }

void Value::clear()
  {
  currentInterface()->destroy(this);
  currentInterface()->newEmpty(this);
  }

bool Value::isValid() const
  {
  return currentInterface()->isValid(this);
  }

bool Value::isObject() const
  {
  return currentInterface()->isObject(this);
  }

bool Value::isBoolean() const
  {
  return currentInterface()->isBoolean(this);
  }

bool Value::isNumber() const
  {
  return currentInterface()->isNumber(this);
  }

bool Value::isString() const
  {
  return currentInterface()->isString(this);
  }

bool Value::isInteger() const
  {
  return currentInterface()->isInteger(this);
  }

bool Value::isArray() const
  {
  return currentInterface()->isArray(this);
  }

xsize Value::length() const
  {
  return currentInterface()->length(this);
  }

Value Value::at(xsize id) const
  {
  Value out;
  currentInterface()->at(&out, this, id);

  return out;
  }

void Value::set(xsize id, const Value &val)
  {
  return currentInterface()->set(this, id, &val);
  }

void *Value::toExternal() const
  {
  return currentInterface()->toExternal(this);
  }

double Value::toNumber() const
  {
  return currentInterface()->toNumber(this);
  }

xint64 Value::toInteger() const
  {
  return currentInterface()->toInteger(this);
  }

bool Value::toBoolean() const
  {
  return currentInterface()->toBoolean(this);
  }

Eks::String Value::toString() const
  {
  Eks::String out;
  currentInterface()->toString(&out, this);
  return out;
  }

QVariant Value::toVariant(int typeHint) const
  {
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
    return QString::fromUtf8(toString().data());
    }
  else if (typeHint == qMetaTypeId<Object>())
    {
    return QVariant::fromValue(Object(*this));
    }
  else if (typeHint == qMetaTypeId<Function>())
    {
    return QVariant::fromValue(Function(*this));
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
    Object object(*this);
    InterfaceBase* interface = object.getInterface();
    if(interface)
      {
      return interface->toVariant(*this, typeHint);
      }
    }

  InterfaceBase *interface = typeHint != QVariant::Invalid ? findInterface(typeHint) : 0;
  if(interface)
    {
    return interface->toVariant(Value(), typeHint);
    }

  if(typeHint == QVariant::Map || isObject())
    {
    QVariantMap map;
    currentInterface()->toMap(&map, this);
    return map;
    }

  return QVariant();
  }

PersistentValue::PersistentValue()
  {
  currentInterface()->newPersistentValue(this);
  }

PersistentValue::PersistentValue(const Value &val)
  {
  currentInterface()->newPersistentValue(this, val);
  }

Value PersistentValue::asValue() const
  {
  Value v;
  currentInterface()->asValue(&v, this);

  return v;
  }

void PersistentValue::makeWeak(void *data, WeakDtor cb)
  {
  currentInterface()->makeWeak(this, data, cb);
  }

void PersistentValue::dispose()
  {
  currentInterface()->dispose(this);
  }

Callback::Callback(EngineInterface *ifc, const Function &fn)
    : _engineInterface(ifc), _function(fn)
  {
  }

Callback::Callback(EngineInterface *ifc, const Object& obj, const Function &fn)
  : _engineInterface(ifc), _object(obj), _function(fn)
  {
  }

Callback::~Callback()
  {
  _object.dispose();
  _function.dispose();
  }

bool Callback::isValid() const
  {
  Function fn(_function.asValue());
  return fn.isValid();
  }

void Callback::call(Value *result, xsize argCount, Value *args, bool *error, QString *errorOut)
  {
  xAssert(currentInterface() == _engineInterface);
  Object obj(_object.asValue());
  Function fn(_object.asValue());

  if(fn.isValid())
    {
    if(obj.isValid())
      {
      fn.callWithTryCatch(result, obj, argCount, args, error, errorOut);
      }
    else
      {
      fn.callWithTryCatch(result, Object(), argCount, args, error, errorOut);
      }
    }
  }

CallbackScope::CallbackScope(Callback& callback) : EngineScope(callback.engineInterface())
  {
  }

}
