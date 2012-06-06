#ifndef XSCRIPRCONSTRUCTORS_H
#define XSCRIPRCONSTRUCTORS_H

#include <stdexcept>
#include "XSignature.h"
#include "XSignatureHelpers.h"
#include "XScriptException.h"
#include "XSignatureSpecialisations.h"
#include "XTemplateMetaHelpers.h"
#include "XScriptDefinitions.h"
#include "XAssert"
#include "XScriptTypeInfo.h"

namespace XScript
{

namespace Detail
{
/**
            Default (unimplemented) CtorForwarderProxy impl. A helper
            for the CtorForwarder class. All specializations except
            the 0-arity one are generated from script code.
        */
template <typename Sig, int Arity = sl::Arity< XSignature<Sig> >::Value >
struct CtorForwarderProxy
  {
  typedef typename XSignature<Sig>::ReturnType ReturnType;
  static ReturnType Call( XScriptArguments const & );
  };

//! Specialization for 0-arity ctors.
template <typename Sig>
struct CtorForwarderProxy<Sig,0>
  {
  typedef typename XSignature<Sig>::ReturnType ReturnType;
  template <typename ArgsType>
      static ReturnType Call( ArgsType const & )
    {
    typedef typename XScriptTypeInfo<ReturnType>::Type RType;
    return new RType;
    }
  };
//! Specialization for ctors taking (XScriptArguments const &).
template <typename Sig>
struct CtorForwarderProxy<Sig,-1>
  {
  typedef typename XSignature<Sig>::ReturnType ReturnType;
  template <typename ArgsType>
      static ReturnType Call( ArgsType const & argv )
    {
    typedef typename XScriptTypeInfo<ReturnType>::Type T;
    return new T(argv);
    }
  };

}

/**
       A utility type to help forward XScriptArguments to native
       constructors. This type is intended to assist in the creation
       of ctor functions for JS-bound C++ classes.

       Requirements:

       - Sig is "almost" a function-signature-style type, but
       because ctors don't have a return value in the conventional
       sense, we have to use a tiny workaround: Sig should be passed
       in like this from client code:

       @code
       typedef CtorForwarder<T * (argType1, argType2)> CF;
       @endcode

       - (new T(...)) must be legal, taking a number of
       arguments equal to Sig's Arity.

       - All arguments to the native ctor must be convertible
       using CastFromJS().

        Example:

       @code
       typedef CtorForwarder<MyType *()> CF0;
       typedef CtorForwarder<MyType *(int)> CF1;
       typedef CtorForwarder<MyType *(double,int)> CF2;
       typedef CtorForwarder<MyType *(XScriptArguments const &)> CFAny;
       @endcode

       @see CtorArityDispatcher
    */
template <typename Sig>
struct CtorForwarder : XSignature<Sig>
  {
  typedef XSignature<Sig> STL;
  enum { Arity = sl::Arity< STL >::Value };
  //typedef typename tmp::AddPointer<typename STL::ReturnType>::Type ReturnType;
  typedef typename STL::ReturnType ReturnType;
  /**
            If (argv.Length()>=Arity) or Arity is less than 0,
            then the constructor is called with Arity arguments
            (if it >=0) or with 1 XScriptArguments parameter (for Arity<0).

            Returns the result of (new Type(...)), transfering ownership
            to the caller.

            May propagate native exceptions.
        */
  template <typename ArgType>
  static ReturnType Call( ArgType const & argv )
    {
    typedef Detail::CtorForwarderProxy<Sig> Proxy;
    return Proxy::Call( argv );
    }
  };

#if !defined(DOXYGEN)
namespace Detail
{

/**
           Internal dispatch routine. CTOR _must_ be a CtorForwarder implementation
           (or interface-compatible).
        */
template <typename T,typename CTOR>
struct CtorFwdDispatch
  {
  typedef typename XScriptTypeInfo<T>::NativeHandle ReturnType;
  static ReturnType Call( XScriptArguments const &  argv )
    {
    return CTOR::Call( argv );
    }
  };
/**
           Internal dispatch end-of-list routine.
        */
template <typename T>
struct CtorFwdDispatch<T, XNilType>
  {
  typedef typename XScriptTypeInfo<T>::NativeHandle ReturnType;
  static ReturnType Call( XScriptArguments const & )
    {
    return 0;
    }
  };
/**
           Internal type to dispatch a XScriptArguments list to one of
           several a bound native constructors, depending on on the
           argument count.

           List MUST be a XSignature< ... > containing ONLY
           CtorFowarder types (or compatible).
        */
template <typename T,typename List>
struct CtorFwdDispatchList
  {
  typedef typename XScriptTypeInfo<T>::NativeHandle ReturnType;
  /**
               Tries to dispatch Arguments to one of the constructors
               in the List type, based on the argument count.
             */
  static ReturnType Call( XScriptArguments const &  argv )
    {
    typedef typename List::Head CTOR;
    typedef typename List::Tail Tail;
    enum { Arity = (0==sl::Index<XScriptArguments const &,CTOR>::Value)
           ? -1 : sl::Length<CTOR>::Value
           };
    return ( (Arity < 0) || (Arity == argv.length()) )
        ? CtorFwdDispatch< T, CTOR >::Call(argv )
        : CtorFwdDispatchList<T,Tail>::Call(argv);
    }
  };
/**
           End-of-list specialization.
        */
template <typename T>
struct CtorFwdDispatchList<T,XNilType>
  {
  typedef typename XScriptTypeInfo<T>::NativeHandle ReturnType;
  /** Writes an error message to errmsg and returns 0. */
  static ReturnType Call( XScriptArguments const &  argv )
    {
    QString error = QString("No native constructor was defined for %1 arguments!\n").arg(argv.length());
    throw std::range_error(error.toUtf8().data());
    return 0;
    }
  };
}
#endif // !DOXYGEN

/**
        Proxies a list of constructors from XScriptArguments.

        CtorList must be-a a Signature type in this form:

        @code
        typedef XSignature<MyType ( // may optionally be (MyType *) - same effect
            CtorForwarder<MyType *()>,
            CtorForwarder<MyType *(char const *)>,
            CtorForwarder<MyType *( int, double )>,
            CtorForwarder<MyType *( XScriptArguments const &)>
        )> Ctors;
        @endcode

        All entries in the typelist must be interface-compatible with
        CtorForwarder. No two entries should have the same number
        of arguments with one exception: an InvocationCallback-like
        function taking (XScriptArguments const &) can be used as a
        catch-all for any number of arguments. If used, it must be
        the LAST entry because it will always match any argument
        count (and will therefore trump any which (would) come after
        it.

        The ctors are dispatched based solely on the argument count,
        not their types. The first one with a matching arity
        is called.

        IN THEORY (untested), the factories passed in the list may
        legally return a type publically derived from
        CtorList::ReturnType.
    */
template <typename CtorList>
struct CtorArityDispatcher
  {
  typedef typename CtorList::ReturnType RT;
  typedef typename XScriptTypeInfo<RT>::NativeHandle NativeHandle;
  static NativeHandle Call( XScriptArguments const & argv )
    {
    typedef typename XPlainType<RT>::Type Type;
    typedef Detail::CtorFwdDispatchList<Type, CtorList> Proxy;
    return Proxy::Call( argv );
    }
  };

template <typename T, typename Wrapper, void Dtor(XPersistentScriptValue, void *)>
struct CtorFunctionWrapperBase
  {
  typedef XScript::ClassCreator_Factory<T> Factory;

