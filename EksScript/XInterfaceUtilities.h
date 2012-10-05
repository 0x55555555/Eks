#ifndef XINTERFACEUTILITIES_H
#define XINTERFACEUTILITIES_H

#include <stdexcept>
#include "XConvertFromScript.h"
#include "XScriptConstructors.h"
#include "XScriptObject.h"

namespace XScript
{

#if 0
/**
       Convenience base type for ClassCreator_InternalFields
       implementations.

       See the member documentation for the meaning of
       HowMany and Index.

       If any of the following conditions are met then
       a compile-time assertion is triggered:

       - (ObjectIndex<0)

       - (ObjectIndex>=HowMany)

       - (TypeIndex>=HowMany).

       - (TypeIndex == ObjectIndex)

        TypeIndex may be negative, which indicates to ClassCreator that the
        binding should not store type ID information. However, if it is
        negative then T must not be used together with
        JSToNative_ObjectWithInternalFieldsTypeSafe - doing so will trigger
        a compile-time assertion.
    */
template <typename T, int HowMany = 2, int TypeIndex = 0, int ObjectIndex = 1>
struct ClassCreator_InternalFields_Base
  {
  /**
           Total number of internal fields assigned to JS-side T
           objects.
        */
  static const int Count = HowMany;

  /**
           The internal field index at which ClassCreator policies should
           expect the native object to be found in any given JS object.
           It must be 0 or greater, and must be less than Value.
        */
  static const int NativeIndex = ObjectIndex;

  /**
            The internal field index at which ClassCreator policies
            should expect a type identifier tag to be stored.
            This can be used in conjunction with
            JSToNative_ObjectWithInternalFieldsTypeSafe (or similar)
            to provide an extra level of type safety at JS runtime.

        */
  static const int TypeIDIndex = TypeIndex;
private:
  typedef char AssertFields[
  (HowMany > TypeIndex)
  && (HowMany > ObjectIndex)
  && (TypeIndex != ObjectIndex)
  && (ObjectIndex >= 0)
  ? 1 : -1];
  };
#endif

#if !defined(DOXYGEN)
namespace Detail
{
/**
           A base class for ClassCreator_Factory_CtorArityDispatcher.
           We don't really need this level of indirection, i think.
        */
template <typename T>
struct Factory_CtorForwarder_Base
  {
  typedef typename TypeInfo<T>::Type Type;
  typedef typename TypeInfo<T>::NativeHandle NativeHandle;
  static void Delete( NativeHandle nself )
    {
    delete nself;
    }
  };
}

#endif // !DOXYGEN

#if 0
/**
        Can be used as a concrete ClassCreator_Factor<T>
        specialization to forward JS ctor calls directly to native
        ctors.

        T must be the ClassCreator'd type to construct. CtorProxy must
        be a type having this interface:

        @code
        TypeInfo<T>::NativeHandle Call( XScriptArguments const & );
        @endcode

        Normally CtorProxy would be CtorForwarder or CtorArityDispatcher,
        but any interface-compatible type will do.

        It must return a new object instance on success. On error it
        may return NULL and "should" throw a native exception explaining
        the problem. The exception will be caught by ClassCreator and
        transformed into a JS-side exception.

        If CtorProxy::Call() succeeds (returns non-NULL and does not throw)
        then NativeToJSMap<T> is used to create a native-to-JS mapping.
        To make use of this, the client should do the following:

        @code
        // in the namespace:
        template <>
            struct NativeToJS<T> : NativeToJSMap<T>::NativeToJSImpl {};
        @endcode

        After that, CastToJS<T>( theNativeObject ) can work.

        The mapping is cleaned up when (if!) the object is sent through
        the JS garbage collector or the client somehow triggers its
        JS-aware destruction (e.g. via ClassCreator::DestroyObject(),
        assuming the type was wrapped using ClassCreator).
    */
template <typename T, typename CtorProxy>
struct ClassCreator_Factory_NativeToJSMap : Detail::Factory_CtorForwarder_Base<T>
  {
public:
  typedef XNativeToJSMap<T> N2JMap;
  typedef typename TypeInfo<T>::Type Type;
  typedef typename TypeInfo<T>::NativeHandle NativeHandle;

