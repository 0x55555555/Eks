#ifndef XSCRIPTENGINE_H
#define XSCRIPTENGINE_H

#include "XScriptGlobal.h"
#include "QVariant"

class QFile;

namespace XScript
{
class Source;
class SourceError;
class Value;
class PersistentValue;
class Object;
class Function;
class InterfaceBase;
class FunctionScope;

struct ConstructorDef;
struct PropertyDef;
struct FunctionDef;


class EngineInterface
  {
public:
  virtual ~EngineInterface() { }

  virtual bool supportsExtension(const QString &) = 0;

  virtual bool loadSource(Source *src, const QString &key, const QString &data) = 0;
  virtual void runSource(Value *result, const Source *src, SourceError *err) = 0;

  virtual void throwError(Value *ret, const QString &err) = 0;

  virtual void addInterface(const InterfaceBase *ifc) = 0;
  virtual void removeInterface(const InterfaceBase *ifc) = 0;
  virtual void wrapInstance(const InterfaceBase *ifc, Object *scObj, void *object) = 0;
  virtual void unwrapInstance(const InterfaceBase *ifc, Object *scObj) = 0;
  virtual void newInstance(Object *result, const InterfaceBase *ifc, int argc, Value argv[], const QString& name) const = 0;


  virtual void newValue(Value* val) = 0;
  virtual void newValue(Value* val, bool c) = 0;
  virtual void newValue(Value* val, xint32 i) = 0;
  virtual void newValue(Value* val, xuint32 i) = 0;
  virtual void newValue(Value* val, xint64 i) = 0;
  virtual void newValue(Value* val, xuint64 i) = 0;
  virtual void newValue(Value* val, float i) = 0;
  virtual void newValue(Value* val, double i) = 0;
  virtual void newValue(Value* val, const QString& i) = 0;
  virtual void newValue(Value* val, void *i) = 0;
  virtual void newValue(Value* val, const QVariantMap &i) = 0;
  virtual void newValue(Value* val, const QVariantList &i) = 0;
  virtual void newValue(Value* val, const QStringList &i) = 0;
  virtual void newValue(Value* val, const Value *i) = 0;
  virtual void newValue(Value* val, const Object *i) = 0;
  virtual void newValue(Value* val, const Function *i) = 0;

  virtual void newArray(Value* val) = 0;
  virtual void newEmpty(Value* val) = 0;

  virtual void destroy(Value *val) = 0;

  virtual bool isValid(const Value *val) = 0;
  virtual bool isObject(const Value *val) = 0;
  virtual bool isBoolean(const Value *val) = 0;
  virtual bool isNumber(const Value *val) = 0;
  virtual bool isString(const Value *val) = 0;
  virtual bool isInteger(const Value *val) = 0;
  virtual bool isArray(const Value *val) = 0;

  virtual bool toBoolean(const Value *val) = 0;
  virtual double toNumber(const Value *val) = 0;
  virtual xint64 toInteger(const Value *val) = 0;
  virtual void toString(QString *, const Value *val) = 0;
  virtual void toMap(QVariantMap *, const Value *val) = 0;
  virtual void toList(QVariantList *, const Value *val) = 0;
  virtual void *toExternal(const Value *val) = 0;

  virtual xsize length(const Value *val) = 0;
  virtual void at(Value *out, const Value *val, xsize idx) = 0;
  virtual void set(Value *val, xsize idx, const Value *) = 0;

  typedef void (*WeakDtor)(PersistentValue object, void* p);
  virtual void newPersistentValue(PersistentValue *value) = 0;
  virtual void newPersistentValue(PersistentValue *value, const Value &) = 0;
  virtual void asValue(Value *out, const PersistentValue *value) = 0;
  virtual void makeWeak(PersistentValue *val, void *data, WeakDtor cb) = 0;
  virtual void dispose(PersistentValue *val) = 0;

  virtual void newObject(Object *obj) = 0;
  virtual void newObject(Object *obj, const Object *cpy) = 0;
  virtual void newObject(Object *obj, const Value *val) = 0;

  virtual void destroy(Object *obj) = 0;

  virtual void *internalField(const Object *obj, ObjectInternalField idx) = 0;

  virtual bool isValid(const Object *) = 0;

  virtual void newMap(Object *obj) = 0;
  virtual void get(Value *ret, const Object *, const QString &n) = 0;
  virtual void set(Object *, const QString &n, const Value *val) = 0;

  virtual void newFunction(Function *fn) = 0;
  virtual void newFunction(Function *fn, const Value &) = 0;
  virtual void newFunction(Function *fn, const Function &) = 0;

  virtual void destroy(Function *) = 0;

  virtual bool isValid(const Function *) = 0;

  virtual void call(const Function *fn,
                    Value *out,
                    const Object &self,
                    int argc,
                    const Value *args,
                    bool *error,
                    QString *message) = 0;

  virtual void beginFunctionScope(FunctionScope *sc) = 0;
  virtual void endFunctionScope(FunctionScope *sc) = 0;
  };

EKSSCRIPT_EXPORT EngineInterface *currentInterface();

class EKSSCRIPT_EXPORT Engine
  {
public:
  enum
    {
    InterfaceCount = 0
#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
      + 1
#endif
#ifdef X_SCRIPT_ENGINE_ENABLE_DART
      + 1
#endif
    };

  static xsize getIndex(EngineInterface *);

  static void adjustAmountOfExternalAllocatedMemory(int in);

  static void initiate(bool enableDebugging);
  static void initiateQtWrappers();
  static void terminate();

  static EngineInterface *beginScope(EngineInterface *ifc);
  static void endScope(EngineInterface *ifc, EngineInterface *oldIfc);

  static void addInterface(const InterfaceBase *i);
  static void removeInterface(const InterfaceBase *i);

  static EngineInterface *findInterface(const QFile *);
  static EngineInterface *findInterface(const QString &extension);

  class Walker
    {
  public:
    EngineInterface **begin() { return _begin; }
    EngineInterface **end() { return _end; }

    EngineInterface **_begin;
    EngineInterface **_end;
    };

  static Walker interfaces();

private:
  Engine();
  ~Engine();
  };

class EKSSCRIPT_EXPORT EngineScope
  {
public:
  EngineScope(EngineInterface *ifc);
  ~EngineScope();

private:
  EngineInterface *_currentInterface;
  EngineInterface *_oldInterface;
  };

}

#endif // XSCRIPTENGINE_H
