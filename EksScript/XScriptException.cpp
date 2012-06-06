#include "XScriptException.h"
#include "XScriptValue.h"
#include "XScriptValueV8Internals.h"
#include "XScriptValueDartInternals.h"

inline XScriptValue TossAsError(const QString &err)
  {
#ifdef X_DART
  Dart_ThrowException(Dart_NewString16((uint16_t*)err.data(), err.length()));
  return XScriptValue();
#else
  v8::Handle<v8::String> string = v8::String::New(err.toUtf8().data());
  return fromHandle(v8::ThrowException(v8::Exception::Error(string)));

#endif
  }

inline XScriptValue Toss(const QString &err)
  {
#ifdef X_DART
  Dart_ThrowException(Dart_NewString16((uint16_t*)err.data(), err.length()));
  return XScriptValue();
#else
  v8::Handle<v8::String> string = v8::String::New(err.toUtf8().data());
  return fromHandle(v8::ThrowException(string));
#endif
  }
