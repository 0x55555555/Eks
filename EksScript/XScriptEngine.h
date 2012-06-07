#ifndef XSCRIPTENGINE_H
#define XSCRIPTENGINE_H

#include "XScriptGlobal.h"

class XScriptValue;
class XScriptArguments;
class XScriptObject;
class XInterfaceBase;

class EKSSCRIPT_EXPORT XScriptEngine
  {
public:
  XScriptEngine(bool enableDebugging);
  ~XScriptEngine();

  static void adjustAmountOfExternalAllocatedMemory(int in);

  static void initiate();
  static void terminate();

  XScriptValue run(const QString &src);

  void addInterface(const XInterfaceBase *i);
  void removeInterface(const XInterfaceBase *i);
  };

#endif // XSCRIPTENGINE_H
