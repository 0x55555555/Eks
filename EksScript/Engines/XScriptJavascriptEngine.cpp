#include "../XScriptEngine.h"
#include "../XScriptValueV8Internals.h"
#include "../XScriptValue.h"
#include "../XScriptObject.h"
#include "../XScriptFunction.h"
#include "../XConvertFromScript.h"
#include "XAssert"

namespace
{

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

XScriptValue XAccessorInfo::data() const
  {
  return fromHandle(XAccessorInfoInternal::val(this).Data());
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

XScriptValue fromHandle(v8::Handle<v8::Value> v)
  {
  XScriptValue o;
  XScriptValueInternal *internal = XScriptValueInternal::val(&o);
  internal->_object = v;
  return o;
  }

v8::Handle<v8::Value> getV8Internal(const XScriptValue &o)
  {
  const XScriptValueInternal *internal = XScriptValueInternal::val(&o);
  return internal->_object;
  }

v8::Handle<v8::Value> *getV8Internal(const XScriptValue *o)
  {
  return (v8::Handle<v8::Value> *)o;
  }

namespace XScript
{

void fatal(const char* location, const char* message)
  {
  qFatal("%s: %s", location, message);
  }

void debugHandler()
  {
  // We are in some random thread. We should already have v8::Locker acquired
  // (we requested this when registered this callback). We was called
  // because new debug messages arrived; they may have already been processed,
  // but we shouldn't worry about this.
  //
  // All we have to do is to set context and call ProcessDebugMessages.
  //
  // We should decide which V8 context to use here. This is important for
  // "evaluate" command, because it must be executed some context.
  // In our sample we have only one context, so there is nothing really to
  // think about.

  v8::HandleScope scope;

  //v8::Context::Scope contextScope(g_engine->context);

  //v8::Debug::ProcessDebugMessages();
  }

class JavascriptEngineInterface : public EngineInterface
  {
  v8::Locker locker;
  v8::HandleScope scope;
  v8::Handle<v8::ObjectTemplate> globalTemplate;
  v8::Persistent<v8::Context> context;
  v8::Context::Scope contextScope;
  v8::Unlocker unlocker;

public:
  JavascriptEngineInterface(bool debugging)
      : globalTemplate(v8::ObjectTemplate::New()),
      context(v8::Context::New(NULL, globalTemplate)),
      contextScope(context)
    {
    v8::V8::SetFatalErrorHandler(fatal);

    context->AllowCodeGenerationFromStrings(false);

    if(debugging)
      {
      v8::Debug::EnableAgent("XScriptAgent", 9222, false);
      v8::Debug::SetDebugMessageDispatchHandler(debugHandler, true);
      }
    }

  ~JavascriptEngineInterface()
    {
    v8::V8::LowMemoryNotification();
    context.Dispose();
    }

  void addInterface(const XInterfaceBase *i)
    {
    xAssert(i->isSealed());
    i->addClassTo(i->typeName(), fromHandle(g_engine->context->Global()));
    }

  void removeInterface(const XInterfaceBase *i)
    {
    xAssert(i->isSealed());
    fromObjectHandle(g_engine->context->Global()).set(i->typeName(), XScriptValue());
    }

  void newValue(XScriptValue *v) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = v8::Null();
    }

  void newValue(XScriptValue *v, bool x) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = v8::Boolean::New(x);
    }

  void newValue(XScriptValue *v, xuint32 x) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = v8::Integer::New(x);
    }

