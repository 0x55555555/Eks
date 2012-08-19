#ifndef XSCRIPTENGINE_H
#define XSCRIPTENGINE_H

#include "XScriptGlobal.h"
#include "QVariant"

class XScriptValue;
class XPersistentScriptValue;
class XScriptArguments;
class XScriptObject;
class XScriptFunction;
class XInterfaceBase;

namespace XScript
{

class FunctionScope;

class EngineInterface
  {
public:
  virtual void newValue(XScriptValue* val) = 0;
  virtual void newValue(XScriptValue* val, bool c) = 0;
  virtual void newValue(XScriptValue* val, xint32 i) = 0;
  virtual void newValue(XScriptValue* val, xuint32 i) = 0;
  virtual void newValue(XScriptValue* val, xint64 i) = 0;
  virtual void newValue(XScriptValue* val, xuint64 i) = 0;
  virtual void newValue(XScriptValue* val, float i) = 0;
  virtual void newValue(XScriptValue* val, double i) = 0;
  virtual void newValue(XScriptValue* val, const QString& i) = 0;
  virtual void newValue(XScriptValue* val, void *i) = 0;
  virtual void newValue(XScriptValue* val, QVariantMap &i) = 0;
  virtual void newValue(XScriptValue* val, QVariantList &i) = 0;
  virtual void newValue(XScriptValue* val, QStringList &i) = 0;
  virtual void newValue(XScriptValue* val, const XScriptObject &i) = 0;
  virtual void newValue(XScriptValue* val, const XScriptFunction &i) = 0;

  virtual void newArray(XScriptValue* val) = 0;
  virtual void newEmpty(XScriptValue* val) = 0;

  virtual void destroy(XScriptValue *val) = 0;

  virtual bool isValid(const XScriptValue *val) = 0;
  virtual bool isObject(const XScriptValue *val) = 0;
  virtual bool isBoolean(const XScriptValue *val) = 0;
  virtual bool isNumber(const XScriptValue *val) = 0;
  virtual bool isString(const XScriptValue *val) = 0;
  virtual bool isInteger(const XScriptValue *val) = 0;
  virtual bool isArray(const XScriptValue *val) = 0;

  virtual bool toBoolean(const XScriptValue *val) = 0;
  virtual double toNumber(const XScriptValue *val) = 0;
  virtual xint64 toInteger(const XScriptValue *val) = 0;
  virtual void toString(QString *, const XScriptValue *val) = 0;
  virtual void toMap(QVariantMap *, const XScriptValue *val) = 0;
  virtual void toList(QVariantList *, const XScriptValue *val) = 0;
  virtual void *toExternal(const XScriptValue *val) = 0;

  virtual xsize length(const XScriptValue *val) = 0;
  virtual void at(XScriptValue *out, const XScriptValue *val, xsize idx) = 0;
  virtual void set(XScriptValue *val, xsize idx, const XScriptValue &) = 0;

  typedef void (*WeakDtor)(XPersistentScriptValue object, void* p);
  virtual void newPersistentValue(XPersistentScriptValue *value) = 0;
  virtual void newPersistentValue(XPersistentScriptValue *value, const XScriptValue &) = 0;
  virtual void asValue(XScriptValue *out, const XPersistentScriptValue *value) = 0;
  virtual void makeWeak(XPersistentScriptValue *val, void *data, WeakDtor cb) = 0;
  virtual void dispose(XPersistentScriptValue *val) = 0;

  virtual void newFunction(XScriptFunction *fn) = 0;
  virtual void newFunction(XScriptFunction *fn, const XScriptValue &) = 0;
  virtual void newFunction(XScriptFunction *fn, const XScriptFunction &) = 0;

  virtual void destroy(XScriptFunction *) = 0;

  virtual bool isValid(const XScriptFunction *) = 0;

  virtual void call(const XScriptFunction *fn,
                    XScriptValue *out,
                    const XScriptObject &self,
                    int argc,
                    const XScriptValue *args,
                    bool *error,
                    QString *message) = 0;

  virtual void callAsConstructor(const XScriptFunction *fn,
                                 XScriptValue *result,
                                 const XScriptArguments &argv) = 0;

  virtual void beginFunctionScope(FunctionScope *sc);
  virtual void endFunctionScope(FunctionScope *sc);
  };

EngineInterface *currentInterface();

}

class EKSSCRIPT_EXPORT XScriptEngine
  {
public:
  XScriptEngine(bool enableDebugging);
  ~XScriptEngine();

  static void adjustAmountOfExternalAllocatedMemory(int in);

  static void initiate();
  static void terminate();

  void addInterface(const XInterfaceBase *i);
  void removeInterface(const XInterfaceBase *i);
  };

#endif // XSCRIPTENGINE_H
