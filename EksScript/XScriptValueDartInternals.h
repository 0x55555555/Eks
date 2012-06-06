#ifndef XSCRIPTVALUEDARTINTERNALS_H
#define XSCRIPTVALUEDARTINTERNALS_H

#include "XScriptGlobal.h"

#include "dart/dart_api.h"
#include "QDebug"

class XScriptValue;
class XScriptObject;
class XInterfaceBase;
class XScriptFunction;
class XScriptEngine;

inline void printError(Dart_Handle han)
  {
  const char *err = Dart_GetError(han);
  if(err)
    {
    qWarning() << err;
    }
  }

XScriptValue fromHandle(Dart_Handle);
XScriptObject fromObjectHandle(Dart_Handle);

#define CHECK_HANDLE(handle) if(Dart_IsError(handle)) { printError(handle); }

QString getDartUrl(const XInterfaceBase* ifc);

Dart_Handle getDartInternal(const XScriptValue&);
Dart_Handle *getDartInternal(const XScriptValue*);
Dart_Handle getDartInternal(const XScriptObject &o);
Dart_Handle getDartInternal(const XInterfaceBase* b);

inline Dart_Handle &getDartHandle(void *&ptr)
  {
  return (Dart_Handle&)ptr;
  }

QString addDartNativeLookup(const QString &typeName, const QString &functionName, xsize argCount, Dart_NativeFunction);

QString getDartSource(const XInterfaceBase *, const QString &parentName);

#endif // XSCRIPTVALUEDARTINTERNALS_H
