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

  typedef XScriptValue (*Function)( XScriptArguments const & argv );

#ifndef X_DART
  XScriptObject get(const QString& name);

  void set(const QString& in, const XScriptObject& obj);
  void set(const QString& in, Function fn);
#endif

  void addInterface(const XInterfaceBase *i);
  };

#endif // XSCRIPTENGINE_H
