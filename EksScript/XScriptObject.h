#ifndef XSCRIPTOBJECT_H
#define XSCRIPTOBJECT_H

#include "XScriptGlobal.h"
#include "QMetaType"

namespace XScript
{

class Function;
class Value;
class PersistentValue;
class InterfaceBase;
template <typename T> class Interface;

EKSSCRIPT_EXPORT InterfaceBase *findInterface(int qMetaTypeId);

class EKSSCRIPT_EXPORT Object
  {
public:
  Object();
  Object(const Value &func);
  //Object(const Function &func);
  ~Object();

  Object(const Object&);
  Object& operator=(const Object&);

  //xsize internalFieldCount() const;
  void *internalField(ObjectInternalField idx) const;

  void *getThis() const
    {
    return internalField(NativePointer);
    }

  InterfaceBase *getInterface() const
    {
    void *tid = internalField(TypeId);
    return findInterface((xsize)tid);
    }

  Value get(const QString &) const;
  void set(const QString &, const Value &);

  bool isValid() const;

  template <typename T>
  static Object newInstance(Interface<T>* i);

  template <typename T> T *castTo();
  template <typename T> const T *castTo() const;

  static Object newMap();

private:
  void *_object;
  friend class XScriptContext;
  };

}

Q_DECLARE_METATYPE(XScript::Object);
Q_DECLARE_METATYPE(XScript::Object*);

#endif // XSCRIPTOBJECT_H
