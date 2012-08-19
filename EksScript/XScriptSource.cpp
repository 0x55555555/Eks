#include "XScriptSource.h"
#include "XScriptValue.h"
#include "XScriptFunction.h"
#include "XConvertFromScript.h"
#include "XScriptValueV8Internals.h"
#include "XScriptValueDartInternals.h"
#include "XAssert"
#include "XConvertToScript.h"
#include "QFile"

struct XScriptSourceImpl
{
  static XScriptSourceImpl* impl(XScriptSource *s) { return reinterpret_cast<XScriptSourceImpl*>(s); }
  static const XScriptSourceImpl* impl(const XScriptSource *s) { return reinterpret_cast<const XScriptSourceImpl*>(s); }

#ifdef X_DART
  Dart_Handle _script;
#else
  v8::Handle<v8::Script> _script;
#endif
  };

xCompileTimeAssert(sizeof(XScriptSourceImpl) == sizeof(XScriptSource));

XScriptSource::XScriptSource(QFile *file)
  {
  load(file->fileName(), QString::fromUtf8(file->readAll()));
  }

XScriptSource::XScriptSource(const QString &key, const QString &data)
  {
  load(key, data);
  }

void XScriptSource::load(const QString &key, const QString &src)
  {
  XScriptSourceImpl *i = XScriptSourceImpl::impl(this);

#ifdef X_DART
  // Create a test library and Load up a test script in it.
  Dart_Handle source = getDartInternal(XScriptConvert::to(src));
  Dart_Handle url = getDartInternal(XScriptConvert::to(key));
  i->_script = Dart_LoadScript(url, source);
  CHECK_HANDLE(i->_script)

#else
  v8::Locker locker;
  // Create a string containing the JavaScript source code.
  v8::Handle<v8::String> source = v8::String::New((xuint16*)src.constData(), src.length());

  v8::Handle<v8::String> fileNameV8 = v8::String::New((xuint16*)key.constData(), key.length());

  // Compile the source code.
  i->_script = v8::Script::Compile(source, fileNameV8);
#endif
  }

XScriptSource::Error::Error() : _hasError(false)
  {
  }

XScriptValue XScriptSource::run(Error *error)
  {
  // Run the script to get the result.
  XScriptSourceImpl *i = XScriptSourceImpl::impl(this);

#ifdef X_DART

  xAssert(!error);

  Dart_Handle result = Dart_Invoke(i->_script,
    Dart_NewString("main"),
    0,
    0);
  CHECK_HANDLE(result)

  return fromHandle(result);
#else
  v8::Locker locker;
  v8::TryCatch trycatch;
  if(i->_script.IsEmpty())
    {
    error->setHasError(true);
    error->setTrace("Running invalid script");
    return XScriptValue();
    }

  v8::Handle<v8::Value> result = i->_script->Run();

  if(error)
    {
    error->setHasError(false);
    }

  if (trycatch.HasCaught())
    {
    if(error)
      {
      error->setHasError(true);
      error->setTrace(XScriptConvert::from<QString>(fromHandle(trycatch.StackTrace())));

      if(!trycatch.Message().IsEmpty())
        {
        v8::Local<v8::Message> mess = trycatch.Message();

        error->setMessage(XScriptConvert::from<QString>(fromHandle(mess->Get())));
        error->setLineNumber(mess->GetLineNumber());
        }
      }
    return fromHandle(trycatch.Exception());
    }

  return fromHandle(result);
#endif
  }
