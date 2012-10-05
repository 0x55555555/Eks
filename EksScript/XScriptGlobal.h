#ifndef XSCRIPTGLOBAL_H
#define XSCRIPTGLOBAL_H

#include "XGlobal"

#ifdef EKSSCRIPT_BUILD
# define EKSSCRIPT_EXPORT X_DECL_EXPORT
#else
# define EKSSCRIPT_EXPORT X_DECL_IMPORT
#endif

//#define X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
#ifdef Q_CC_MSVC
# define X_SCRIPT_ENGINE_ENABLE_DART
#endif

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
class JSAccessorInfo;
}

class Value;

typedef Value (*FunctionJS)( internal::JSArguments const & argv );
typedef void (*FunctionDart)( internal::DartArguments argv );

typedef Value (*GetterFn)(Value property, const internal::JSAccessorInfo& info);
typedef void (*SetterFn)(Value property, Value value, const internal::JSAccessorInfo& info);

}

#endif // XSCRIPTGLOBAL_H
