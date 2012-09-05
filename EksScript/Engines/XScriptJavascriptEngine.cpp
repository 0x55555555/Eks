#include "../XScriptGlobal.h"

#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
# include "v8.h"
# include "v8-debug.h"
#endif

#include "../XScriptEngine.h"
#include "../XScriptValue.h"
#include "../XScriptSource.h"
#include "../XScriptObject.h"
#include "../XScriptFunction.h"
#include "../XConvertFromScript.h"
#include "../XInterface.h"

#include "XAssert"

namespace XScript
{

namespace internal
{

#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT

struct JSArgumentsInternal
  {
  static JSArgumentsInternal *init(Function *o)
    {
    JSArgumentsInternal *internal = (JSArgumentsInternal*)o;
    new(internal) JSArgumentsInternal;


    return internal;
    }

  static void term(Function *o)
    {
    JSArgumentsInternal *internal = (JSArgumentsInternal*)o;
    (void)internal;
    internal->~JSArgumentsInternal();
    }

  static const v8::Arguments &val(const internal::JSArguments *o)
    {
    JSArgumentsInternal *internal = (JSArgumentsInternal*)o;
    return (v8::Arguments&)internal->_args;
    }

  mutable xuint8 _args[sizeof(v8::Arguments)];
  };
xCompileTimeAssert(sizeof(internal::JSArguments) == sizeof(JSArgumentsInternal));

struct JSAccessorInfoInternal
  {
  static JSAccessorInfoInternal *init(internal::JSAccessorInfo *o)
    {
    JSAccessorInfoInternal *internal = (JSAccessorInfoInternal*)o;
    new(internal) JSAccessorInfoInternal;


    return internal;
    }

  static void term(internal::JSAccessorInfo *o)
    {
    JSAccessorInfoInternal *internal = (JSAccessorInfoInternal*)o;
    (void)internal;
    internal->~JSAccessorInfoInternal();
    }

  static const v8::AccessorInfo &val(const internal::JSAccessorInfo *o)
    {
    JSAccessorInfoInternal *internal = (JSAccessorInfoInternal*)o;
    return (v8::AccessorInfo&)internal->_args;
    }

  mutable xuint8 _args[sizeof(v8::AccessorInfo)];
  };
xCompileTimeAssert(sizeof(internal::JSAccessorInfo) == sizeof(JSAccessorInfoInternal));

#endif

Object JSAccessorInfo::calleeThis() const
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
  return fromObjectHandle(JSAccessorInfoInternal::val(this).This());
#else
  return Object();
#endif
  }

Value JSAccessorInfo::data() const
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
  return fromHandle(JSAccessorInfoInternal::val(this).Data());
#else
  return Value();
#endif
  }

JSArguments::JSArguments()
  {
  }

JSArguments::~JSArguments()
  {
  }

bool JSArguments::isConstructCall() const
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
  return JSArgumentsInternal::val(this).IsConstructCall();
#else
  return false;
#endif
  }

Function JSArguments::callee() const
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
  return fromFunction(JSArgumentsInternal::val(this).Callee());
#else
  return Function();
#endif
  }

Object JSArguments::calleeThis() const
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
  return fromObjectHandle(JSArgumentsInternal::val(this).This());
#else
  return Object();
#endif
  }

xsize JSArguments::length() const
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
  return JSArgumentsInternal::val(this).Length();
#else
  return 0;
#endif
  }

Value JSArguments::at(xsize i) const
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
  return fromHandle(JSArgumentsInternal::val(this)[i]);
#else
  (void)i;
  return Value();
#endif
  }

}

}

#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT

namespace XScript
{

struct FunctionInternal
  {
  static FunctionInternal *init(Function *o, const Function *other=0)
    {
    FunctionInternal *internal = (Function  *)o;
    new(internal) FunctionInternal;

    if(other)
      {
      const FunctionInternal *otherInternal = (const FunctionInternal*)other;
      internal->_object = otherInternal->_object;
      }

    return internal;
    }

  static void term(Function *o)
    {
    FunctionInternal *internal = (FunctionInternal*)o;
    (void)internal;
    internal->~FunctionInternal();
    }

  static const FunctionInternal *val(const Function *o)
    {
    const FunctionInternal *internal = (const FunctionInternal*)o;
    return internal;
    }

