#ifndef XSCRIPTVALUE_H
#define XSCRIPTVALUE_H

#include "XScriptGlobal.h"

class XScriptObject;
class QVariant;

class EKSSCRIPT_EXPORT XScriptValue
  {
public:
  XScriptValue();
  explicit XScriptValue(bool x);
  XScriptValue(xuint32 x);
  XScriptValue(xint32 x);
  XScriptValue(xuint64 x);
  XScriptValue(xint64 x);
  XScriptValue(double x);
  XScriptValue(float x);
  XScriptValue(const QString &str);
  XScriptValue(const XScriptObject &str);
  XScriptValue(const QVariant& val);
  explicit XScriptValue(void* val);
  ~XScriptValue();

  XScriptValue(const XScriptValue&);
  XScriptValue& operator=(const XScriptValue&);

  void clear();

  bool isValid() const;
  bool isObject() const;
  bool isArray() const;
  bool isBoolean() const;
  bool isNumber() const;
  bool isInteger() const;
  bool isString() const;

  xsize length() const;
  XScriptValue at(xsize id) const;
  void set(xsize id, const XScriptValue &);

  void *toExternal() const;
  double toNumber() const;
  xint64 toInteger() const;
  bool toBoolean() const;
  QString toString() const;
  QVariant toVariant(int typeHint=0) const;

  static XScriptValue newArray();
  static XScriptValue empty();

private:
  void *_object;
  friend class XPersistentScriptValue;
  };

class EKSSCRIPT_EXPORT XPersistentScriptValue
  {
public:
  XPersistentScriptValue();
  XPersistentScriptValue(const XScriptValue &val);

  ~XPersistentScriptValue()
    {
    }

  XScriptValue asValue() const;

  typedef void (*WeakDtor)(XPersistentScriptValue object, void* p);
  void makeWeak(void *data, WeakDtor cb);

  void dispose();

private:
  void *_object;
  };

#endif // XSCRIPTVALUE_H
