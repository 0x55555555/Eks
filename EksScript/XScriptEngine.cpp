#include "XScriptValueDartInternals.h"
#include "XScriptValueV8Internals.h"

#ifndef X_DART
# include "v8-debug.h"
#endif

#include "XScriptGlobal.h"
#include "XScriptEngine.h"
#include "XScriptValue.h"
#include "XInterface.h"
#include "XConvertToScript.h"
#include "XQObjectWrapper.h"
#include "XQtWrappers.h"

#define WRAPPER_NAME "NativeWrapper"

#ifdef X_DART
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

#else

void debugHandler() {
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
#endif

struct StaticEngine
  {
    StaticEngine()
#ifndef X_DART
       : globalTemplate(v8::ObjectTemplate::New()),
      context(v8::Context::New(NULL, globalTemplate)),
      contextScope(context)
#endif
    {
#ifdef X_DART
        Dart_SetVMFlags(0, (const char**)0);
        Dart_Initialize(isolateCreateCallback, isolateInterruptCallback);

        // Start an Isolate, load a script and create a full snapshot.
        Dart_CreateIsolate(0, 0, 0, 0, 0);

        Dart_EnterScope();
        Dart_SetLibraryTagHandler(tagHandler);
#else
        context->AllowCodeGenerationFromStrings(false);
#endif
    }
  ~StaticEngine()
  {
#ifdef X_DART
    Dart_ExitScope();
    Dart_ShutdownIsolate();
#else
    v8::V8::LowMemoryNotification();
    context.Dispose();
#endif
    }

#ifndef X_DART
  v8::Locker locker;
  v8::HandleScope scope;
  v8::Handle<v8::ObjectTemplate> globalTemplate;
  v8::Persistent<v8::Context> context;
  v8::Context::Scope contextScope;
  v8::Unlocker unlocker;
#endif
  };

StaticEngine *g_engine = 0;

void fatal(const char* location, const char* message)
  {
  qFatal("%s: %s", location, message);
  }

void XScriptEngine::initiate()
  {
  if(g_engine)
  {
    return;
  }

  g_engine = new StaticEngine();
  }

void XScriptEngine::terminate()
  {
  delete g_engine;
  }

XScriptEngine::XScriptEngine(bool debugging)
  {
  XQObjectWrapper::initiate(this);
  XQtWrappers::initiate(this);
#ifdef X_DART
  xAssert(!debugging);
#else

  v8::V8::SetFatalErrorHandler(fatal);
  if(debugging)
    {
    v8::Debug::EnableAgent("XScriptAgent", 9222, false);
    v8::Debug::SetDebugMessageDispatchHandler(debugHandler, true);
    }
#endif
  }

XScriptEngine::~XScriptEngine()
  {
  }
QString getDartUrl(const XInterfaceBase* i)
  {
  return i->typeName();
  }

void XScriptEngine::addInterface(const XInterfaceBase *i)
  {
#ifdef X_DART
  QString parentName = i->parent() ? i->parent()->typeName() : WRAPPER_NAME;
  QString src = getDartSource(i, parentName);

  if(i->parent())
    {
    src = "#import(\"" + parentName + "\");\n" + src;
    }

  QString url = getDartUrl(i);
  src = "#library(\"" + url + "\");\n" + src;

  _libs[url] = getDartInternal(XScriptConvert::to(src));
#else
  xAssert(i->isSealed());
  i->addClassTo(i->typeName(), fromHandle(g_engine->context->Global()));
#endif
  }

void XScriptEngine::removeInterface(const XInterfaceBase *i)
  {
#ifdef X_DART
  xAssertFail();
#else
  xAssert(i->isSealed());
  fromObjectHandle(g_engine->context->Global()).set(i->typeName(), XScriptValue());
#endif
  }

void XScriptEngine::adjustAmountOfExternalAllocatedMemory(int in)
  {
#ifdef X_DART
  (void)in;
#else
  v8::V8::AdjustAmountOfExternalAllocatedMemory(in);
#endif
  }

#ifndef X_DART
v8::Handle<v8::ObjectTemplate> getGlobalTemplate(XScriptEngine *)
  {
  return g_engine->globalTemplate;
  }

v8::Handle<v8::Context> getV8EngineInternal()
  {
  return g_engine->context;
  }
#endif
