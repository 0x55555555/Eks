#ifndef XSCRIPTFUNCTION_H
#define XSCRIPTFUNCTION_H

#include "XScriptGlobal.h"
#include "XScriptEngine.h"
#include "QMetaType"

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

private:
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

  void callWithTryCatch(Value *result, const Object &thisObject, int argc, const Value args[], bool *error, QString *) const;
  //XScriptValue call(const XScriptObject &thisObject, int argc, const XScriptValue args[]) const;
  //void callAsConstructor(Value *result, const Arguments&);

  typedef FunctionScope Scope;

private:
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
}

}

Q_DECLARE_METATYPE(XScript::Function*);
Q_DECLARE_METATYPE(XScript::Function);

#endif // XSCRIPTFUNCTION_H
