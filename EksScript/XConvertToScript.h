#ifndef XCONVERTTOSCRIPT_H
#define XCONVERTTOSCRIPT_H

#include "XAssert"
#include "XScriptTypeInfo.h"
#include "XSignatureHelpers.h"
#include "XSignatureSpecialisations.h"
#include "XConvert.h"


namespace XScript
{

namespace Convert
{

namespace internal
{

template <typename NT> struct NativeToJS<NT *> : NativeToJS<NT> {};
template <typename NT> struct NativeToJS<NT const *> : NativeToJS<NT *> {};
template <typename NT> struct NativeToJS<const NT &> : NativeToJS<NT> {};
template <typename NT> struct NativeToJS<const NT> : NativeToJS<NT> {};

template <typename NT> struct NativeToJS<NT &>
  {
  typedef typename TypeInfo<NT>::Type & ArgType;
  Value operator()( ArgType n ) const
    {
    typedef NativeToJS< typename TypeInfo<NT>::NativeHandle > Cast;
    return Cast()( &n );
    }
  };


template <typename IntegerT> struct NativeToJS_int_small
  {
  Value operator()( IntegerT v ) const
    {
    return Value(v);
    }
  };

template <typename IntegerT> struct NativeToJSUIntSmall
  {
  Value operator()( IntegerT v ) const
    {
    return Value(v);
    }
  };

template <typename T> struct UselessConversionType
  {
  };

template <> struct NativeToJS<unsigned char> : NativeToJSUIntSmall<xuint8> {};
template <> struct NativeToJS<xint16> : NativeToJS_int_small<xint16> {};
template <> struct NativeToJS<xuint16> : NativeToJSUIntSmall<xuint16> {};
template <> struct NativeToJS<xint32> : NativeToJS_int_small<xint32> {};
template <> struct NativeToJS<xuint32> : NativeToJSUIntSmall<xuint32> {};

template <> struct NativeToJS<xint64>
  {
  Value operator()( xint64 v ) const
    {
    return v;
    }
  };

template <> struct NativeToJS<xuint64>
  {
  Value operator()( xuint64 v ) const
    {
    return v;
    }
  };


template <> struct NativeToJS< XIfElse< XSameType<unsigned long int, xuint64>::Value,
    UselessConversionType<unsigned long>,
    unsigned long >::Type >
  {
  Value operator()( unsigned long int v ) const
    {
    return Value((xuint64)v);
    }
  };


template <> struct NativeToJS< XIfElse< XSameType<long, xint64>::Value,
    UselessConversionType<long>,
    long >::Type >
  {
  Value operator()( xint64 v ) const
    {
    return v;
    }
  };

template <> struct NativeToJS<double>
  {
  Value operator()( double v ) const
    {
    return v;
    }
  };

template <> struct NativeToJS<float>
  {
  Value operator()( float v ) const
    {
    return v;
    }
  };

template <> struct NativeToJS<bool>
  {
  Value operator()( bool v ) const
    {
    return Value( v );
    }
  };

template <> struct NativeToJS<char const *>
  {
  Value operator()(char const *v) const
    {
    if(!v) return Value();
    else return Value( v );
    }
  };

template <typename C, xsize P, typename A> struct NativeToJS<Eks::StringBase<C, P, A> >
  {
  Value operator()(const Eks::StringBase<C, P, A> &v) const
    {
    return Value(v);
    }
  };

template <> struct NativeToJS<Eks::String>
  {
  Value operator()(const Eks::String &v) const
    {
    return Value(v);
    }
  };

template <> struct NativeToJS<QVariant>
  {
  Value operator()(QVariant v) const
    {
    return Value(v);
    }
  };

// Converts a native std::exception to a JS exception and throws
// that exception via v8::ThrowException().
template <> struct NativeToJS<std::exception>
  {
  Value operator()( std::exception const &) const
    {
    xAssertFail();
    return Value();
    //char const *msg = ex.what();
    //return v8::Exception::Error(v8::String::New( msg ? msg : "unspecified std::exception" ));
    }
  };


}

// Overloads to avoid ambiguity in certain calls.
static inline Value to(char const *v)
  {
  typedef internal::NativeToJS<char const *> F;
  return F()( v );
  }

//static inline Value to(v8::InvocationCallback v)
//  {
//  typedef internal::NativeToJS<v8::InvocationCallback> F;
//  return F()( v );
//  }

static inline Value to(char *v)
  {
  typedef internal::NativeToJS<char const *> F;
  return F()( v );
  }

template <typename T> inline Value to(T const &v)
  {
  typedef internal::NativeToJS<T const> F;
  return F()( v );
  }

}

}

#endif // XCONVERTTOSCRIPT_H