  static FunctionInternal *val(Function *o)
    {
    FunctionInternal *internal = (FunctionInternal*)o;
    return internal;
    }

  mutable v8::Handle<v8::Function> _object;
  };
xCompileTimeAssert(sizeof(Function) == sizeof(FunctionInternal));



struct ValueInternal
  {
  static ValueInternal *init(Value *o, const Value *other=0)
    {
    ValueInternal *internal = (ValueInternal*)o;
    new(internal) ValueInternal;

    if(other)
      {
      const ValueInternal *otherInternal = (const ValueInternal*)other;
      internal->_object = otherInternal->_object;
      }

    return internal;
    }

  static void term(Value *o)
    {
    ValueInternal *internal = (ValueInternal*)o;
    (void)internal;
    internal->~ValueInternal();
    }

  static const ValueInternal *val(const Value *o)
    {
    const ValueInternal *internal = (ValueInternal*)o;
    return internal;
    }

  static ValueInternal *val(Value *o)
    {
    ValueInternal *internal = (ValueInternal*)o;
    return internal;
    }

  mutable v8::Handle<v8::Value> _object;
  };
xCompileTimeAssert(sizeof(Value) == sizeof(ValueInternal));

struct SourceImpl
  {
  static SourceImpl* impl(Source *s) { return reinterpret_cast<SourceImpl*>(s); }
  static const SourceImpl* impl(const Source *s) { return reinterpret_cast<const SourceImpl*>(s); }

  v8::Handle<v8::Script> _script;
  };

xCompileTimeAssert(sizeof(SourceImpl) == sizeof(Source));

struct ObjectInternal
  {
  static ObjectInternal *init(Object *o, const Object *other=0)
    {
    ObjectInternal *internal = (ObjectInternal*)o;
    new(internal) ObjectInternal;

    if(other)
      {
      const ObjectInternal *otherInternal = (const ObjectInternal*)other;
      internal->_object = otherInternal->_object;
      }

    return internal;
    }

  static void term(Object *o)
    {
    ObjectInternal *internal = (ObjectInternal*)o;
    (void)internal;
    internal->~ObjectInternal();
    }

  static const ObjectInternal *val(const Object *o)
    {
    ObjectInternal *internal = (ObjectInternal*)o;
    return internal;
    }

  static ObjectInternal *val(Object *o)
    {
    ObjectInternal *internal = (ObjectInternal*)o;
    return internal;
    }

  mutable v8::Handle<v8::Object> _object;
  };
xCompileTimeAssert(sizeof(Object) == sizeof(ObjectInternal));

struct PersistentValueInternal
  {
  static PersistentValueInternal *init(PersistentValue *o, const PersistentValue *other=0)
    {
    PersistentValueInternal *internal = (PersistentValueInternal*)o;
    new(internal) PersistentValueInternal;

    if(other)
      {
      const PersistentValueInternal *otherInternal = (const PersistentValueInternal*)other;
      internal->_object = otherInternal->_object;
      }

    return internal;
    }

  static void term(PersistentValue *o)
    {
    PersistentValueInternal *internal = (PersistentValueInternal*)o;
    (void)internal;
    internal->~PersistentValueInternal();
    }

  static const PersistentValueInternal *val(const PersistentValue *o)
    {
    const PersistentValueInternal *internal = (PersistentValueInternal*)o;
    return internal;
    }

  static PersistentValueInternal *val(PersistentValue *o)
    {
    PersistentValueInternal *internal = (PersistentValueInternal*)o;
    return internal;
    }

  mutable v8::Persistent<v8::Value> _object;
  };
xCompileTimeAssert(sizeof(PersistentValue) == sizeof(PersistentValueInternal));



v8::Handle<v8::Function> getV8Internal(const Function &o)
  {
  const FunctionInternal *internal = FunctionInternal::val(&o);
  return internal->_object;
  }

v8::Handle<v8::Object> getV8Internal(const Object &o)
  {
  const ObjectInternal *internal = ObjectInternal::val(&o);
  return internal->_object;
  }

Object fromObjectHandle(v8::Handle<v8::Object> v)
  {
  Object o;
  ObjectInternal *internal = ObjectInternal::val(&o);
  internal->_object = v;
  return o;
  }

Value fromHandle(v8::Handle<v8::Value> v)
  {
  Value o;
  ValueInternal *internal = ValueInternal::val(&o);
  internal->_object = v;
  return o;
  }

v8::Handle<v8::Value> &getV8Internal(const Value *o)
  {
  return *(v8::Handle<v8::Value> *)o;
  }

v8::Handle<v8::Object> &getV8Internal(const Object *o)
  {
  return *(v8::Handle<v8::Object> *)o;
  }

#if 0
v8::Handle<v8::ObjectTemplate> getV8Internal(InterfaceBase *o)
  {
  void *proto = o->prototype();
  return *::prototype(proto);
  }
#endif

Function fromFunction(v8::Handle<v8::Function> fn)
  {
  Function o;
  FunctionInternal *internal = FunctionInternal::val(&o);
  internal->_object = fn;
  return o;
  }

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

