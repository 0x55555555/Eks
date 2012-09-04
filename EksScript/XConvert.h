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

template <typename T> inline const T &ptrMatcher(T **in, bool& valid)
  {
  if(!*in)
    {
    static char o[sizeof(T)];
    valid = false;
    return *(T*)o;
    }
  return **in;
  }

template <typename Out, typename In> Out match(In *in, bool &valid)
  {
  valid = true;
  return Out(*in);
  }

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
