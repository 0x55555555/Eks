#ifndef XCONVERTFROMSCRIPT_H
#define XCONVERTFROMSCRIPT_H

#include "XAssert"
#include "XConvert.h"
#include "XScriptTypeInfo.h"
#include "XSignatureHelpers.h"
#include "XSignatureSpecialisations.h"
#include "XScriptObject.h"
#include <stdexcept>

namespace XScriptConvert
{

namespace internal
{

template <typename JST> struct JSToNative<JST const> : JSToNative<JST> {};
template <typename JST> struct JSToNative<JST *> : JSToNative<JST> {};
template <typename JST> struct JSToNative<JST const *> : JSToNative<JST> {};

template <typename JST> struct JSToNative<JST &>
  {
  typedef typename XScriptTypeInfo<JST>::Type &ResultType;

  ResultType operator()(XScriptValue const &h) const
    {
    typedef JSToNative<JST*> Cast;
    typedef typename Cast::ResultType NH;
    NH n = Cast()( h );
    if( ! n )
      {
      throw std::runtime_error("JSToNative<T&> could not get native pointer. Throwing to avoid dereferencing null!");
      }
    else
      {
      return *n;
      }
    }
  };

template <typename JST> struct JSToNative<JST const &>
  {
  typedef typename XScriptTypeInfo<JST>::Type const &ResultType;

  ResultType operator()(XScriptValue const &h) const
    {
    typedef JSToNative<JST &> Cast;
    typedef typename Cast::ResultType NH;
    return Cast()(h);
    }
  };

template <typename JST> struct JSToNativeAbstract
  {
  typedef typename XScriptTypeInfo<JST>::NativeHandle ResultType;

  ResultType operator()( XScriptValue const & ) const
    {
    return 0;
    }
  };

//template <typename T> struct JSToNative<XScriptValue>
//  {
//  typedef v8::Handle<T> ResultType;

//  ResultType operator()( XScriptValue const & h ) const
//    {
//    return h;
//    }
//  };

//template <typename T> struct JSToNative<v8::Handle<T> const &> : JSToNative< v8::Handle<T> > {};
//template <typename T> struct JSToNative<v8::Handle<T> &> : JSToNative< v8::Handle<T> > {};

//template <typename T> struct JSToNative<v8::Local<T> >
//  {
//  typedef v8::Local<T> ResultType;

//  ResultType operator()( v8::Local<T> const & h ) const
//    {
//    return h;
//    }
//  };

//template <typename T> struct JSToNative<v8::Local<T> const &> : JSToNative< v8::Local<T> > {};
//template <typename T> struct JSToNative<v8::Local<T> &> : JSToNative< v8::Local<T> > {};
/*
namespace
{
template <typename V8Type, bool (v8::Value::*IsA)() const> struct JSToNativeV8Type
  {
  typedef v8::Handle<V8Type> ResultType;

  ResultType operator()(v8::Handle<v8::Value> const &h) const
    {
    return (h.IsEmpty() || !((*h)->*IsA)())
        ? v8::Handle<V8Type>()
        : v8::Handle<V8Type>(V8Type::Cast(*h));
    }
  };
}

template <> struct JSToNative<v8::Handle<v8::Object> > : JSToNativeV8Type<v8::Object, &v8::Value::IsObject> {};
template <> struct JSToNative<v8::Handle<v8::Object> &> : JSToNative<v8::Handle<v8::Object> > {};
template <> struct JSToNative<v8::Handle<v8::Object> const &> : JSToNative<v8::Handle<v8::Object> > {};

template <> struct JSToNative<v8::Handle<v8::Array> > : JSToNativeV8Type<v8::Array, &v8::Value::IsArray> {};
template <> struct JSToNative<v8::Handle<v8::Array> &> : JSToNative<v8::Handle<v8::Array> > {};
template <> struct JSToNative<v8::Handle<v8::Array> const &> : JSToNative<v8::Handle<v8::Array> > {};

template <> struct JSToNative<v8::Handle<v8::Function> > : JSToNativeV8Type<v8::Function, &v8::Value::IsFunction> {};
template <> struct JSToNative<v8::Handle<v8::Function> &> : JSToNative<v8::Handle<v8::Function> > {};
template <> struct JSToNative<v8::Handle<v8::Function> const &> : JSToNative<v8::Handle<v8::Function> > {};
*/

template <> struct JSToNative<XScriptFunction>
  {
  typedef XScriptFunction ResultType;

  ResultType operator()(XScriptValue const &h) const
    {
    return XScriptFunction(h);
    }
  };
template <> struct JSToNative<XScriptFunction const &> : JSToNative<XScriptFunction> {};

template <> struct JSToNative<XScriptObject>
  {
  typedef XScriptObject ResultType;

  ResultType operator()(XScriptValue const &h) const
    {
    return XScriptObject(h);
    }
  };
template <> struct JSToNative<XScriptObject const &> : JSToNative<XScriptObject> {};

template <> struct JSToNative<void>
  {
  typedef void ResultType;

  ResultType operator()(...) const
    {
    return;
    }
  };


namespace internal
{
template <typename Ret, Ret (XScriptValue::*ToA)() const> struct JSToNativePODType
  {
  typedef Ret ResultType;

  ResultType operator()(XScriptValue const &h) const
    {
    return (h.*ToA)();
    }
  };

template <typename ExtType> struct JSToNativeExternalType
    : JSToNativePODType<ExtType, &XScriptValue::toExternal>
  {
  };

template <typename IntType> struct JSToNativeIntegerType
    : JSToNativePODType<IntType, &XScriptValue::toInteger>
  {
  };

template <typename NumType> struct JSToNativeNumberType
    : JSToNativePODType<NumType, &XScriptValue::toNumber>
  {
  };
}

template <> struct JSToNative<void *> : internal::JSToNativeExternalType<void *> {};

template <> struct JSToNative<xint8> : internal::JSToNativeIntegerType<xint64> {};
template <> struct JSToNative<xuint8> : internal::JSToNativeIntegerType<xint64> {};
template <> struct JSToNative<xint16> : internal::JSToNativeIntegerType<xint64> {};
template <> struct JSToNative<xuint16> : internal::JSToNativeIntegerType<xint64> {};
template <> struct JSToNative<xint32> : internal::JSToNativeIntegerType<xint64> {};
template <> struct JSToNative<xuint32> : internal::JSToNativeIntegerType<xint64> {};
template <> struct JSToNative<xint64> : internal::JSToNativeIntegerType<xint64> {};
template <> struct JSToNative<xuint64> : internal::JSToNativeIntegerType<xint64> {};

template <> struct JSToNative<double> : internal::JSToNativeNumberType<double> {};
template <> struct JSToNative<float> : internal::JSToNativeNumberType<double> {};

template <> struct JSToNative<bool>
  {
  typedef bool ResultType;

  ResultType operator()(XScriptValue const &h) const
    {
    return h.toBoolean();
    }
  };

template <> struct JSToNative<QString>
  {
  typedef QString ResultType;

  ResultType operator()(XScriptValue const &h) const
    {
    return h.toString();
    }
  };

template <> struct JSToNative<const QString &> : JSToNative<QString>
  {
  };

template <> struct JSToNative<QByteArray>
  {
  typedef QByteArray ResultType;

  ResultType operator()(XScriptValue const &h) const
    {
    return h.toString().toUtf8();
    }
  };

template <> struct JSToNative<const QByteArray &> : JSToNative<QByteArray>
  {
  };

template <> struct JSToNative<QVariant>
  {
  typedef QVariant ResultType;

  ResultType operator()(XScriptValue const &h) const
    {
    return h.toVariant();
    }
  };

template <> struct JSToNative<const QVariant &> : JSToNative<QVariant>
  {
  };

namespace
{
template <typename T> struct UselessConversionTypeToNative
  {
  };
}

template <> struct JSToNative< XIfElse<
    XSameType<unsigned long int,xuint64>::Value,
    UselessConversionTypeToNative<unsigned long>,
    unsigned long >::Type >
    : JSToNative<xuint64>
  {
  };

template <> struct JSToNative< XIfElse<
    XSameType<long,xint64>::Value,
    UselessConversionTypeToNative<long>,
    long >::Type > : JSToNative<xint64>
  {
  };
}

template <typename NT> typename internal::JSToNative<NT>::ResultType from(XScriptValue const &h)
  {
  typedef internal::JSToNative<NT> F;
  return F()( h );
  }

/**
       ArgCaster is a thin wrapper around CastFromJS(), and primarily
       exists to give us a way to convert JS values to (char const *)
       for purposes of passing them to native functions. The main
       difference between this type and JSToNative<T> is that this
       interface explicitly allows for the conversion to be stored by
       an instance of this type. That allows us to get more lifetime
       out of converted values in certain cases (namely (char const*)).

       The default implementation is suitable for all cases which
       JSToNative<T> supports, but specializations can handle some of
       the corner cases which JSToNative cannot (e.g. (char const *)).

       Added 20091121.
    */
template <typename T>
struct ArgCaster
  {
  typedef typename XScriptConvert::internal::JSToNative<T>::ResultType ResultType;
  /**
           Default impl simply returns XScriptConvert::from<T>(v).
           Specializations are allowed to store the result of the
           conversion, as long as they release it when the destruct.
           See XScriptConvert::ArgCaster<char const *> for an example of that.
        */
  static ResultType toNative( XScriptValue const & v )
    {
    return XScriptConvert::from<T>( v );
    }
  /**
            To eventually be used for some internal optimizations.
        */
  enum { HasConstOp = 1 };
  };
/**
       Specialization for (char const *). The value returned from
       ToNative() is guaranteed to be valid as long as the ArgCaster
       object is alive or until ToNative() is called again (which will
       almost certainly change the pointer). Holding a pointer to the
       ToNative() return value after the ArgCaster is destroyed will
       lead to undefined behaviour. Likewise, fetching a pointer, then
       calling ToNative() a second time, will invalidate the first
       pointer.

       BEWARE OF THESE LIMITATIONS:

       1) This will only work properly for nul-terminated strings,
       and not binary data!

       2) Do not use this to pass (char const *) as a function
       parameter if that function will hold a copy of the pointer
       after it returns (as opposed to copying/consuming the
       pointed-to-data before it returns) OR if it returns the
       pointer passed to it. Returning is a specific corner-case
       of "holding a copy" for which we cannot guaranty the lifetime
       at the function-binding level.

       3) Do not use the same ArgCaster object to convert multiple
       arguments, as each call to ToNative() will invalidate the
       pointer returned by previous calls.

       4) The to-string conversion uses whatever encoding
       JSToNative<std::string> uses.

       Violating any of those leads to undefined behaviour, and
       very possibly memory corruption for cases 2 or 3.
    */
template <> struct ArgCaster<char const *>
  {
private:
  /**
            Reminder to self: we cannot use v8::String::Utf8Value
            here because at the point the bindings call ToNative(),
            v8 might have been unlocked, at which point dereferencing
            the Utf8Value becomes illegal.
        */
  std::string val;
  typedef char Type;
public:
  typedef Type const * ResultType;
  /**
           Returns the toString() value of v unless:

           - v.IsEmpty()
           - v->IsNull()
           - v->IsUndefined()

           In which cases it returns 0.

           The returned value is valid until:

           - ToNative() is called again.
           - This object is destructed.
        */
  static ResultType toNative( XScriptValue const & )
    {/*
    typedef XScriptConvert::internal::JSToNative<std::string> C;
    if( !v.isValid() )
      {
      return 0;
      }
    this->val = C()( v );*/
    xAssertFail();
    return "";//this->val.c_str();
    }
  };

}

template <typename T> T *XScriptObject::castTo()
  {
  return XScriptConvert::from<T>(*this);
  }

template <typename T> const T *XScriptObject::castTo() const
  {
  return XScriptConvert::from<T>(*this);
  }

#endif // XCONVERTFROMSCRIPT_H