  static void CallDart(XScriptDartArguments argv)
    {
    XScriptDartArgumentsWithThis args(argv);
    XScriptValue ths = args.calleeThis();
    if( !ths.isValid() )
      {
      return;
      }

    XPersistentScriptValue persistent(ths);
    XScriptObject self(persistent.asValue());

    void *constructed = Wrapper::Wrap(args);
    if(!constructed)
      {
      Toss("Native constructor failed");
      }

    typedef typename XScriptTypeInfo<T>::NativeHandle NativeHandle;
    NativeHandle native = static_cast<NativeHandle>(constructed);

    persistent.makeWeak( constructed, Dtor );
    ::findInterface<T>(native)->wrapInstance(self, constructed);
    }
  };

template <typename T, typename CtorType, void Dtor(XPersistentScriptValue, void *)>
struct CtorFunctionWrapper : CtorFunctionWrapperBase<T, CtorFunctionWrapper<T, CtorType, Dtor>, Dtor>
  {
  typedef XScript::CtorForwarder<CtorType> Forwarder;
  enum { Arity = Forwarder::Arity };

  template <typename ArgsType>
  static T *Wrap(ArgsType argv)
    {
    return Forwarder::Call(argv);
    }
  };


template <typename T, void Dtor(XPersistentScriptValue, void *)>
    struct CtorNativeWrap : XScript::CtorFunctionWrapperBase<T, CtorNativeWrap<T, Dtor>, Dtor>
  {
  enum { Arity = 1 };

  template <typename ArgType> static void *Wrap(ArgType argv)
    {
    xAssert(argv.length() == 1);
    return argv.at(0).toExternal();
    }
  };

} // namespaces

#endif // XSCRIPRCONSTRUCTORS_H