  void newValue(XScriptValue *v, xint32 x) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = v8::Integer::New(x);
    }

  void newValue(XScriptValue *v, xuint64 x) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = v8::Number::New(x);
    }

  void newValue(XScriptValue *v, xint64 x) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = v8::Number::New(x);
    }

  void newValue(XScriptValue *v, double x) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = v8::Number::New(x);
    }

  void newValue(XScriptValue *v, float x) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = v8::Number::New(x);
    }

  void newValue(XScriptValue*, QVariantMap &) X_OVERRIDE
    {
    xAssertFail();
    }

  void newValue(XScriptValue*, QVariantList &) X_OVERRIDE
    {
    xAssertFail();
    }

  void newValue(XScriptValue*, QStringList &) X_OVERRIDE
    {
    xAssertFail();
    }

  void newValue(XScriptValue *v, const QString &str) X_OVERRIDE
    {
    v8::Locker l;
    XScriptValueInternal *internal = XScriptValueInternal::init(v);

    if(str.length())
      {
      internal->_object = v8::String::New((const uint16_t*)str.constData(), str.length());
      }
    else
      {
      internal->_object = v8::String::New("");
      }
    }

  void newValue(XScriptValue *v, const XScriptObject &obj) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = getV8Internal(obj);
    }

  void newValue(XScriptValue *v, const XScriptFunction &obj) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = getV8Internal(obj);
    }

  void newValue(XScriptValue *v, void* val) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::init(v);
    internal->_object = v8::External::New(val);
    }

  void destroy(XScriptValue* v) X_OVERRIDE
    {
    XScriptValueInternal::term(v);
    }


  void newEmpty(XScriptValue *v) X_OVERRIDE
    {
    XScriptValueInternal *internal = XScriptValueInternal::val(v);
    internal->_object.Clear();
    }

  void newArray(XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    internal->_object = v8::Array::New();
    }

  bool isValid(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    return !internal->_object.IsEmpty() &&
        !internal->_object->IsNull() &&
        !internal->_object->IsUndefined();
    }

  bool isObject(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    return internal->_object->IsObject();
    }

  bool isBoolean(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    return internal->_object->IsBoolean();
    }

  bool isArray(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    return internal->_object->IsArray();
    }

  bool isNumber(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    return internal->_object->IsNumber();
    }

  bool isString(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    return internal->_object->IsString();
    }

  bool isInteger(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    return internal->_object->IsInt32() || internal->_object->IsUint32();
    }

  xsize length(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    v8::Handle<v8::Array> arr = internal->_object.As<v8::Array>();
    return arr->Length();
    }

  void at(XScriptValue* out, const XScriptValue *v, xsize id) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    XScriptValueInternal *outInternal = XScriptValueInternal::val(out);
    v8::Handle<v8::Array> arr = internal->_object.As<v8::Array>();
    outInternal->_object = arr->Get(id);
    }

  void set(XScriptValue *v, xsize id, const XScriptValue &val) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    v8::Handle<v8::Array> arr = internal->_object.As<v8::Array>();
    arr->Set(id, getV8Internal(val));
    }

  void *toExternal(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    return internal->_object.As<v8::External>()->Value();
    }

  double toNumber(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    return internal->_object.As<v8::Number>()->Value();
    }

  xint64 toInteger(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    if(internal->_object->IsNumber())
      {
      return internal->_object.As<v8::Number>()->Value();
      }
    return 0;
    }

  bool toBoolean(const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    if(internal->_object->IsBoolean())
      {
      return internal->_object->ToBoolean()->Value();
      }
    return false;
    }

  void toString(QString *val, const XScriptValue *v) X_OVERRIDE
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(v);
    if(!internal->_object.IsEmpty())
      {
      v8::Handle<v8::String> str = internal->_object->ToString();
      if(!str.IsEmpty())
        {
        QString strOut;
        strOut.resize(str->Length());

        xCompileTimeAssert(sizeof(QChar) == sizeof(uint16_t));
        str->Write((uint16_t*)strOut.data(), 0, strOut.length());
        return;
        }
      else
        {
        //xAssertFail();
        //v8::String::AsciiValue exception_str(internal->_object);
        //return *exception_str;
        }
      }
    *val = QString();
    }

  void toList(QVariantList *, const XScriptValue *) X_OVERRIDE
    {
    }

  void toMap(QVariantMap *out, const XScriptValue *val) X_OVERRIDE
    {
    out->clear();
    const XScriptValueInternal *internal = XScriptValueInternal::val(val);
    if(internal->_object->IsObject())
      {
      v8::Handle<v8::Object> obj = internal->_object.As<v8::Object>();
      v8::Handle<v8::Array> propertyNames = obj->GetOwnPropertyNames();
      for(uint32_t i = 0, s = propertyNames->Length(); i < s; ++i)
        {
        v8::Handle<v8::Value> key = propertyNames->Get(i);
        v8::Handle<v8::Value> value = obj->Get(key);

        out->insert(XScriptConvert::from<QString>(fromHandle(key)),
                   fromHandle(value).toVariant(QVariant::Invalid));
        }
      }
    }

  void newPersistentValue(XPersistentScriptValue *value)
    {
    XPersistentScriptValueInternal::init(value);
    }

  void newPersistentValue(XPersistentScriptValue *value, const XScriptValue &in)
    {
    const XPersistentScriptValueInternal *internal = XPersistentScriptValueInternal::init(value);

    const XScriptValueInternal *other = XScriptValueInternal::val(&in);
    internal->_object = v8::Persistent<v8::Value>::New(other->_object);
    }

  void asValue(XScriptValue *out, const XPersistentScriptValue *value)
    {
    const XScriptValueInternal *internal = XScriptValueInternal::val(out);
    const XPersistentScriptValueInternal *other = XPersistentScriptValueInternal::val(value);

    internal->_object = other->_object;
    }

  void makeWeak(XPersistentScriptValue *val, void *data, WeakDtor cb)
    {
    const XPersistentScriptValueInternal *internal = XPersistentScriptValueInternal::val(val);
    internal->_object.MakeWeak(data, (v8::WeakReferenceCallback)cb);
    }

  void dispose(XPersistentScriptValue *val)
    {
    const XPersistentScriptValueInternal *internal = XPersistentScriptValueInternal::val(val);
    internal->_object.Dispose();
    internal->_object.Clear();
    }


  void newFunction(XScriptFunction *fn)
    {
    XScriptFunctionInternal::init(fn);
    }

  void newFunction(XScriptFunction *fn, const XScriptValue &other)
    {
    const v8::Handle<v8::Value> otherInternal = getV8Internal(other);
    XScriptFunctionInternal *internal = XScriptFunctionInternal::init(fn);
    if(getV8Internal(other)->IsFunction())
      {
      internal->_object = v8::Handle<v8::Function>(v8::Function::Cast(*otherInternal));
      }
    }

  void newFunction(XScriptFunction *fn, const XScriptFunction &oth)
    {
    XScriptFunctionInternal::init(fn, &oth);
    }

  void destroy(XScriptFunction *v)
    {
    XScriptFunctionInternal::term(v);
    }

  bool isValid(const XScriptFunction *v)
    {
    const XScriptFunctionInternal* func = XScriptFunctionInternal::val(v);

    return (!func->_object.IsEmpty() && func->_object->IsFunction());
    }

  void call(const XScriptFunction *fn,
            XScriptValue *out,
            const XScriptObject &self,
            int argc,
            const XScriptValue *args,
            bool *error,
            QString *message)
    {
    v8::Locker locker;
    v8::TryCatch trycatch;
    if(error)
    {
      *error = false;
    }

    try
      {
      const XScriptFunctionInternal* func = XScriptFunctionInternal::val(fn);
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
        *out = XScriptValue::newEmpty();
        }

      *out = fromHandle(result);
      }
    catch(...)
      {
      xAssertFail();
      }
    }

  void callAsConstructor(const XScriptFunction *fn, XScriptValue *result, const XScriptArguments &argv)
    {
    const v8::Arguments &args = XScriptArgumentsInternal::val(&argv);

    const int argc = args.Length();
    std::vector< v8::Handle<v8::Value> > av(static_cast<size_t>(argc),v8::Undefined());

    for( int i = 0; i < argc; ++i )
      {
      av[i] = args[i];
      }

    const XScriptFunctionInternal* func = XScriptFunctionInternal::val(fn);
    *result = fromHandle(func->_object->NewInstance(argc, &av[0]));
    }

  void beginFunctionScope(XScriptFunction::Scope *sc)
    {
    v8::Locker l;
    v8::Handle<v8::Context> ctxt = getV8EngineInternal();

    v8::HandleScope handle_scope;
    v8::Context::Scope scope(ctxt);
    }

  void endFunctionScope(XScriptFunction::Scope *sc)
    {
    }
  };

EngineInterface *createV8Interface(bool debugging)
  {
  return new JavascriptEngineInterface(bool debugging);
  }

}
