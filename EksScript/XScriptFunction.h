#ifndef XSCRIPTFUNCTION_H
#define XSCRIPTFUNCTION_H

#include "XScriptGlobal.h"
#include "XScriptEngine.h"
#include "QtCore/QMetaType"
#include "QtCore/QVariant"

namespace XScript
{
class Object;
class Value;
class Arguments;
class Source;

class EKSSCRIPT_EXPORT FunctionScope : public EngineScope
  {
public:
  FunctionScope(EngineInterface *ifc) : EngineScope(ifc)
    {
    currentInterface()->beginFunctionScope(this);
    }

  FunctionScope(Source &);

  ~FunctionScope()
    {
    currentInterface()->endFunctionScope(this);
    }

protected:
  void* _data[2];
  };

class EKSSCRIPT_EXPORT Function
  {
public:
  //typedef XScriptValue (*Function)( XScriptArguments const & argv );
  //XScriptFunction(Function fn);

  Function(const Value& );
  Function();
  ~Function();

  Function(const Function&);
  Function& operator=(const Function&);

  bool isValid() const;

  void callWithTryCatch(Value *result, const Object &thisObject, xsize argc, const Value args[], bool *error, QString *) const;
  //XScriptValue call(const XScriptObject &thisObject, int argc, const XScriptValue args[]) const;
  //void callAsConstructor(Value *result, const Arguments&);

  typedef FunctionScope Scope;

protected:
  void *_func;
  };

namespace internal
{

class EKSSCRIPT_EXPORT JSAccessorInfo
  {
public:
  Object calleeThis() const;
  Value data() const;

private:
  JSAccessorInfo();
  ~JSAccessorInfo();
  JSAccessorInfo(const JSAccessorInfo&);
  JSAccessorInfo& operator=(const JSAccessorInfo&);

  void* _info;
  };

class EKSSCRIPT_EXPORT JSArguments
  {
public:
  bool isConstructCall() const;
  Function callee() const;
  Object calleeThis() const;
  xsize length() const;
  Value at(xsize i) const;

  template <typename T, typename Ret>Ret at(xsize idx) const;

private:
  JSArguments();
  ~JSArguments();
  JSArguments(const JSArguments&);
  JSArguments& operator=(const JSArguments&);

  void* _args;
  void* _values;
  int _length;
  bool _isConstructCall;
  };

class EKSSCRIPT_EXPORT DartArguments
  {
public:
  void setReturnValue(const Value& val);

private:
  DartArguments() { }

  friend class DartArgumentsNoThis;
  friend class DartArgumentsWithThis;
  void *_args;
  };

class EKSSCRIPT_EXPORT DartArgumentsNoThis
  {
public:
  DartArgumentsNoThis(DartArguments args, xsize offset=0)
    : _args(args), _offset(offset)
    {
    }

  template <typename T, typename Ret>Ret at(xsize idx) const;

  Value at(xsize idx) const;
  xsize length() const;

protected:
  DartArguments _args;
  xsize _offset;
  };

class EKSSCRIPT_EXPORT DartArgumentsWithThis : public DartArgumentsNoThis
  {
public:
  DartArgumentsWithThis(DartArguments args)
      : DartArgumentsNoThis(args, 1)
    {
    }

  Object calleeThis();
  };

template <typename In, typename Out> struct ConvertArg
  {
  static Out conv(In &a)
    {
    return (Out)a;
    }
  };

template <typename In> struct ConvertArg<In, In>
  {
  static In &conv(In &a)
    {
    return a;
    }
  };

template <typename In> struct ConvertArg<const In &, In>
  {
  static In conv(const In &a)
    {
    return a;
    }
  };

template <typename In> struct ConvertArg<In, In*>
  {
  static In *conv(In &a)
    {
    return &a;
    }
  };

class ReflectArguments
  {
public:
  void* ths;
  QVariant* args;
  xsize argCount;
  QVariant *result;

  template <typename T, typename Ret>Ret at(xsize idx) const
    {
    typedef typename std::remove_reference<T>::type NoRefType;
    typedef typename std::remove_const<NoRefType>::type NoConstNoRefType;

    xAssert(idx < argCount);
    T thing = args[idx].value<Ret>();

    return ConvertArg<T, Ret>::conv(thing);
    }
  xsize length() const
    {
    return argCount;
    }

  void setReturnValue(const QVariant& val)
    {
    *result = val;
    }

  template <typename T> T* calleeThis()
    {
    return static_cast<T*>(ths);
    }
  };

}

}

Q_DECLARE_METATYPE(XScript::Function*);
Q_DECLARE_METATYPE(XScript::Function);

#endif // XSCRIPTFUNCTION_H
