#include "XScriptFunction.h"
#include "XScriptValue.h"
#include "XScriptObject.h"
#include "XScriptException.h"
#include "XScriptValueDartInternals.h"
#include "XScriptValueV8Internals.h"
#include "XAssert"

struct XScriptFunctionInternal
  {
  static XScriptFunctionInternal *init(XScriptFunction *o, const XScriptFunction *other=0)
    {
    XScriptFunctionInternal *internal = (XScriptFunctionInternal*)o;
    new(internal) XScriptFunctionInternal;

    if(other)
      {
      const XScriptFunctionInternal *otherInternal = (const XScriptFunctionInternal*)other;
      internal->_object = otherInternal->_object;
      }

    return internal;
    }

  static void term(XScriptFunction *o)
    {
    XScriptFunctionInternal *internal = (XScriptFunctionInternal*)o;
    (void)internal;
    internal->~XScriptFunctionInternal();
    }

  static const XScriptFunctionInternal *val(const XScriptFunction *o)
    {
    const XScriptFunctionInternal *internal = (const XScriptFunctionInternal*)o;
    return internal;
    }

  static XScriptFunctionInternal *val(XScriptFunction *o)
    {
    XScriptFunctionInternal *internal = (XScriptFunctionInternal*)o;
    return internal;
    }

  mutable v8::Handle<v8::Function> _object;
  };
xCompileTimeAssert(sizeof(XScriptFunction) == sizeof(XScriptFunctionInternal));

struct XScriptArgumentsInternal
  {
  static XScriptArgumentsInternal *init(XScriptFunction *o)
    {
    XScriptArgumentsInternal *internal = (XScriptArgumentsInternal*)o;
    new(internal) XScriptArgumentsInternal;


    return internal;
    }

  static void term(XScriptFunction *o)
    {
    XScriptArgumentsInternal *internal = (XScriptArgumentsInternal*)o;
    (void)internal;
    internal->~XScriptArgumentsInternal();
    }

  static const v8::Arguments &val(const XScriptArguments *o)
    {
    XScriptArgumentsInternal *internal = (XScriptArgumentsInternal*)o;
    return (v8::Arguments&)internal->_args;
    }

  mutable xuint8 _args[sizeof(v8::Arguments)];
  };
xCompileTimeAssert(sizeof(XScriptArguments) == sizeof(XScriptArgumentsInternal));

struct XAccessorInfoInternal
  {
  static XAccessorInfoInternal *init(XAccessorInfo *o)
    {
    XAccessorInfoInternal *internal = (XAccessorInfoInternal*)o;
    new(internal) XAccessorInfoInternal;


    return internal;
    }

  static void term(XAccessorInfo *o)
    {
    XAccessorInfoInternal *internal = (XAccessorInfoInternal*)o;
    (void)internal;
    internal->~XAccessorInfoInternal();
    }

  static const v8::AccessorInfo &val(const XAccessorInfo *o)
    {
    XAccessorInfoInternal *internal = (XAccessorInfoInternal*)o;
    return (v8::AccessorInfo&)internal->_args;
    }

  mutable xuint8 _args[sizeof(v8::AccessorInfo)];
  };
xCompileTimeAssert(sizeof(XAccessorInfo) == sizeof(XAccessorInfoInternal));

XScriptFunction::XScriptFunction(Function fn)
  {
  XScriptFunctionInternal *internal = XScriptFunctionInternal::init(this);

  v8::Handle<v8::FunctionTemplate> fnTmpl = ::v8::FunctionTemplate::New((v8::InvocationCallback)fn);
  internal->_object = fnTmpl->GetFunction();
  }

XScriptFunction::XScriptFunction(const XScriptValue &other)
  {
  const v8::Handle<v8::Value> otherInternal = getV8Internal(other);
  XScriptFunctionInternal *internal = XScriptFunctionInternal::init(this);
  if(getV8Internal(other)->IsFunction())
    {
    internal->_object = v8::Handle<v8::Function>(v8::Function::Cast(*otherInternal));
    }
  }

XScriptFunction::XScriptFunction()
  {
  XScriptFunctionInternal::init(this);
  }

XScriptFunction::~XScriptFunction()
  {
  XScriptFunctionInternal::term(this);
  }

XScriptFunction::XScriptFunction(const XScriptFunction &other)
  {
  XScriptFunctionInternal::init(this, &other);
  }

XScriptFunction& XScriptFunction::operator=(const XScriptFunction &other)
  {
  XScriptFunctionInternal::term(this);
  XScriptFunctionInternal::init(this, &other);
  return *this;
  }