  /**
            If CtorProxy::Call(argv) succeeds, N2JMap::Insert(jself, theNative)
            is called. The result of CtorProxy::Call() is returned.
        */
  static NativeHandle Create( XScriptObject jself, XScriptArguments const &  argv )
    {
    NativeHandle n = CtorProxy::Call( argv );
    if( n ) N2JMap::Insert( jself, n );
    return n;
    }
  /**
            Calls N2JMap::Remove( nself ) then (delete nself).
        */
  static void Delete( NativeHandle nself )
    {
    N2JMap::Remove( nself );
    delete nself;
    }
  };
#endif

#if 0
/**
        A ClassCreator_Factory implementation which forwards its Create()
        member to CtorT::Call() (the interface used by CtorForwarder and friends).

        T must (or is assumed to) be a ClassCreator<T>-wrapped class.
        CtorForwarderList must be a Signature typelist of CtorForwarder
        types and its "return type" must be T (optionally pointer-qualified).

        Example:

        @code
        typedef CtorForwarder<MyType *()> C0;
        typedef CtorForwarder<MyType *(int)> C1;
        typedef CtorForwarder<MyType *(int, double)> C2;
        typedef XSignature< CFT (C0, C1, C2) > CtorList;

        // Then create Factory specialization based on those:
        template <>
        struct ClassCreator_Factory<MyType> :
            ClassCreator_Factory_Dispatcher<MyType, CtorArityDispatcher<CtorList> > {};
        @endcode

        Or:

        @code
        template <>
        struct ClassCreator_Factory<MyType> :
            ClassCreator_Factory_Dispatcher< MyType, CtorForwarder<MyType *()> >
        {};
        @endcode
    */
template <typename T,typename CtorT>
struct ClassCreator_Factory_Dispatcher : Detail::Factory_CtorForwarder_Base<T>
  {
public:
  typedef typename TypeInfo<T>::Type Type;
  typedef typename TypeInfo<T>::NativeHandle NativeHandle;
  static NativeHandle Create( XScriptObject jself, XScriptArguments const &  argv )
    {
    return CtorT::Call( argv );
    }
  };

/**
       A special-case factory implementation for use when T
       is abstract or otherwise should not be instantiable
       from JS code. It has one or two obscure uses when binding
       certain class hierarchies.
    */
template <typename T>
struct ClassCreator_Factory_Abstract : Detail::Factory_CtorForwarder_Base<T>
  {
public:
  typedef typename TypeInfo<T>::Type Type;
  typedef typename TypeInfo<T>::NativeHandle NativeHandle;
  /**
           Always returns NULL.
        */
  static NativeHandle Create( XScriptObject jself, XScriptArguments const &  argv )
    {
    return NULL;
    }
  };
}
#endif

namespace Convert
{

namespace internal
{

template <typename T> struct JSToNativeObject
  {
public:
  typedef typename TypeInfo<T>::NativeHandle ResultType;

  ResultType operator()(Value const &h) const
    {
    if(h.isObject())
      {
      const xsize TypeID = findInterface<T>((const T*)0)->typeId();

      Object obj(h);

      void const *tid = obj.internalField(TypeId);
      if((xsize)tid == TypeID)
        {
        return static_cast<ResultType>(obj.internalField(NativePointer));
        }
      }

    return 0;
    }
  };



template <typename T, typename BASE> struct JSToNativeObjectInherited
  {
public:
  typedef typename TypeInfo<T>::NativeHandle ResultType;

  ResultType operator()(Value const &h) const
    {
    if(h.isObject())
      {
      const xsize TypeID = findInterface<BASE>((const BASE*)0)->typeId();

      Object obj(h);

      void const *tid = obj.internalField(TypeId);
      if((xsize)tid == TypeID)
        {
        void *ext = static_cast<ResultType>(obj.internalField(NativePointer));
        return Convert::castFromBase<T>(static_cast<BASE*>(ext));
        }
      }
    return 0;
    }
  };
}

}

template <typename T> Object Object::newInstance(Interface<T>* i)
  {
  return i->newInstance(0, NULL);
  }

} // namespaces

#endif // XINTERFACEUTILITIES_H