  bool supportsExtension(const QString &ext)
    {
    return ext == "js";
    }

  void throwError(Value *ret, const QString &err) X_OVERRIDE
    {
    v8::Handle<v8::String> string = v8::String::New(err.toUtf8().data());

    ValueInternal *internal = ValueInternal::init(ret);
    internal->_object = v8::ThrowException(string);
    }

  bool loadSource(Source *src, const QString &key, const QString &data) X_OVERRIDE
    {
    SourceImpl *i = SourceImpl::impl(src);

    v8::Locker locker;
    // Create a string containing the JavaScript source code.
    v8::Handle<v8::String> source = v8::String::New((xuint16*)data.constData(), data.length());

    v8::Handle<v8::String> fileNameV8 = v8::String::New((xuint16*)key.constData(), key.length());

    // Compile the source code.
    i->_script = v8::Script::Compile(source, fileNameV8);
    }

  void runSource(Value *result, const Source *src, SourceError *error) X_OVERRIDE
    {
    // Run the script to get the result.
    const SourceImpl *i = SourceImpl::impl(src);


    ValueInternal *resultInt = ValueInternal::init(result);

    v8::Locker locker;
    v8::TryCatch trycatch;
    if(i->_script.IsEmpty())
      {
      error->setHasError(true);
      error->setTrace("Running invalid script");

      resultInt->_object = v8::Undefined();
      return;
      }

    v8::Handle<v8::Value> resultValue = i->_script->Run();

    if(error)
      {
      error->setHasError(false);
      }

    if (trycatch.HasCaught())
      {
      if(error)
        {
        error->setHasError(true);
        error->setTrace(Convert::from<QString>(fromHandle(trycatch.StackTrace())));

        if(!trycatch.Message().IsEmpty())
          {
          v8::Local<v8::Message> mess = trycatch.Message();

          error->setMessage(Convert::from<QString>(fromHandle(mess->Get())));
          error->setLineNumber(mess->GetLineNumber());
          }
        }
      resultInt->_object = trycatch.Exception();
      }

    resultInt->_object = resultValue;
    }

  void addInterface(const InterfaceBase *i) X_OVERRIDE
    {
    typedef v8::Persistent<v8::FunctionTemplate> FnTempl;
    typedef v8::Persistent<v8::ObjectTemplate> ObjTempl;

    new(constructor(_constructor)) FnTempl(FnTempl::New(v8::FunctionTemplate::New((v8::InvocationCallback)ctor)));
    new(::prototype(_prototype)) ObjTempl(ObjTempl::New((*constructor(_constructor))->PrototypeTemplate()));

    v8::Handle<v8::String> typeNameV8 = v8::String::New("typeName");
    v8::Handle<v8::String> typeNameStrV8 = v8::String::New((uint16_t*)typeName.constData(), typeName.length());
    (*constructor(_constructor))->Set(typeNameV8, typeNameStrV8);
    (*::prototype(_prototype))->Set(typeNameV8, typeNameStrV8);

    (*constructor(_constructor))->InstanceTemplate()->SetInternalFieldCount(2);

    if(ifc->parent())
      {
      FnTempl* templ = constructor(_constructor);
      const FnTempl* pTempl = constructor(parentType->_constructor);
      (*templ)->Inherit( (*pTempl) );
      }

    for(xsize c = 0; c < ctorCount; ++c)
      {
      }

    for(xsize p = 0; p < propCount; ++p)
      {
      PropertyDef &prop = props[p];

      (*::prototype(_prototype))->SetAccessor(v8::String::New(cname), (v8::AccessorGetter)getter, (v8::AccessorSetter)setter);
      }

    for(xsize f = 0; f < fnCount; ++f)
      {
      FunctionDef &fn = fns[f];

      v8::Handle<v8::FunctionTemplate> fnTmpl = ::v8::FunctionTemplate::New((v8::InvocationCallback)fn);


      v8::Handle<v8::Integer> id = v8::Integer::New(userData);
      fnTmpl->Set(v8::String::New("0"), id);

      (*::prototype(_prototype))->Set(v8::String::New(cname), fnTmpl);
      }

    xAssert(i->isSealed());
    i->addClassTo(i->typeName(), fromHandle(g_engine->context->Global()));
    }

