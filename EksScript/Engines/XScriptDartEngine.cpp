#include "../XScriptEngine.h"
#include "../XScriptValueDartInternals.h"
#include "../XScriptValue.h"
#include "../XScriptObject.h"
#include "../XScriptFunction.h"
#include "../XConvertFromScript.h"
#include "XAssert"

namespace
{
#define WRAPPER_NAME "NativeWrapper"

Dart_Handle& getDartHandle(const XScriptValue *obj)
  {
  struct Internal
    {
    Dart_Handle ptr;
    };

  return ((Internal*)(obj))->ptr;
  }

Dart_Handle& getDartHandle(const XPersistentScriptValue *obj)
  {
  struct Internal
    {
    Dart_Handle ptr;
    };

  return ((Internal*)(obj))->ptr;
  }

Dart_Handle& getDartHandle(const XScriptFunction &obj)
  {
  struct Internal
    {
    Dart_Handle ptr;
    };
  xCompileTimeAssert(sizeof(Internal) == sizeof(XScriptFunction));

  return ((Internal*)(&obj))->ptr;
  }

}

Dart_Handle getDartInternal(const XScriptFunction& v)
  {
  return getDartHandle(v);
  }

void XScriptDartArguments::setReturnValue(const XScriptValue& val)
  {
  Dart_SetReturnValue((Dart_NativeArguments)_args, getDartInternal(val));
  }

XScriptDartArguments::XScriptDartArguments()
  {
  }

xsize XScriptDartArgumentsNoThis::length() const
  {
  return Dart_GetNativeArgumentCount((Dart_NativeArguments)_args._args) - _offset;
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
  return fromObjectHandle(Dart_GetNativeArgument((Dart_NativeArguments)_args._args, 0));
  }


namespace XScript
{

xCompileTimeAssert(sizeof(XScriptValue) == sizeof(Dart_Handle));

Dart_Handle *getDartInternal(const XScriptValue *o)
  {
  return (Dart_Handle*)o;
  }

Dart_Handle *getDartInternal(const XScriptValue &o)
  {
  return (Dart_Handle*)&o;
  }

XScriptValue fromHandle(Dart_Handle h)
  {
  XScriptValue v;
  getDartHandle(&v) = h;
  return v;
  }

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

Dart_Handle loadLibrary(Dart_Handle url, Dart_Handle libSrc)
{
  Dart_Handle lib = Dart_LoadLibrary(url, libSrc);

  CHECK_HANDLE(lib)
  Dart_SetNativeResolver(lib, Resolve);

  Dart_CreateNativeWrapperClass(lib,
    Dart_NewString(WRAPPER_NAME),
    kNumEventHandlerFields);

  return lib;
}

QMap<QString, Dart_Handle> _libs;
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
    QString strUrl = XScriptConvert::from<QString>(fromHandle(url));
    xAssert(_libs.find(strUrl) != _libs.end());
    Dart_Handle source = _libs[strUrl];
    importLibrary = loadLibrary(url, source);
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

  void addInterface(const XInterfaceBase *i)
    {
    QString parentName = i->parent() ? i->parent()->typeName() : WRAPPER_NAME;
    QString src = getDartSource(i, parentName);

    if(i->parent())
      {
      src = "#import(\"" + parentName + "\");\n" + src;
      }

    QString url = getDartUrl(i);
    src = "#library(\"" + url + "\");\n" + src;

    _libs[url] = getDartInternal(XScriptConvert::to(src));
    }

  void removeInterface(const XInterfaceBase *ifc)
    {
    (void)ifc;
    xAssertFail();
    }

  void newValue(XScriptValue *v) X_OVERRIDE
    {
    getDartHandle(v) = Dart_Null();
    }

  void newValue(XScriptValue *v, bool x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewBoolean(x);
    }

  void newValue(XScriptValue *v, xuint32 x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewInteger(x);
    }

  void newValue(XScriptValue *v, xint32 x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewInteger(x);
    }

  void newValue(XScriptValue *v, xuint64 x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewInteger(x);
    }

  void newValue(XScriptValue *v, xint64 x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewInteger(x);
    }

  void newValue(XScriptValue *v, double x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewDouble(x);
    }

