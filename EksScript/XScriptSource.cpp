#include "XScriptSource.h"
#include "XScriptValue.h"
#include "XScriptFunction.h"
#include "XConvertFromScript.h"
#include "XScriptValueV8Internals.h"
#include "XAssert"

struct XScriptSourceImpl
{
  static XScriptSourceImpl* impl(XScriptSource *s) { return reinterpret_cast<XScriptSourceImpl*>(s); }
  static const XScriptSourceImpl* impl(const XScriptSource *s) { return reinterpret_cast<const XScriptSourceImpl*>(s); }

  v8::Handle<v8::Script> _script;
  };

xCompileTimeAssert(sizeof(XScriptSourceImpl) == sizeof(XScriptSource));

XScriptSource::XScriptSource(const QString &filename, const QString &data)
  {
  v8::Locker locker;
  // Create a string containing the JavaScript source code.
  v8::Handle<v8::String> source = v8::String::New((xuint16*)data.constData(), data.length());

  v8::Handle<v8::String> fileNameV8 = v8::String::New((xuint16*)filename.constData(), filename.length());

  // Compile the source code.
  XScriptSourceImpl *i = XScriptSourceImpl::impl(this);
  i->_script = v8::Script::Compile(source, fileNameV8);
  }

XScriptSource::Error::Error() : _hasError(false)
  {
  }

XScriptValue XScriptSource::run(Error *error)
  {
  v8::Locker locker;
  v8::TryCatch trycatch;
  // Run the script to get the result.
  XScriptSourceImpl *i = XScriptSourceImpl::impl(this);
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
  }
