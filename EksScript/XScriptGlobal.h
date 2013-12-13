#ifndef XSCRIPTGLOBAL_H
#define XSCRIPTGLOBAL_H

#include "XGlobal.h"

#ifdef EKSSCRIPT_BUILD
# define EKSSCRIPT_EXPORT X_DECL_EXPORT
#else
# define EKSSCRIPT_EXPORT X_DECL_IMPORT
#endif

class QVariant;

namespace XScript
{

enum ObjectInternalField
  {
  NativePointer,
  TypeId,
  InternalFieldCount
  };

namespace internal
{
class JSArguments;
class DartArguments;
class ReflectArguments;
class JSAccessorInfo;
}

class Value;

typedef Value (*FunctionJS)(internal::JSArguments const & argv);
typedef void (*FunctionDart)(internal::DartArguments argv);
typedef void (*FunctionReflection)(internal::ReflectArguments &argv);

typedef Value (*GetterFn)(Value property, const internal::JSAccessorInfo& info);
typedef void (*SetterFn)(Value property, Value value, const internal::JSAccessorInfo& info);

}

#endif // XSCRIPTGLOBAL_H
