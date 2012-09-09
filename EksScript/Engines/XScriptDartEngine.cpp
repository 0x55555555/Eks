#include "../XScriptGlobal.h"

#ifdef X_SCRIPT_ENGINE_ENABLE_DART
# include "../dart/dart_api.h"
#endif

#include "../XScriptEngine.h"
#include "../XScriptValue.h"
#include "../XScriptObject.h"
#include "../XScriptFunction.h"
#include "../XScriptSource.h"
#include "../XConvertFromScript.h"
#include "../XInterface.h"

#include "XAssert"

namespace XScript
{

#ifdef X_SCRIPT_ENGINE_ENABLE_DART

template <typename T> T fromHandle(Dart_Handle h)
  {
  T v;
  getDartHandle(&v) = h;
  return v;
  }

template <typename T> Dart_Handle& getDartHandle(const T *obj)
  {
  struct Internal
    {
    Dart_Handle ptr;
    };

  xCompileTimeAssert(sizeof(T) == sizeof(Internal));

  return ((Internal*)(obj))->ptr;
  }

Dart_Handle& getDartHandle(const Source *obj)
  {
  struct Internal
    {
    EngineInterface *_engine;
    Dart_Handle ptr;
    };

  xCompileTimeAssert(sizeof(Source) == sizeof(Internal));

  return ((Internal*)(obj))->ptr;
  }

template <typename T> Dart_Handle* getDartArrayHandle(const T *obj)
  {
  struct Internal
    {
    Dart_Handle ptr;
    };

  xCompileTimeAssert(sizeof(T) == sizeof(Dart_Handle));

  return &((Internal*)(obj))->ptr;
  }
#endif

namespace internal
{

void DartArguments::setReturnValue(const Value& val)
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_DART
  Dart_SetReturnValue((Dart_NativeArguments)_args, getDartHandle(&val));
#else
  (void)val;
#endif
  }

xsize DartArgumentsNoThis::length() const
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_DART
  return Dart_GetNativeArgumentCount((Dart_NativeArguments)_args._args) - _offset;
#else
  return 0;
#endif
  }

Value DartArgumentsNoThis::at(xsize i) const
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_DART
  return fromHandle<Value>(Dart_GetNativeArgument((Dart_NativeArguments)_args._args, i + _offset));
#else
  (void)i;
  return Value();
#endif
  }

Object DartArgumentsWithThis::calleeThis()
  {
#ifdef X_SCRIPT_ENGINE_ENABLE_DART
  return fromHandle<Object>(Dart_GetNativeArgument((Dart_NativeArguments)_args._args, 0));
#else
  return Object();
#endif
  }

}

}

#ifdef X_SCRIPT_ENGINE_ENABLE_DART

