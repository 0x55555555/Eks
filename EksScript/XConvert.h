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
  };

template <typename T> class TypeMatcher<const T &, T *>
  {
public:
  static const T &match(T **in, bool &valid)
    {
    valid = true;
    return Out(*in);
    }
  };

template <typename T> class TypeMatcher<const T *, T *>
  {
public:
  static const T *match(T **in, bool &valid)
    {
    valid = true;
    return *in;
    }
  };

template <typename T> class TypeMatcher<T *, T *>
  {
public:
  static T *match(T **in, bool &valid)
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