  void newValue(XScriptValue *v, float x) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewDouble(x);
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
    if(str.length())
      {
      getDartHandle(v) = Dart_NewString16((const uint16_t*)str.constData(), str.length());
      }
    else
      {
      getDartHandle(v) = Dart_NewString("");
      }
    }

  void newValue(XScriptValue *v, const XScriptObject &obj) X_OVERRIDE
    {
    getDartHandle(v) = getDartInternal(obj);
    }

  void newValue(XScriptValue *v, const XScriptFunction &obj) X_OVERRIDE
    {
    getDartHandle(v) = getDartInternal(obj);
    }

  void newValue(XScriptValue *v, void* val) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewInteger((int64_t)val);
    }

  void destroy(XScriptValue* v) X_OVERRIDE
    {
    getDartHandle(v) = Dart_Null();
    }


  void newEmpty(XScriptValue *v) X_OVERRIDE
    {
    getDartHandle(v) = 0;
    }

  void newArray(XScriptValue *v) X_OVERRIDE
    {
    getDartHandle(v) = Dart_NewList(0);
    }

  bool isValid(const XScriptValue *v) X_OVERRIDE
    {
    Dart_Handle h = getDartHandle(v);
    return h != 0 && !Dart_IsNull(h);
    }

  bool isObject(const XScriptValue *v) X_OVERRIDE
    {
    // everything is an object!
    // probably not so useful.
    return isValid(v);
    }

  bool isBoolean(const XScriptValue *v) X_OVERRIDE
    {
    return Dart_IsBoolean(getDartHandle(v));
    }

  bool isArray(const XScriptValue *v) X_OVERRIDE
    {
    return Dart_IsList(getDartHandle(v));
    }

  bool isNumber(const XScriptValue *v) X_OVERRIDE
    {
    return Dart_IsNumber(getDartHandle(v));
    }

  bool isString(const XScriptValue *v) X_OVERRIDE
    {
    return Dart_IsString(getDartHandle(v));
    }

  bool isInteger(const XScriptValue *v) X_OVERRIDE
    {
    return Dart_IsInteger(getDartHandle(v));
    }

  xsize length(const XScriptValue *v) X_OVERRIDE
    {
    intptr_t len = 0;
    Dart_ListLength(getDartHandle(v), &len);
    return len;
    }

  void at(XScriptValue* out, const XScriptValue *v, xsize id) X_OVERRIDE
    {
    getDartHandle(out) = Dart_ListGetAt(getDartHandle(v), id);
    }

  void set(XScriptValue *v, xsize id, const XScriptValue &val) X_OVERRIDE
    {
    Dart_ListSetAt(getDartHandle(v), id, getDartHandle(&val));
    }

  void *toExternal(const XScriptValue *v) X_OVERRIDE
    {
    return (void*)toInteger(v);
    }

  double toNumber(const XScriptValue *v) X_OVERRIDE
    {
    double dbl = 0.0f;
    Dart_DoubleValue(getDartHandle(v), &dbl);
    return dbl;
    }

  xint64 toInteger(const XScriptValue *v) X_OVERRIDE
    {
    xint64 igr = 0;
    Dart_IntegerToInt64(getDartHandle(v), &igr);
    return igr;
    }

  bool toBoolean(const XScriptValue *v) X_OVERRIDE
    {
    bool b = 0;
    Dart_BooleanValue(getDartHandle(v), &b);
    return b;
    }

  void toString(QString *val, const XScriptValue *v) X_OVERRIDE
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

  void toList(QVariantList *, const XScriptValue *) X_OVERRIDE
    {
    }

  void toMap(QVariantMap *out, const XScriptValue *) X_OVERRIDE
    {
    xAssertFail();
    *out = QVariantMap();
    }

  void newPersistentValue(XPersistentScriptValue *value)
    {
    getDartHandle(value) = 0;
    }

  void newPersistentValue(XPersistentScriptValue *value, const XScriptValue &in)
    {
    getDartHandle(value) = Dart_NewPersistentHandle(getDartHandle(&in));
    }

  void asValue(XScriptValue *out, const XPersistentScriptValue *value)
    {
    getDartHandle(out) = getDartHandle(value);
    }

  void makeWeak(XPersistentScriptValue *val, void *data, WeakDtor cb)
    {
    Dart_Handle oldhandle = getDartHandle(val);
    getDartHandle(val) = Dart_NewPrologueWeakPersistentHandle(
                               oldhandle,
                               data,
                               (Dart_WeakPersistentHandleFinalizer)cb);
    }

  void dispose(XPersistentScriptValue *val)
    {
    Dart_DeletePersistentHandle(getDartHandle(val));
    }

  void newFunction(XScriptFunction *)
    {
    }

  void newFunction(XScriptFunction *, const XScriptValue &)
    {
    }

  void newFunction(XScriptFunction *, const XScriptFunction &)
    {
    }

  void destroy(XScriptFunction *)
    {
    }

  bool isValid(const XScriptFunction *)
    {
    return false;
    }

  void call(const XScriptFunction *,
            XScriptValue *,
            const XScriptObject &,
            int,
            const XScriptValue *,
            bool *,
            QString *)
    {
    }

  void callAsConstructor(const XScriptFunction *,
                                 XScriptValue *,
                                 const XScriptArguments &)
    {
    }
  };

EngineInterface *createDartInterface(bool debugging)
  {
  return new DartEngineInterface(debugging);
  }
}