namespace XScript
{

#define WRAPPER_NAME "NATIVE_WRAPPER__"

const int NumEventHandlerFields = 2;

inline void printError(Dart_Handle han)
  {
  const char *err = Dart_GetError(han);
  if(err)
    {
    qWarning() << err;
    }
  }

#define CHECK_HANDLE(handle) if(Dart_IsError(handle)) { printError(handle); }

QString getDartUrl(const InterfaceBase* i)
  {
  return i->typeName();
  }

#if 0
Object fromObjectHandle(Dart_Handle v)
  {
  Object o;
  getDartHandle(&o) = v;
  return o;
  }
#endif

bool isolateCreateCallback(const char* ,
  const char* ,
  void* ,
  char** )
{
  xAssertFail();
  return true;
}

bool isolateInterruptCallback()
{
  xAssertFail();
  return true;
}

Dart_Handle getDartClass(const InterfaceBase *ifc)
  {
  Value urlVal = Convert::to(getDartUrl(ifc));
  Dart_Handle lib = Dart_LookupLibrary(getDartHandle(&urlVal));

  Value typenameVal = Convert::to(ifc->typeName());

  return Dart_GetClass(lib, getDartHandle(&typenameVal));
  }

typedef QPair<QString, uint8_t> ArgPair;
QMap<ArgPair, Dart_NativeFunction> _symbols;

Dart_NativeFunction Resolve(Dart_Handle name, int num_of_arguments)
{
  const char* cname;
  Dart_StringToCString(name, &cname);

  ArgPair toFind(ArgPair(cname, num_of_arguments));

  xAssert(_symbols.find(toFind) != _symbols.end());
  return _symbols[toFind];
}

QString addDartNativeLookup(const QString &typeName, const QString &functionName, xsize argCount, Dart_NativeFunction fn)
{
  QString fullName = typeName + "_" + functionName + QString::number(argCount);
  _symbols[ArgPair(fullName, argCount)] = fn;

  return fullName;
}

QMap<QString, QString> _libs;
Dart_Handle loadLibrary(Dart_Handle url)
{
  QString strUrl = Convert::from<QString>(fromHandle<Value>(url));

  QString source;
  if(strUrl == WRAPPER_NAME)
    {
    source = "#library(\"" WRAPPER_NAME "\");\n";
    }
  else
    {
    xAssert(_libs.find(strUrl) != _libs.end());
    source = _libs[strUrl];
    }

  Value srcVal(source);
  Dart_Handle dartSource = getDartHandle(&srcVal);

  Dart_Handle lib = Dart_LoadLibrary(url, dartSource);

  CHECK_HANDLE(lib)
  Dart_SetNativeResolver(lib, Resolve);

  if(strUrl == WRAPPER_NAME)
    {
    Dart_CreateNativeWrapperClass(lib,
      Dart_NewString(WRAPPER_NAME),
      NumEventHandlerFields);
    }

  return lib;
}

Dart_Handle tagHandler(Dart_LibraryTag tag, Dart_Handle library, Dart_Handle url)
{
  if (!Dart_IsLibrary(library)) {
    return Dart_Error("not a library");
  }
  if (!Dart_IsString8(url)) {
    return Dart_Error("url is not a string");
  }

  if (tag == kCanonicalizeUrl) {
    return url;
  }

  Dart_Handle importLibrary = Dart_LookupLibrary(url);
  if (Dart_IsError(importLibrary))
  {
    importLibrary = loadLibrary(url);
  }

  if (!Dart_IsError(importLibrary) && (tag == kImportTag))
  {
    Dart_Handle result = Dart_LibraryImportLibrary(library, importLibrary);
    CHECK_HANDLE(result);
  }

  return importLibrary;
}


class DartEngineInterface : public EngineInterface
  {
public:
  DartEngineInterface(bool debugging)
    {
    xAssert(!debugging);

    Dart_SetVMFlags(0, (const char**)0);
    Dart_Initialize(isolateCreateCallback, isolateInterruptCallback);

    // Start an Isolate, load a script and create a full snapshot.
    Dart_CreateIsolate(0, 0, 0, 0, 0);

    Dart_EnterScope();
    Dart_SetLibraryTagHandler(tagHandler);
    }

  ~DartEngineInterface()
    {
    Dart_ExitScope();
    Dart_ShutdownIsolate();
    }

  bool supportsExtension(const QString &ext)
    {
    return ext == "dart";
    }

  bool loadSource(Source *src, const QString &key, const QString &data)
    {
    Value srcVal = Convert::to(data);
    Value urlVal = Convert::to(key);
    // Create a test library and Load up a test script in it.
    Dart_Handle source = getDartHandle(&srcVal);
    Dart_Handle url = getDartHandle(&urlVal);

    Dart_Handle &hnd = getDartHandle(src);

    hnd = Dart_LoadScript(url, source);

    if(Dart_IsError(hnd))
      {
      printError(hnd);
      return false;
      }

    return true;
    }

  void runSource(Value *result, const Source *src, SourceError *err)
    {
    xAssert(!err);

    Dart_Handle &hnd = getDartHandle(result);

    hnd = Dart_Invoke(getDartHandle(src),
      Dart_NewString("main"),
      0,
      0);
    CHECK_HANDLE(hnd)
    }