  void removeInterface(const InterfaceBase *i) X_OVERRIDE
    {
    xAssert(i->isSealed());
    fromObjectHandle(context->Global()).set(i->typeName(), Value());

    constructor(_constructor)->~FnTempl();
    ::prototype(_prototype)->~ObjTempl();
    }

  void wrapInstance(const InterfaceBase *ifc, Object *scObj, void *object) X_OVERRIDE
    {
    xAssert(findInterface(ifc->baseTypeId()));
    v8::Handle<v8::Object> obj = getV8Internal(scObj);
    xsize tId = ifc->typeIdField();
    if( 0 <= tId )
      {
      xAssert(tId < (xsize)obj->InternalFieldCount());
      obj->SetPointerInInternalField(tId, (void*)ifc->baseTypeId());
      }
    xAssert(ifc->nativeField() < (xsize)obj->InternalFieldCount());
    obj->SetPointerInInternalField(ifc->nativeField(), object);
    }

  void unwrapInstance(const InterfaceBase *ifc, Object *scObj) X_OVERRIDE
    {
    v8::Locker l;
    v8::Handle<v8::Object> object = getV8Internal(scObj);
    xAssert(_nativeField < (xsize)object->InternalFieldCount());
    object->SetInternalField(ifc->nativeField(), v8::Null());

    xsize tId = ifc->typeIdField();
    if(0 <= tId)
      {
      xAssert(_typeIdField < (xsize)object->InternalFieldCount());
      object->SetInternalField(tId, v8::Null());
      }
    }

  void newInstance(Object *result,
                   const InterfaceBase *ifc,
                   int argc,
                   Value argv[],
                   const QString& name) const X_OVERRIDE
    {
    v8::Locker l;
    v8::Handle<v8::Object> newObj = getV8Internal(constructorFunction())->NewInstance(argc, getV8Internal(argv));

#ifdef X_DEBUG
    v8::Handle<v8::Value> proto = newObj->GetPrototype();
    xAssert(!proto.IsEmpty());
    xAssert(proto->IsObject());
#endif

    ObjectInternal *internal = ObjectInternal::init(result);
    internal->_object = newObj;
    }

