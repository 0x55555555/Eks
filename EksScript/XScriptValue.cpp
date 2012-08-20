#include "XScriptValue.h"
#include "XScriptObject.h"
#include "XUnorderedMap"
#include "QStringList"
#include "XConvertFromScript.h"
#include "XConvertToScript.h"
#include "XInterface.h"
#include "XAssert"

XScriptValue::XScriptValue()
  {
  XScript::currentInterface()->newValue(this);
  }

XScriptValue::XScriptValue(bool x)
  {
  XScript::currentInterface()->newValue(this, x);
  }

XScriptValue::XScriptValue(xuint32 x)
  {
  XScript::currentInterface()->newValue(this, x);
  }

XScriptValue::XScriptValue(xint32 x)
  {
  XScript::currentInterface()->newValue(this, x);
  }

XScriptValue::XScriptValue(xuint64 x)
  {
  XScript::currentInterface()->newValue(this, x);
  }

XScriptValue::XScriptValue(xint64 x)
  {
  XScript::currentInterface()->newValue(this, x);
  }

XScriptValue::XScriptValue(double x)
  {
  XScript::currentInterface()->newValue(this, x);
  }

XScriptValue::XScriptValue(float x)
  {
  XScript::currentInterface()->newValue(this, x);
  }

XScriptValue::XScriptValue(const QString &str)
  {
  XScript::currentInterface()->newValue(this, str);
  }

XScriptValue::XScriptValue(const XScriptObject &obj)
  {
  XScript::currentInterface()->newValue(this, obj);
  }

XScriptValue::XScriptValue(const XScriptFunction &fn)
  {
  XScript::currentInterface()->newValue(this, fn);
  }

XScriptValue::XScriptValue(const QVariant& val)
  {
  switch(val.type())
    {
    case QVariant::String:
      XScript::currentInterface()->newValue(this, val.toString());
      break;
    case QVariant::Bool:
      XScript::currentInterface()->newValue(this, val.toBool());
      break;
    case QVariant::Int:
      XScript::currentInterface()->newValue(this, val.toInt());
      break;
    case QVariant::Double:
    XScript::currentInterface()->newValue(this, val.toDouble());
      break;
    case QVariant::List:
      {
      XScript::currentInterface()->newValue(this, val.toList());
      }
      break;
    case QVariant::StringList:
      {
      XScript::currentInterface()->newValue(this, val.toStringList());
      }
      break;
    case QVariant::Map:
      {
      XScript::currentInterface()->newValue(this, val.toMap());
      }
      break;

    default:
      {
      const XInterfaceBase* interface = findInterface(val.userType());
      xAssert(interface);
      if(interface)
        {
        XScriptValue cpy = interface->copyValue(val);
        XScript::currentInterface()->newValue(this, &cpy);
        }
      break;
      }
    }
  }

XScriptValue::XScriptValue(void* val)
  {
  XScript::currentInterface()->newValue(this, val);
  }


XScriptValue XScriptValue::newArray()
  {
  XScriptValue val;
  XScript::currentInterface()->newArray(&val);
  return val;
  }

XScriptValue XScriptValue::newEmpty()
  {
  XScriptValue val;
  XScript::currentInterface()->newEmpty(&val);
  return val;
  }

XScriptValue::~XScriptValue()
  {
  XScript::currentInterface()->destroy(this);
  }

XScriptValue::XScriptValue(const XScriptValue &other)
  {
  XScript::currentInterface()->newValue(this, &other);
  }

XScriptValue& XScriptValue::operator=(const XScriptValue &other)
  {
  XScript::currentInterface()->destroy(this);
  XScript::currentInterface()->newValue(this, &other);
  return *this;
  }

void XScriptValue::clear()
  {
  XScript::currentInterface()->destroy(this);
  XScript::currentInterface()->newEmpty(this);
  }

bool XScriptValue::isValid() const
  {
  return XScript::currentInterface()->isValid(this);
  }

bool XScriptValue::isObject() const
  {
  return XScript::currentInterface()->isObject(this);
  }