  void throwError(Value *val, const QString &err)
    {
    Dart_ThrowException(Dart_NewString16((uint16_t*)err.data(), err.length()));
    getDartHandle(val) = 0;
    }

  void addInterface(const InterfaceBase *i)
    {
    QString parentName = i->parent() ? i->parent()->typeName() : WRAPPER_NAME;
    QString functionSource;

    QString typeName = i->typeName();

    for(xsize c = 0; c < i->constructorCount(); ++c)
      {
      const ConstructorDef &ctor = i->constructor(c);

      QString callArgs;
      for(xuint8 i = 0; i < ctor.argCount; ++i)
        {
        callArgs += "_" + QString::number(i);;
        if(i < (ctor.argCount-1))
          {
          callArgs += ",";
          }
        }

      QString name = typeName;
      QString shortName(ctor.name);
      if(shortName.length())
        {
        name = typeName + "." + shortName;
        }

      // plus one for this.
      QString nativeName = addDartNativeLookup(typeName, "_ctor_" + shortName, 1 + ctor.argCount, (Dart_NativeFunction)ctor.functionDart);
      functionSource += name + "(" + callArgs + ") {" + nativeName + "(" + callArgs + "); } \n" +
        "void " + nativeName + "(" + callArgs + ") native \"" + nativeName + "\";\n";
      }

    for(xuint8  p = 0; p < i->propertyCount(); ++p)
      {
      const PropertyDef &prop = i->property(p);

      QString name = prop.name;
      if(prop.getterDart)
        {
        // 1 argument (this)
        QString resolvedName = addDartNativeLookup(typeName, name, 1, (Dart_NativeFunction)prop.getterDart);

        functionSource += "Dynamic get " + name + "() => _get" + name + "();\n"
                          "Dynamic _get" + name + "() native \"" + resolvedName + "\";\n";
        }
      if(prop.setterDart)
        {
        // 1 argument (this and value)
        QString resolvedName = addDartNativeLookup(typeName, name, 2, (Dart_NativeFunction)prop.setterDart);

        functionSource += "set " + name + "(var a) => _set" + name + "(a);\n"
                          "Dynamic _set" + name + "(var a) native \"" + resolvedName + "\";\n";
        }
      }

    for(xsize f = 0; f < i->functionCount(); ++f)
      {
      const FunctionDef &fn = i->function(f);

      QString callArgs;
      for(xuint8 i = 0; i < fn.argCount; ++i)
        {
        callArgs += "_" + QString::number(i);;
        if(i < (fn.argCount-1))
          {
          callArgs += ",";
          }
        }

      const char* leadIn = "Dynamic ";
      xsize extraArgs = 1;
      if(fn.isStatic)
        {
        leadIn = "static Dynamic ";
        extraArgs = 0;
        }

      QString name = fn.name;
      QString resolvedName = addDartNativeLookup(typeName, name, extraArgs + fn.argCount, (Dart_NativeFunction)fn.functionDart);
      functionSource += leadIn + name + "(" + callArgs + ") native \"" + resolvedName + "\";\n";
      }

    
    functionSource = "class " + i->typeName() +
      " extends " + parentName + " {\n" +
      functionSource +
      "}";

    if(i->parent())
      {
      functionSource = "#import(\"" + parentName + "\");\n" + functionSource;
      }

    QString url = getDartUrl(i);
    functionSource = "#library(\"" + url + "\");\n" 
                     "#import(\"" WRAPPER_NAME "\");\n" + 
                      functionSource;

    _libs[url] = functionSource;
    }

  void removeInterface(const InterfaceBase *ifc)
    {
    (void)ifc;
    xAssertFail();
    }

  void wrapInstance(const InterfaceBase *ifc, Object *scObj, void *object)
    {
    Dart_Handle &obj = getDartHandle(scObj);
    Dart_SetNativeInstanceField(obj, TypeId, (intptr_t)ifc->baseTypeId());
    Dart_SetNativeInstanceField(obj, NativePointer, (intptr_t)object);
    }

