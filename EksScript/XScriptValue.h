#ifndef XSCRIPTVALUE_H
#define XSCRIPTVALUE_H

#include "XProperty"
#include "XScriptGlobal.h"

class QVariant;

namespace XScript
{

class PersistentValue;
class Object;
class Function;

class EKSSCRIPT_EXPORT Value
  {
public:
  Value();
  explicit Value(bool x);
  Value(xuint32 x);
  Value(xint32 x);
  Value(xuint64 x);
  Value(xint64 x);
  Value(double x);
  Value(float x);
  Value(const QString &str);
  Value(const Object &str);
  Value(const Function &str);
  explicit Value(const QVariant& val);
  explicit Value(void* val);
  ~Value();

  Value(const Value&);
  Value& operator=(const Value&);

  void clear();

  bool isValid() const;
  bool isObject() const;
  bool isArray() const;
  bool isBoolean() const;
  bool isNumber() const;
  bool isInteger() const;
  bool isString() const;

  xsize length() const;
  Value at(xsize id) const;
  void set(xsize id, const Value &);

  void *toExternal() const;
  double toNumber() const;
  xint64 toInteger() const;
  bool toBoolean() const;
  QString toString() const;
  QVariant toVariant(int typeHint=0) const;

  static Value newArray();
  static Value newEmpty();

private:
  void *_object;
  friend class PersistentValue;
  };

class EKSSCRIPT_EXPORT PersistentValue
  {
public:
  PersistentValue();
  PersistentValue(const Value &val);

  ~PersistentValue()
    {
    }

  Value asValue() const;

  typedef void (*WeakDtor)(PersistentValue object, void* p);
  void makeWeak(void *data, WeakDtor cb);

  void dispose();

private:
  void *_object;
  };


class EngineInterface;

class Callback
  {
XProperties:
  XROProperty(EngineInterface *, engineInterface);

public:
  Callback() : _engineInterface(0) { }
  Callback(XScript::EngineInterface* ifc, const Object& obj, const Function &fn);
  Callback(XScript::EngineInterface* ifc, const Function &fn);
  ~Callback();

  bool isValid() const;

  void call(Value *result, xsize argCount, Value *args, bool *error, QString *errorOut);

private:
  PersistentValue _object;
  PersistentValue _function;
  };

class CallbackScope
  {
  CallbackScope(Callback& callback);
  ~CallbackScope();

private:
  EngineInterface *_currentInterface;
  EngineInterface *_oldInterface;
  };
}

#endif // XSCRIPTVALUE_H
