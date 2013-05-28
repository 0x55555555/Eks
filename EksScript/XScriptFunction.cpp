#include "XScriptFunction.h"
#include "XScriptValue.h"
#include "XScriptObject.h"
#include "XScriptEngine.h"
#include "XScriptSource.h"
#include "XAssert"

namespace XScript
{

FunctionScope::FunctionScope(Source &src) : EngineScope(src.engineInterface())
  {
  currentInterface()->beginFunctionScope(this);
  }

/*Function::Function(Function fn)
  {
  FunctionInternal *internal = FunctionInternal::init(this);

  v8::Handle<v8::FunctionTemplate> fnTmpl = ::v8::FunctionTemplate::New((v8::InvocationCallback)fn);
  internal->_object = fnTmpl->GetFunction();
  }*/

Function::Function(const Value &other)
  {
  currentInterface()->newFunction(this, other);
  }

Function::Function()
  {
  currentInterface()->newFunction(this);
  }

Function::~Function()
  {
  currentInterface()->destroy(this);
  }

Function::Function(const Function &other)
  {
  currentInterface()->newFunction(this, other);
  }

Function& Function::operator=(const Function &other)
  {
  currentInterface()->destroy(this);
  currentInterface()->newFunction(this, other);
  return *this;
  }

//XScriptValue Function::call(const XScriptObject &self, int argc, const XScriptValue args[]) const
//  {
//  currentInterface()->call(this, self, argc, args);
//  v8::Locker locker;
//  const FunctionInternal* func = FunctionInternal::val(this);

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

bool Function::isValid() const
  {
  return currentInterface()->isValid(this);
  }

void Function::callWithTryCatch(Value *result, const Object &self, xsize argc, const Value *args, bool *error, QString *message) const
  {
  currentInterface()->call(this, result, self, argc, args, error, message);
  }

}