  void unwrapInstance(const InterfaceBase *, Object *scObj)
    {
    Dart_Handle &obj = getDartHandle(scObj);

    Dart_SetNativeInstanceField(obj, TypeId, (intptr_t)0);
    Dart_SetNativeInstanceField(obj, NativePointer, (intptr_t)0);
    }

  void newInstance(Object *result, const InterfaceBase *ifc, int argc, Value argv[], const QString& name) const
    {
    Dart_Handle cls = getDartClass(ifc);

    Value nameVal = Convert::to(name);
    Dart_Handle obj = Dart_New(cls, getDartHandle(&nameVal), argc, getDartArrayHandle(argv));
    getDartHandle(result) = obj;
    }

  void newValue(Value *v) X_OVERRIDE
    {
    getDartHandle(v) = Dart_Null();
    }

  void newValue(Value *v, bool x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewBoolean(x);
    }

  void newValue(Value *v, xuint32 x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewInteger(x);
    }

  void newValue(Value *v, xint32 x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewInteger(x);
    }

  void newValue(Value *v, xuint64 x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewInteger(x);
    }

  void newValue(Value *v, xint64 x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewInteger(x);
    }

  void newValue(Value *v, double x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewDouble(x);
    }

  void newValue(Value *v, float x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewDouble(x);
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
    if(str.length())
      {
      getDartHandle(v) = Dart_NewString16((const uint16_t*)str.constData(), str.length());
      }
    else
      {
      getDartHandle(v) = Dart_NewString("");
      }
    }

  void newValue(Value *v, const Value *obj) X_OVERRIDE
    {
    getDartHandle(v) = getDartHandle(obj);
    }

  void newValue(Value *v, const Object *obj) X_OVERRIDE
    {
    getDartHandle(v) = getDartHandle(obj);
    }

  void newValue(Value *v, const Function *obj) X_OVERRIDE
    {
    getDartHandle(v) = getDartHandle(obj);
    }

  void newValue(Value *v, void* val) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewInteger((int64_t)val);
    }

  void destroy(Value* v) X_OVERRIDE
    {
    getDartHandle(v) = Dart_Null();
    }


  void newEmpty(Value *v) X_OVERRIDE
    {
    getDartHandle(v) = 0;
    }