XScriptValue XScriptFunction::call(const XScriptObject &self, int argc, const XScriptValue args[]) const
  {
  v8::Locker locker;
  const XScriptFunctionInternal* func = XScriptFunctionInternal::val(this);

  try
    {
    return (func->_object.IsEmpty() || !func->_object->IsFunction())
        ? Toss("Illegal argument: empty v8::Handle<>.")
        : fromHandle(func->_object->Call(getV8Internal(self), argc, getV8Internal(args)));
    }
  catch(...)
    {
    xAssertFail();
    }

  return XScriptValue();
  }

bool XScriptFunction::isValid() const
  {
  const XScriptFunctionInternal* func = XScriptFunctionInternal::val(this);

  return (!func->_object.IsEmpty() && func->_object->IsFunction());
  }

XScriptValue XScriptFunction::callWithTryCatch(const XScriptObject &self, int argc, const XScriptValue *args, bool *error, QString *message) const
  {
  v8::Locker locker;
  v8::TryCatch trycatch;
  if(error)
  {
    *error = false;
  }

  try
    {
    const XScriptFunctionInternal* func = XScriptFunctionInternal::val(this);
    v8::Handle<v8::Value> result = func->_object->Call(getV8Internal(self), argc, getV8Internal(args));

    if (result.IsEmpty())
      {
      if(error)
        {
        *error = true;
        }
      v8::String::Value exception_str(trycatch.Exception());
      if(message)
        {
        *message = QString((QChar*)*exception_str, exception_str.length());
        }
      return XScriptValue::empty();
      }

    return fromHandle(result);
    }
  catch(...)
    {
    xAssertFail();
    }

  return XScriptValue();
  }

XScriptValue XScriptFunction::callAsConstructor(const XScriptArguments &argv)
  {
  const v8::Arguments &args = XScriptArgumentsInternal::val(&argv);

  const int argc = args.Length();
  std::vector< v8::Handle<v8::Value> > av(static_cast<size_t>(argc),v8::Undefined());

  for( int i = 0; i < argc; ++i )
    {
    av[i] = args[i];
    }

  const XScriptFunctionInternal* func = XScriptFunctionInternal::val(this);
  return fromHandle(func->_object->NewInstance(argc, &av[0]));
  }

XScriptFunction fromFunction(v8::Handle<v8::Function> fn)
  {
  XScriptFunction o;
  XScriptFunctionInternal *internal = XScriptFunctionInternal::val(&o);
  internal->_object = fn;
  return o;
  }

XScriptObject XAccessorInfo::calleeThis() const
  {
  return fromObjectHandle(XAccessorInfoInternal::val(this).This());
  }

XScriptArguments::XScriptArguments()
  {
  }

XScriptArguments::~XScriptArguments()
  {
  }

bool XScriptArguments::isConstructCall() const
  {
  return XScriptArgumentsInternal::val(this).IsConstructCall();
  }

XScriptFunction XScriptArguments::callee() const
  {
  return fromFunction(XScriptArgumentsInternal::val(this).Callee());
  }

XScriptObject XScriptArguments::calleeThis() const
  {
  return fromObjectHandle(XScriptArgumentsInternal::val(this).This());
  }

xsize XScriptArguments::length() const
  {
  return XScriptArgumentsInternal::val(this).Length();
  }

XScriptValue XScriptArguments::at(xsize i) const
  {
  return fromHandle(XScriptArgumentsInternal::val(this)[i]);
  }

v8::Handle<v8::Function> getV8Internal(const XScriptFunction &o)
  {
  const XScriptFunctionInternal *internal = XScriptFunctionInternal::val(&o);
  return internal->_object;
  }

void XScriptDartArguments::setReturnValue(const XScriptValue& val)
  {
#ifdef X_DART
  Dart_SetReturnValue((Dart_NativeArguments)_args, getDartInternal(val));
#endif
  }

XScriptDartArguments::XScriptDartArguments()
  {
  }

xsize XScriptDartArgumentsNoThis::length() const
  {
#ifdef X_DART
  return Dart_GetNativeArgumentCount((Dart_NativeArguments)_args._args) - _offset;
#else
  return 0;
#endif
  }

XScriptValue XScriptDartArgumentsNoThis::at(xsize i) const
  {
#ifdef X_DART
  return fromHandle(Dart_GetNativeArgument((Dart_NativeArguments)_args._args, i + _offset));
#else
  return XScriptValue();
#endif
  }

XScriptObject XScriptDartArgumentsWithThis::calleeThis()
  {
#ifdef X_DART
  return fromObjectHandle(Dart_GetNativeArgument((Dart_NativeArguments)_args._args, 0));
#else
  return XScriptObject();
#endif
  }
