#ifndef XCONVERTTOSCRIPT_H
#define XCONVERTTOSCRIPT_H

#include "XAssert"
#include "XConvert.h"
#include "XScriptTypeInfo.h"
#include "XSignatureHelpers.h"
#include "XSignatureSpecialisations.h"

namespace XScriptConvert
{

namespace internal
{

template <typename NT> struct NativeToJS<NT *> : NativeToJS<NT> {};
template <typename NT> struct NativeToJS<NT const *> : NativeToJS<NT *> {};
template <typename NT> struct NativeToJS<const NT &> : NativeToJS<NT> {};
template <typename NT> struct NativeToJS<const NT> : NativeToJS<NT> {};

template <typename NT> struct NativeToJS<NT &>
  {
  typedef typename XScriptTypeInfo<NT>::Type & ArgType;
  XScriptValue operator()( ArgType n ) const
    {
    typedef NativeToJS< typename XScriptTypeInfo<NT>::NativeHandle > Cast;
    return Cast()( &n );
    }
  };


namespace internal
{
template <typename IntegerT> struct NativeToJS_int_small
  {
  XScriptValue operator()( IntegerT v ) const
    {
    return XScriptValue(v);
    }
  };

template <typename IntegerT> struct NativeToJSUIntSmall
  {
  XScriptValue operator()( IntegerT v ) const
    {
    return XScriptValue(v);
    }
  };

template <typename IntegerT> struct NativeToJSNumber
  {
  XScriptValue operator()( IntegerT v ) const
    {
    return XScriptValue(v);
    }
  };

template <typename T> struct UselessConversionType
  {
  };
}

template <> struct NativeToJS<unsigned char> : internal::NativeToJSUIntSmall<xuint8> {};
template <> struct NativeToJS<xint16> : internal::NativeToJS_int_small<xint16> {};
template <> struct NativeToJS<xuint16> : internal::NativeToJSUIntSmall<xuint16> {};
template <> struct NativeToJS<xint32> : internal::NativeToJS_int_small<xint32> {};
template <> struct NativeToJS<xuint32> : internal::NativeToJSUIntSmall<xuint32> {};

template <> struct NativeToJS<xint64> : internal::NativeToJSNumber<xint64> {};
template <> struct NativeToJS<xuint64> : internal::NativeToJSNumber<xuint64> {};
template <> struct NativeToJS<double> : internal::NativeToJSNumber<double> {};
template <> struct NativeToJS<float> : internal::NativeToJSNumber<float> {};

// workarounds where long int != uint64
template <> struct NativeToJS< XIfElse< XSameType<unsigned long int, xuint64>::Value,
    internal::UselessConversionType<unsigned long>,
    unsigned long >::Type >
    : internal::NativeToJSNumber<unsigned long int>
  {
  };

template <> struct NativeToJS< XIfElse< XSameType<long, xint64>::Value,
    internal::UselessConversionType<long>,
    long >::Type >
    : internal::NativeToJSNumber<xint64>
  {
  };

template <> struct NativeToJS<bool>
  {
  XScriptValue operator()( bool v ) const
    {
    return XScriptValue( v );
    }
  };

//template <typename T> struct NativeToJS< ::v8::Handle<T> >
//  {
//  typedef ::v8::Handle<T> HandleType;
//  XScriptValue operator()(HandleType const &li) const
//    {
//    return li;
//    }
//  };
/*
template <typename T> struct NativeToJS< ::v8::Local<T> >
  {
  typedef ::v8::Local<T> HandleType;
  XScriptValue operator()(HandleType const &li) const
    {
    return li;
    }
  };

template <typename T> struct NativeToJS< ::v8::Persistent<T> >
  {
  typedef ::v8::Persistent<T> HandleType;
  XScriptValue operator()(HandleType const &li) const
    {
    return li;
    }
  };

template <> struct NativeToJS< ::v8::InvocationCallback >
  {
  XScriptValue operator()(::v8::InvocationCallback const ) const
    {
    xAssertFail();
    return XScriptValue();
    //return ::v8::FunctionTemplate::New(f)->GetFunction();
    }
  };*/

template <> struct NativeToJS<char const *>
  {
  XScriptValue operator()(char const *v) const
    {
    if(!v) return XScriptValue();
    else return XScriptValue( v );
    }
  };

template <> struct NativeToJS<QString>
  {
  XScriptValue operator()(QString v) const
    {
    return XScriptValue(v);
    }
  };

template <> struct NativeToJS<QVariant>
  {
  XScriptValue operator()(QVariant v) const
    {
    return XScriptValue(v);
    }
  };

// Converts a native std::exception to a JS exception and throws
// that exception via v8::ThrowException().
template <> struct NativeToJS<std::exception>
  {
  XScriptValue operator()( std::exception const &) const
    {
    xAssertFail();
    return XScriptValue();
    //char const *msg = ex.what();
    //return v8::Exception::Error(v8::String::New( msg ? msg : "unspecified std::exception" ));
    }
  };

template <> struct NativeToJS<std::runtime_error> : NativeToJS<std::exception> {};
template <> struct NativeToJS<std::range_error> : NativeToJS<std::exception> {};
template <> struct NativeToJS<std::logic_error> : NativeToJS<std::exception> {};

}

// Overloads to avoid ambiguity in certain calls.
static inline XScriptValue to(char const *v)
  {
  typedef internal::NativeToJS<char const *> F;
  return F()( v );
  }

//static inline XScriptValue to(v8::InvocationCallback v)
//  {
//  typedef internal::NativeToJS<v8::InvocationCallback> F;
//  return F()( v );
//  }

static inline XScriptValue to(char *v)
  {
  typedef internal::NativeToJS<char const *> F;
  return F()( v );
  }

template <typename T> inline XScriptValue to(T const &v)
  {
  typedef internal::NativeToJS<T const> F;
  return F()( v );
  }

}

#endif // XCONVERTTOSCRIPT_H