  void newArray(Value *v) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewList(0);
    }

  bool isValid(const Value *v) X_OVERRIDE
    {
    Dart_Handle h = getDartHandle(v);
    return h != 0 && !Dart_IsNull(h);
    }

  bool isObject(const Value *v) X_OVERRIDE
    {
    // everything is an object!
    // probably not so useful.
    return isValid(v);
    }

  bool isBoolean(const Value *v) X_OVERRIDE
    {
    return Dart_IsBoolean(getDartHandle(v));
    }

  bool isArray(const Value *v) X_OVERRIDE
    {
    return Dart_IsList(getDartHandle(v));
    }

  bool isNumber(const Value *v) X_OVERRIDE
    {
    return Dart_IsNumber(getDartHandle(v));
    }

  bool isString(const Value *v) X_OVERRIDE
    {
    return Dart_IsString(getDartHandle(v));
    }

  bool isInteger(const Value *v) X_OVERRIDE
    {
    return Dart_IsInteger(getDartHandle(v));
    }

  xsize length(const Value *v) X_OVERRIDE
    {
    intptr_t len = 0;
    Dart_ListLength(getDartHandle(v), &len);
    return len;
    }

  void at(Value* out, const Value *v, xsize id) X_OVERRIDE
    {
    getDartHandle(out) = Dart_ListGetAt(getDartHandle(v), id);
    }

  void set(Value *v, xsize id, const Value *val) X_OVERRIDE
    {
    Dart_ListSetAt(getDartHandle(v), id, getDartHandle(val));
    }

  void *toExternal(const Value *v) X_OVERRIDE
    {
    return (void*)toInteger(v);
    }

  double toNumber(const Value *v) X_OVERRIDE
    {
    double dbl = 0.0f;
    Dart_DoubleValue(getDartHandle(v), &dbl);
    return dbl;
    }

  xint64 toInteger(const Value *v) X_OVERRIDE
    {
    xint64 igr = 0;
    Dart_IntegerToInt64(getDartHandle(v), &igr);
    return igr;
    }

  bool toBoolean(const Value *v) X_OVERRIDE
    {
    bool b = 0;
    Dart_BooleanValue(getDartHandle(v), &b);
    return b;
    }

  void toString(QString *val, const Value *v) X_OVERRIDE
    {
    intptr_t len;
    Dart_Handle str = getDartHandle(v);
    if(!Dart_IsString(str))
      {
      str = Dart_ToString(str);
      }

    Dart_StringLength(str, &len);
    if(len > 0)
      {
      val->resize(len);
      Dart_StringGet16(str, (uint16_t*)val->data(), &len);
      }
    }

  void toList(QVariantList *, const Value *) X_OVERRIDE
    {
    }

  void toMap(QVariantMap *out, const Value *) X_OVERRIDE
    {
    xAssertFail();
    *out = QVariantMap();
    }

  void newPersistentValue(PersistentValue *value)
    {
    getDartHandle(value) = 0;
    }

  void newPersistentValue(PersistentValue *value, const Value &in)
    {
    getDartHandle(value) = Dart_NewPersistentHandle(getDartHandle(&in));
    }

  void asValue(Value *out, const PersistentValue *value)
    {
    getDartHandle(out) = getDartHandle(value);
    }

  void makeWeak(PersistentValue *val, void *data, WeakDtor cb)
    {
    Dart_Handle oldhandle = getDartHandle(val);
    getDartHandle(val) = Dart_NewPrologueWeakPersistentHandle(
                               oldhandle,
                               data,
                               (Dart_WeakPersistentHandleFinalizer)cb);
    }

  void dispose(PersistentValue *val)
    {
    Dart_DeletePersistentHandle(getDartHandle(val));
    }

  void newObject(Object *obj)
    {
    getDartHandle(obj) = 0;
    }

  void newObject(Object *obj, const Value *v)
    {
    getDartHandle(obj) = getDartHandle(v);
    }

  void newObject(Object *obj, const Object *o)
    {
    getDartHandle(obj) = getDartHandle(o);
    }

  void destroy(Object *obj)
    {
    getDartHandle(obj) = 0;
    }

  void *internalField(const Object *obj, ObjectInternalField idx)
    {
    intptr_t val = 0;
    Dart_GetNativeInstanceField(getDartHandle(obj),
                                idx,
                                &val);
    return (void*)val;
    }

  bool isValid(const Object *o)
    {
    return getDartHandle(o) != 0 && !Dart_IsNull(getDartHandle(o));
    }

  void newMap(Object *obj)
    {
    (void)obj;
    xAssertFail();
    }

  void get(Value *ret, const Object *obj, const QString &n)
    {
    Value key = Convert::to(n);

    getDartHandle(ret) = Dart_GetField(getDartHandle(obj), getDartHandle(&key));
    }

  void set(Object *obj, const QString &n, const Value *val)
    {
    Value key = Convert::to(n);

    Dart_SetField(getDartHandle(obj), getDartHandle(&key), getDartHandle(val));
    }

  void newFunction(Function *)
    {
    }

  void newFunction(Function *, const Value &)
    {
    }

  void newFunction(Function *, const Function &)
    {
    }

  void destroy(Function *)
    {
    }

  bool isValid(const Function *)
    {
    return false;
    }

  void call(const Function *,
            Value *,
            const Object &,
            int,
            const Value *,
            bool *,
            QString *)
    {
    }


  void beginFunctionScope(FunctionScope *sc)
    {
    }

  void endFunctionScope(FunctionScope *sc)
    {
    }
  };


EngineInterface *createDartInterface(bool debugging)
  {
  return new DartEngineInterface(debugging);
  }
}

#endif
