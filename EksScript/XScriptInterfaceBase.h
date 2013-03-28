#ifndef XSCRIPTINTERFACEBASE_H
#define XSCRIPTINTERFACEBASE_H

#include "XScriptGlobal.h"
#include "XStringSimple"
#include "XProperty"
#include "XMacroHelpers"
#include "XScriptFunction.h"
#include "XInterfaceUtilities.h"
#include "XConvertToScript.h"
#include "XScriptObject.h"
#include "XScriptException.h"
#include "XFunctions.h"
#include "XUnorderedMap"
#include "XStringSimple"
#include <array>


namespace XScript
{

namespace internal
{
template <typename T, bool HasQMetaType = QMetaTypeId<T>::Defined> struct QMetaTypeIdOrInvalid
  {
  static int id()
    {
    return 0;
    }
  };

template <typename T> struct QMetaTypeIdOrInvalid<T, true>
  {
  static int id()
    {
    return qMetaTypeId<T>();
    }
  };
}

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

template <xsize CCount,
          xsize PCount,
          xsize FCount> struct ClassDef
  {
  ConstructorDef constructors[CCount];
  PropertyDef properties[PCount];
  FunctionDef functions[FCount];
  };

template <xsize CCount,
          xsize PCount> struct ClassDef<CCount, PCount, 0>
  {
  ConstructorDef constructors[CCount];
  PropertyDef properties[PCount];
  enum
    {
    functions = 0
    };
  };

template <xsize CCount,
          xsize FCount> struct ClassDef<CCount, 0, FCount>
  {
  ConstructorDef constructors[CCount];
  FunctionDef functions[FCount];
  enum
    {
    properties = 0
    };
  };

template <xsize CCount> struct ClassDef<CCount, 0, 0>
  {
  ConstructorDef constructors[CCount];
  enum
    {
    properties = 0,
    functions = 0
    };
  };

template <xsize PCount,
          xsize FCount> struct ClassDef<0, PCount, FCount>
  {
  PropertyDef properties[PCount];
  FunctionDef functions[FCount];
  enum
    {
    constructors = 0,
    };
  };

template <xsize PCount> struct ClassDef<0, PCount, 0>
  {
  PropertyDef properties[PCount];
  enum
    {
    constructors = 0,
    functions = 0,
    };
  };


template <xsize FCount> struct ClassDef<0, 0, FCount>
  {
  FunctionDef functions[FCount];
  enum
    {
    constructors = 0,
    properties = 0
    };
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

  void buildInterface(
      const ConstructorDef *ctors,
      xsize ctorCount,
      const PropertyDef *props,
      xsize propCount,
      const FunctionDef *fns,
      xsize fnCount);

  template <xsize CtorCount,
            xsize PropCount,
            xsize FnCount>
  void buildInterface(const ClassDef<CtorCount, PropCount, FnCount> &cls)
    {
    buildInterface((const ConstructorDef *)cls.constructors,
                   CtorCount,
                   (const PropertyDef *)cls.properties,
                   PropCount,
                   (const FunctionDef *)cls.functions,
                   FnCount);
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

  // generating defs

  template <typename Getter,
            typename Setter>
      X_CONST_EXPR static PropertyDef property(const char *name)
    {
    return PropertyDef(
      name,
      Getter::Get,
      Setter::Set,
      Getter::GetDart,
      Setter::SetDart
      );
    }

  template <typename Getter>
      X_CONST_EXPR static PropertyDef property(const char *name)
    {
    return PropertyDef(
      name,
      Getter::Get,
      0,
      Getter::GetDart,
      0
      );
    }

  template <typename FunctionType>
      X_CONST_EXPR static FunctionDef method(const char *name)
    {
    return FunctionDef(
      name,
      FunctionType::Call,
      FunctionType::CallDart,
      FunctionType::Arity,
      false // not static method
      );
    }

  template <typename FunctionType>
      static void methodX(const char *name)
    {
    return;
    }

  template <typename FunctionType>
      static FunctionDef function(const char *name)
    {
    return FunctionDef(
      name,
      FunctionType::Call,
      FunctionType::CallDart,
      FunctionType::Arity,
      true // is static method
      );
    }

  template <typename SIG,
            typename XFunctionSignature<SIG>::FunctionType METHOD>
      X_CONST_EXPR static FunctionDef staticMethod(const char *name)
    {
    typedef XScript::FunctionToInCa<SIG, METHOD> FunctionType;

    return function<FunctionType>(name);
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