  void newValue(Value *v) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = v8::Null();
    }

  void newValue(Value *v, bool x) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = v8::Boolean::New(x);
    }

  void newValue(Value *v, xuint32 x) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = v8::Integer::New(x);
    }

  void newValue(Value *v, xint32 x) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = v8::Integer::New(x);
    }

  void newValue(Value *v, xuint64 x) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = v8::Number::New(x);
    }

  void newValue(Value *v, xint64 x) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = v8::Number::New(x);
    }

  void newValue(Value *v, double x) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = v8::Number::New(x);
    }

  void newValue(Value *v, float x) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = v8::Number::New(x);
    }

  void newValue(Value*, QVariantMap &) X_OVERRIDE
    {
    xAssertFail();
    }

  void newValue(Value*, QVariantList &) X_OVERRIDE
    {
    xAssertFail();
    }

  void newValue(Value*, QStringList &) X_OVERRIDE
    {
    xAssertFail();
    }

  void newValue(Value *v, const QString &str) X_OVERRIDE
    {
    v8::Locker l;
    ValueInternal *internal = ValueInternal::init(v);

    if(str.length())
      {
      internal->_object = v8::String::New((const uint16_t*)str.constData(), str.length());
      }
    else
      {
      internal->_object = v8::String::New("");
      }
    }

  void newValue(Value *v, const Object *obj) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = getV8Internal(obj);
    }

  void newValue(Value *v, const Function *obj) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = getV8Internal(obj);
    }

  void newValue(Value *v, void* val) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::init(v);
    internal->_object = v8::External::New(val);
    }

  void destroy(Value* v) X_OVERRIDE
    {
    ValueInternal::term(v);
    }


  void newEmpty(Value *v) X_OVERRIDE
    {
    ValueInternal *internal = ValueInternal::val(v);
    internal->_object.Clear();
    }

  void newArray(Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    internal->_object = v8::Array::New();
    }

  bool isValid(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    return !internal->_object.IsEmpty() &&
        !internal->_object->IsNull() &&
        !internal->_object->IsUndefined();
    }

  bool isObject(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    return internal->_object->IsObject();
    }

  bool isBoolean(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    return internal->_object->IsBoolean();
    }

  bool isArray(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    return internal->_object->IsArray();
    }

  bool isNumber(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    return internal->_object->IsNumber();
    }

  bool isString(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    return internal->_object->IsString();
    }

  bool isInteger(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    return internal->_object->IsInt32() || internal->_object->IsUint32();
    }

  xsize length(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    v8::Handle<v8::Array> arr = internal->_object.As<v8::Array>();
    return arr->Length();
    }

  void at(Value* out, const Value *v, xsize id) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    ValueInternal *outInternal = ValueInternal::val(out);
    v8::Handle<v8::Array> arr = internal->_object.As<v8::Array>();
    outInternal->_object = arr->Get(id);
    }

  void set(Value *v, xsize id, const Value *val) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    v8::Handle<v8::Array> arr = internal->_object.As<v8::Array>();
    arr->Set(id, getV8Internal(*val));
    }

  void *toExternal(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    return internal->_object.As<v8::External>()->Value();
    }

  double toNumber(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    return internal->_object.As<v8::Number>()->Value();
    }

  xint64 toInteger(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    if(internal->_object->IsNumber())
      {
      return internal->_object.As<v8::Number>()->Value();
      }
    return 0;
    }

  bool toBoolean(const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
    if(internal->_object->IsBoolean())
      {
      return internal->_object->ToBoolean()->Value();
      }
    return false;
    }

  void toString(QString *val, const Value *v) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(v);
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

  void toList(QVariantList *, const Value *) X_OVERRIDE
    {
    }

  void toMap(QVariantMap *out, const Value *val) X_OVERRIDE
    {
    out->clear();
    const ValueInternal *internal = ValueInternal::val(val);
    if(internal->_object->IsObject())
      {
      v8::Handle<v8::Object> obj = internal->_object.As<v8::Object>();
      v8::Handle<v8::Array> propertyNames = obj->GetOwnPropertyNames();
      for(uint32_t i = 0, s = propertyNames->Length(); i < s; ++i)
        {
        v8::Handle<v8::Value> key = propertyNames->Get(i);
        v8::Handle<v8::Value> value = obj->Get(key);

        out->insert(Convert::from<QString>(fromHandle(key)),
                   fromHandle(value).toVariant(QVariant::Invalid));
        }
      }
    }

  void newPersistentValue(PersistentValue *value) X_OVERRIDE
    {
    PersistentValueInternal::init(value);
    }

  void newPersistentValue(PersistentValue *value, const Value &in) X_OVERRIDE
    {
    const PersistentValueInternal *internal = PersistentValueInternal::init(value);

    const ValueInternal *other = ValueInternal::val(&in);
    internal->_object = v8::Persistent<v8::Value>::New(other->_object);
    }

  void asValue(Value *out, const PersistentValue *value) X_OVERRIDE
    {
    const ValueInternal *internal = ValueInternal::val(out);
    const PersistentValueInternal *other = PersistentValueInternal::val(value);

    internal->_object = other->_object;
    }

  void makeWeak(PersistentValue *val, void *data, WeakDtor cb) X_OVERRIDE
    {
    const PersistentValueInternal *internal = PersistentValueInternal::val(val);
    internal->_object.MakeWeak(data, (v8::WeakReferenceCallback)cb);
    }

  void dispose(PersistentValue *val) X_OVERRIDE
    {
    const PersistentValueInternal *internal = PersistentValueInternal::val(val);
    internal->_object.Dispose();
    internal->_object.Clear();
    }

  void newObject(Object *obj) X_OVERRIDE
    {
    ObjectInternal::init(obj);
    }

  void newObject(Object *obj, const Value *v) X_OVERRIDE
    {
    getDartHandle(obj) = v;
    }

  void newObject(Object *obj, const Object *o) X_OVERRIDE
    {
    const v8::Handle<v8::Value> otherInternal = getV8Internal(other);
    ObjectInternal *internal = ObjectInternal::init(this);
    if(getV8Internal(other)->IsObject())
      {
      internal->_object = v8::Handle<v8::Object>(v8::Object::Cast(*otherInternal));
      }
    }

  void destroy(Object *obj) X_OVERRIDE
    {
    ObjectInternal::term(obj);
    }

  void *internalField(const Object *obj, ObjectInternalField idx) X_OVERRIDE
    {
    void *data = 0;
    v8::Handle<v8::Value> proto(obj);
    while(!data && proto->IsObject())
      {
      v8::Handle<v8::Object> const &protoObj(proto);

      data = protoObj->GetPointerFromInternalField(idx);

      proto = protoObj->GetPrototype();
      }

    return data;
    }

  bool isValid(const Object *o) X_OVERRIDE
    {
    const ObjectInternal *internal = ObjectInternal::val(o);
    return !internal->_object.IsEmpty() && internal->_object->IsObject();
    }

  void newMap(Object *obj) X_OVERRIDE
    {
    const ObjectInternal *internal = ObjectInternal::val(obj);
    internal->_object = v8::Object::New();
    }

  void get(Value *ret, const Object *obj, const QString &n) X_OVERRIDE
    {
    Value key = Convert::to(n);

    const ObjectInternal *internal = ObjectInternal::val(this);

    ValueInternal *out = ValueInternal::init(ret);
    out->_object = internal->_object->Get(getV8Internal(key));
    }

  void set(Object *obj, const QString &n, const Value *val) X_OVERRIDE
    {
    Value key = Convert::to(n);

    const ObjectInternal *internal = ObjectInternal::val(obj);
    internal->_object->Set(getV8Internal(key), getV8Internal(v));
    }

  void newFunction(Function *fn) X_OVERRIDE
    {
    FunctionInternal::init(fn);
    }

  void newFunction(Function *fn, const Value &other) X_OVERRIDE
    {
    const v8::Handle<v8::Value> otherInternal = getV8Internal(other);
    FunctionInternal *internal = FunctionInternal::init(fn);
    if(getV8Internal(other)->IsFunction())
      {
      internal->_object = v8::Handle<v8::Function>(v8::Function::Cast(*otherInternal));
      }
    }

  void newFunction(Function *fn, const Function &oth) X_OVERRIDE
    {
    FunctionInternal::init(fn, &oth);
    }

  void destroy(Function *v) X_OVERRIDE
    {
    FunctionInternal::term(v);
    }

  bool isValid(const Function *v) X_OVERRIDE
    {
    const FunctionInternal* func = FunctionInternal::val(v);

    return (!func->_object.IsEmpty() && func->_object->IsFunction());
    }

  void call(const Function *fn,
            Value *out,
            const Object &self,
            int argc,
            const Value *args,
            bool *error,
            QString *message) X_OVERRIDE
    {
    v8::Locker locker;
    v8::TryCatch trycatch;
    if(error)
    {
      *error = false;
    }

    try
      {
      const FunctionInternal* func = FunctionInternal::val(fn);
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
        *out = Value::newEmpty();
        }

      *out = fromHandle(result);
      }
    catch(...)
      {
      xAssertFail();
      }
    }

  void callAsConstructor(const Function *fn, Value *result, const internal::JSArguments &argv)
    {
    const v8::Arguments &args = JSArgumentsInternal::val(&argv);

    const int argc = args.Length();
    std::vector< v8::Handle<v8::Value> > av(static_cast<size_t>(argc),v8::Undefined());

    for( int i = 0; i < argc; ++i )
      {
      av[i] = args[i];
      }

    const FunctionInternal* func = FunctionInternal::val(fn);
    *result = fromHandle(func->_object->NewInstance(argc, &av[0]));
    }

  void beginFunctionScope(Function::Scope *sc) X_OVERRIDE
    {
    v8::Locker l;
    v8::Handle<v8::Context> ctxt = getV8EngineInternal();

    v8::HandleScope handle_scope;
    v8::Context::Scope scope(ctxt);
    }

  void endFunctionScope(Function::Scope *sc) X_OVERRIDE
    {
    }
  };


EngineInterface *createV8Interface(bool debugging)
  {
  return new JavascriptEngineInterface(debugging);
  }

}

#endif

