#ifndef XSCRIPTINTERFACEBASE_H
#define XSCRIPTINTERFACEBASE_H

#include "XScriptGlobal.h"
#include "XStringSimple"
#include "XProperty"
#include "XMacroHelpers"
#include "XScriptFunction.h"
#include "XInterfaceUtilities.h"
#include "XScriptObject.h"
#include "XUnorderedMap"
#include "XStringSimple"
#include <array>


namespace XScript
{

struct ConstructorDef
  {
  X_CONST_EXPR ConstructorDef(
      const char *n,
      FunctionJS v,
      FunctionDart d,
      xuint8 c)
    : name(n),
      functionV8(v),
      functionDart(d),
      argCount(c)
    {
    }

  const char *name;
  FunctionJS functionV8;
  FunctionDart functionDart;
  xuint8 argCount;
  };

struct FunctionDef
  {
  X_CONST_EXPR FunctionDef(
      const char *n,
      FunctionJS v,
      FunctionDart d,
      xuint8 c,
      bool s)
    : name(n),
      functionV8(v),
      functionDart(d),
      argCount(c),
      isStatic(s)
    {
    }

  const char *name;
  FunctionJS functionV8;
  FunctionDart functionDart;
  xuint8 argCount;
  bool isStatic;
  };

struct PropertyDef
  {
  X_CONST_EXPR PropertyDef (
      const char *n,
      GetterFn gv,
      SetterFn sv,
      FunctionDart gd,
      FunctionDart sd)
    : name(n),
      getterV8(gv),
      setterV8(sv),
      getterDart(gd),
      setterDart(sd)
    {
    }

  const char *name;
  GetterFn getterV8;
  SetterFn setterV8;
  FunctionDart getterDart;
  FunctionDart setterDart;
  };

class InterfaceInternal
  {
  void* _data[2];
  };

class EKSSCRIPT_EXPORT InterfaceBase
  {
public:
  typedef Value (*ToScriptFn)(const void * const &);
  typedef void *(*FromScriptFn)(Value const &);

  typedef void* (*UpCastFn)(void *ptr);
  typedef Eks::UnorderedMap<int, UpCastFn> UpCastMap;

XProperties:
  XROProperty(Eks::String, typeName);
  XROProperty(int, typeId);
  XROProperty(int, baseTypeId);
  XROProperty(int, nonPointerTypeId);
  XROProperty(int, baseNonPointerTypeId);

  XProperty(ToScriptFn, toScript, setToScript);
  XProperty(FromScriptFn, fromScript, setFromScript);

  XROProperty(const InterfaceBase *, parent);

  XRORefProperty(UpCastMap, upcasts);

  XROProperty(xsize, constructorCount);
  XROProperty(xsize, propertyCount);
  XROProperty(xsize, functionCount);

public:
  //typedef Value (*NativeCtor)(internalArguments const &argv);
  InterfaceBase(int typeID,
                 int nonPointerTypeID,
                 int baseTypeID,
                 int baseNonPointerTypeID,
                 const Eks::String &typeName,
                 const InterfaceBase *parent,
                 ToScriptFn convertTo=0,
                 FromScriptFn convertFrom=0);

  InterfaceBase(int typeID,
                 int nonPointerTypeID,
                 const Eks::String &typeName,
                 const InterfaceBase *parent);
  ~InterfaceBase();

  void clear();

  void seal();
  bool isSealed() const
    {
    return _isSealed;
    }

  QVariant toVariant(const Value &val, int typeHint);
  Value copyValue(const QVariant &) const;

  void wrapInstance(Object *obj, void *object) const;
  void unwrapInstance(Object *object) const;

  Object newInstance(int argc, Value argv[], const QString& name="") const;
  //void set(const char *name, Value val);

  typedef Value (*NamedGetter)(Value, const internal::JSAccessorInfo& info);
  typedef Value (*IndexedGetter)(xuint32, const internal::JSAccessorInfo& info);

  void build(const char* name);

  template <typename T> void build(const char* name)
    {
    build(name);
    }

  void addChildInterface(int typeId, UpCastFn fn);

  const ConstructorDef &constructor(xsize index) const
    {
    xAssert(_constructors);
    xAssert(index < _constructorCount);
    return _constructors[index];
    }

  const PropertyDef &property(xsize index) const
    {
    xAssert(_properties);
    xAssert(index < _propertyCount);
    return _properties[index];
    }

  const FunctionDef &function(xsize index) const
    {
    xAssert(_functions);
    xAssert(index < _functionCount);
    return _functions[index];
    }

protected:
  mutable bool _isSealed;

  const ConstructorDef *_constructors;
  const PropertyDef *_properties;
  const FunctionDef *_functions;
  };

EKSSCRIPT_EXPORT InterfaceBase *findInterface(int qMetaTypeId);
EKSSCRIPT_EXPORT void registerInterface(InterfaceBase *ifc);

template <typename T> const InterfaceBase* findInterface(const T *)
  {
  return findInterface(qMetaTypeId<T*>());
  }
}

#endif // XSCRIPTINTERFACEBASE_H
