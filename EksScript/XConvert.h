#ifndef XCONVERT_H
#define XCONVERT_H

#include "XScriptTypeInfo.h"
#include "XScriptValue.h"
#include "XAssert"

namespace XScript
{

namespace Convert
{

template <typename T, typename U> T *castFromBase(U *ptr)
  {
  return dynamic_cast<T*>(ptr);
  }

template <typename T> inline T &ptrMatcher(T **in, bool& valid)
  {
  if(!*in)
    {
    static char o[sizeof(T)];
    union
      {
      char *c;
      T *t;
      } u;
    u.c = o;
    valid = false;
    return *u.t;
    }
  return **in;
  }

template <typename Out, typename In> class TypeMatcher
  {
public:
  typedef typename std::remove_reference<In>::type NoRef;
  static Out match(NoRef *in, bool &valid)
    {
    valid = true;
    return *in;
    }
  };

namespace internal
{

template <typename JST> struct JSToNative
  {
  typedef typename TypeInfo<JST>::NativeHandle ResultType;

  //! Must be specialized to be useful.
  ResultType operator()(Value const &h) const;
  };

template <typename NT> struct NativeToJS
  {
  template <typename X>
  Value operator()( X const & ) const;
  };
}

}

}

#endif // XCONVERT_H
