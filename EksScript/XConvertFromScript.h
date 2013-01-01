#ifndef XCONVERTFROMSCRIPT_H
#define XCONVERTFROMSCRIPT_H

#include "XAssert"
#include "XConvert.h"
#include "XScriptTypeInfo.h"
#include "XSignatureHelpers.h"
#include "XSignatureSpecialisations.h"
#include "XScriptObject.h"
#include <stdexcept>

namespace XScript
{

namespace Convert
{

namespace internal
{

template <typename JST> struct JSToNative<JST const> : JSToNative<JST> {};
template <typename JST> struct JSToNative<JST *> : JSToNative<JST> {};
template <typename JST> struct JSToNative<JST const *> : JSToNative<JST> {};

template <typename JST> struct JSToNative<JST &>
  {
  typedef typename JSToNative<JST>::ResultType &ResultType;

  ResultType operator()(Value const &h) const
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

template <typename JST> struct JSToNative<const JST &>
  {
  typedef typename const JSToNative<JST &>::ResultType ResultType;

  ResultType operator()(Value const &h) const
    {
    typedef JSToNative<JST &> Cast;
    return Cast()(h);
    }
  };

template <typename JST> struct JSToNativeAbstract
  {
  typedef typename TypeInfo<JST>::NativeHandle ResultType;

  ResultType operator()( Value const & ) const
    {
    return 0;
    }
  };

template <> struct JSToNative<const Value &>
  {
  typedef Value ResultType;

  ResultType operator()( const Value &h ) const
    {
    return h;
    }
  };

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

template <> struct JSToNative<Function>
  {
  typedef Function ResultType;

  ResultType operator()(Value const &h) const
    {
    return Function(h);
    }
  };
template <> struct JSToNative<Function const &> : JSToNative<Function> {};

template <> struct JSToNative<Object>
  {
  typedef Object ResultType;

  ResultType operator()(Value const &h) const
    {
    return Object(h);
    }
  };
template <> struct JSToNative<Object const &> : JSToNative<Object> {};

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
template <typename Ret, Ret (Value::*ToA)() const> struct JSToNativePODType
  {
  typedef Ret ResultType;

  ResultType operator()(Value const &h) const
    {
    return (h.*ToA)();
    }
  };

template <typename ExtType> struct JSToNativeExternalType
    : JSToNativePODType<ExtType, &Value::toExternal>
  {
  };

template <typename IntType> struct JSToNativeIntegerType
    : JSToNativePODType<IntType, &Value::toInteger>
  {
  };

template <typename NumType> struct JSToNativeNumberType
    : JSToNativePODType<NumType, &Value::toNumber>
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

  ResultType operator()(Value const &h) const
    {
    return h.toBoolean();
    }
  };

template <typename C, xsize P, typename A> struct JSToNative<const Eks::StringBase<C, P, A> &>
  {
  typedef Eks::StringBase<C, P, A> ResultType;

  ResultType operator()(Value const &h) const
    {
    return h.toString();
    }
  };

template <typename C, xsize P, typename A> struct JSToNative<Eks::StringBase<C, P, A> >
  {
  typedef Eks::StringBase<C, P, A> ResultType;

  ResultType operator()(Value const &h) const
    {
    return h.toString();
    }
  };

template <> struct JSToNative<QString>
  {
  typedef QString ResultType;

  ResultType operator()(Value const &h) const
    {
    return h.toString().toQString();
    }
  };

template <> struct JSToNative<const QString &> : JSToNative<QString>
  {
  };

template <> struct JSToNative<QByteArray>
  {
  typedef QByteArray ResultType;

  ResultType operator()(Value const &h) const
    {
    Eks::String s = h.toString();
    return QByteArray(s.data(), s.length());
    }
  };

template <> struct JSToNative<const QByteArray &> : JSToNative<QByteArray>
  {
  };

template <> struct JSToNative<QVariant>
  {
  typedef QVariant ResultType;

  ResultType operator()(Value const &h) const
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

template <typename NT> typename internal::JSToNative<NT>::ResultType from(Value const &h)
  {
  typedef internal::JSToNative<NT> F;
  return F()( h );
  }

}

template <typename T> T *Object::castTo()
  {
  return Convert::from<T>(*this);
  }

template <typename T> const T *Object::castTo() const
  {
  return Convert::from<T>(*this);
  }

}


#endif // XCONVERTFROMSCRIPT_H