bool XScriptValue::isBoolean() const
  {
  return XScript::currentInterface()->isBoolean(this);
  }

bool XScriptValue::isNumber() const
  {
  return XScript::currentInterface()->isNumber(this);
  }

bool XScriptValue::isString() const
  {
  return XScript::currentInterface()->isString(this);
  }

bool XScriptValue::isInteger() const
  {
  return XScript::currentInterface()->isInteger(this);
  }

bool XScriptValue::isArray() const
  {
  return XScript::currentInterface()->isArray(this);
  }

xsize XScriptValue::length() const
  {
  return XScript::currentInterface()->length(this);
  }

XScriptValue XScriptValue::at(xsize id) const
  {
  XScriptValue out;
  XScript::currentInterface()->at(&out, this, id);

  return out;
  }

void XScriptValue::set(xsize id, const XScriptValue &val)
  {
  return XScript::currentInterface()->set(this, id, val);
  }

void *XScriptValue::toExternal() const
  {
  return XScript::currentInterface()->toExternal(this);
  }

double XScriptValue::toNumber() const
  {
  return XScript::currentInterface()->toNumber(this);
  }

xint64 XScriptValue::toInteger() const
  {
  return XScript::currentInterface()->toInteger(this);
  }

bool XScriptValue::toBoolean() const
  {
  return XScript::currentInterface()->toBoolean(this);
  }

QString XScriptValue::toString() const
  {
  QString out;
  XScript::currentInterface()->toString(&out, this);
  return out;
  }

QVariant XScriptValue::toVariant(int typeHint) const
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
    return toString();
    }
  else if (typeHint == qMetaTypeId<XScriptObject>())
    {
    return QVariant::fromValue(XScriptObject(*this));
    }
  else if (typeHint == qMetaTypeId<XScriptFunction>())
    {
    return QVariant::fromValue(XScriptFunction(*this));
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
    XScript::currentInterface()->toMap(&map, this);
    return map;
    }

  return QVariant();
  }

XPersistentScriptValue::XPersistentScriptValue()
  {
  XScript::currentInterface()->newPersistentValue(this);
  }

XPersistentScriptValue::XPersistentScriptValue(const XScriptValue &val)
  {
  XScript::currentInterface()->newPersistentValue(this, val);
  }

XScriptValue XPersistentScriptValue::asValue() const
  {
  XScriptValue v;
  XScript::currentInterface()->asValue(&v, this);

  return v;
  }

void XPersistentScriptValue::makeWeak(void *data, WeakDtor cb)
  {
  XScript::currentInterface()->makeWeak(this, data, cb);
  }

void XPersistentScriptValue::dispose()
  {
  XScript::currentInterface()->dispose(this);
  }

namespace XScript
{

Callback::Callback(EngineInterface *ifc, const XScriptFunction &fn)
    : _engineInterface(ifc), _function(fn)
  {
  }

Callback::Callback(EngineInterface *ifc, const XScriptObject& obj, const XScriptFunction &fn)
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
  XScriptFunction fn(_function.asValue());
  return fn.isValid();
  }

void Callback::call(XScriptValue *result, xsize argCount, XScriptValue *args, bool *error, QString *errorOut)
  {
  xAssert(currentInterface() == _engineInterface);
  XScriptObject obj(_object.asValue());
  XScriptFunction fn(_object.asValue());

  if(fn.isValid())
    {
    if(obj.isValid())
      {
      fn.callWithTryCatch(result, obj, argCount, args, error, errorOut);
      }
    else
      {
      fn.callWithTryCatch(result, XScriptObject(), argCount, args, error, errorOut);
      }
    }
  }

CallbackScope::CallbackScope(Callback& callback) : _currentInterface(callback.engineInterface())
  {
  xAssert(_currentInterface);
  _oldInterface = Engine::beginScope(_currentInterface);
  }

CallbackScope::~CallbackScope()
  {
  Engine::endScope(_currentInterface, _oldInterface);
  }

}
