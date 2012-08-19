#include "XScriptFunction.h"
#include "XScriptValue.h"
#include "XScriptObject.h"
#include "XScriptException.h"
#include "XScriptEngine.h"
#include "XAssert"

/*XScriptFunction::XScriptFunction(Function fn)
  {
  XScriptFunctionInternal *internal = XScriptFunctionInternal::init(this);

  v8::Handle<v8::FunctionTemplate> fnTmpl = ::v8::FunctionTemplate::New((v8::InvocationCallback)fn);
  internal->_object = fnTmpl->GetFunction();
  }*/

XScriptFunction::XScriptFunction(const XScriptValue &other)
  {
  XScript::currentInterface()->newFunction(this, other);
  }

XScriptFunction::XScriptFunction()
  {
  XScript::currentInterface()->newFunction(this);
  }

XScriptFunction::~XScriptFunction()
  {
  XScript::currentInterface()->destroy(this);
  }

XScriptFunction::XScriptFunction(const XScriptFunction &other)
  {
  XScript::currentInterface()->newFunction(this, other);
  }

XScriptFunction& XScriptFunction::operator=(const XScriptFunction &other)
  {
  XScript::currentInterface()->destroy(this);
  XScript::currentInterface()->newFunction(this, other);
  return *this;
  }

//XScriptValue XScriptFunction::call(const XScriptObject &self, int argc, const XScriptValue args[]) const
//  {
//  XScript::currentInterface()->call(this, self, argc, args);
//  v8::Locker locker;
//  const XScriptFunctionInternal* func = XScriptFunctionInternal::val(this);

//  try
//    {
//    return (func->_object.IsEmpty() || !func->_object->IsFunction())
//        ? Toss("Illegal argument: empty v8::Handle<>.")
//        : fromHandle(func->_object->Call(getV8Internal(self), argc, getV8Internal(args)));
//    }
//  catch(...)
//    {
//    xAssertFail();
//    }

//  return XScriptValue();
//  }

bool XScriptFunction::isValid() const
  {
  return XScript::currentInterface()->isValid(this);
  }

void XScriptFunction::callWithTryCatch(XScriptValue *result, const XScriptObject &self, int argc, const XScriptValue *args, bool *error, QString *message) const
  {
  XScript::currentInterface()->call(this, result, self, argc, args, error, message);
  }

void XScriptFunction::callAsConstructor(XScriptValue *result, const XScriptArguments &argv)
  {
  XScript::currentInterface()->callAsConstructor(this, result, argv);
  }
