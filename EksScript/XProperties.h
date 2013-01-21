#ifndef XPROPERTIES_H
#define XPROPERTIES_H

#include "XConvert.h"
#include "XFunctions.h"
#include "XConvertFromScript.h"
#include "XInterface.h"

namespace XScript
{

/**
        Marker class, primarily for documentation purposes.

        This class models the v8::AccessorGetter interface, and
        XyzToGetter classes inherit this type as a sign that they
        implement this interface.

        This class has no implemention - it only exists for
        documentation purposes.
    */
struct XAccessorGetterType
  {
  /**
            The v8::AccessorGetter() interface.
        */
  static Value Get(Value property, const internal::JSAccessorInfo &info);
  };

/**
        Marker class, primarily for documentation purposes.

        This class models the v8::AccessorSetter interface, and
        XyzToSetter classes inherit this type as a sign that they
        implement this interface.

        This class has no useful implemention - it only exists for
        documentation purposes.
    */
struct XAccessorSetterType
  {
  /** The v8::AccessorSetter() interface. */
  static void Set(Value property, Value value, const internal::JSAccessorInfo& info);
  };

typedef XAccessorGetterType Getter;
typedef XAccessorSetterType Setter;

/**
        A tag type for use with VarTo, MemberTo, MethodTo, and FunctionTo.
    */
struct Accessors : XAccessorGetterType, XAccessorSetterType {};

/**
        Wraps a v8:::AccessorGetter into the AccessorGetterType
        interface, so that we can use plain AccessorGetters in certain
        templating contexts. e.g. this can be useful if we want to
        wrap a custom v8::AccessorGetter in a GetterCatcher.
    */
template <XScript::GetterFn G>
struct GetterToGetter : XAccessorGetterType
  {
  /** Implements the v8::AccessorGetter() interface. */
  inline static Value Get(Value property, const internal::JSAccessorInfo &info)
    {
    return G(property, info);
    }
  };

/**
        Wraps a v8:::AccessorSetter into the AccessorSetterType
        interface, so that we can use plain AccessorSetters in certain
        templating contexts. e.g. this can be useful if we want to
        wrap a custom v8::AccessorSetter in a SetterCatcher.
    */
template <XScript::SetterFn S>
struct SetterToSetter : XAccessorSetterType
  {
  /** Implements the v8::AccessorSetter() interface. */
  inline static void Set(Value property, Value value, const internal::JSAccessorInfo& info)
    {
    S(property, value, info);
    }
  };

/**
       This template create an v8::AccessorGetter from a static/shared
       variable.

       SharedVar must be pointer to a static variable and must not
       be NULL.

       CastToJS(*SharedVar) must be legal.
    */
template <typename PropertyType, PropertyType * const SharedVar>
struct VarToGetter : XAccessorGetterType
  {
  /** Implements the v8::AccessorGetter() interface. */
  inline static Value Get(Value property, const internal::JSAccessorInfo &info)
    {
    return CastToJS( *SharedVar );
    }
  };

/**
       The setter counterpart of StaticVarToGetter().

       SharedVar must be pointer to a static variable and must not
       be NULL.

       (*SharedVar = Convert::from<PropertyType>()) must be legal.

       Reminder: this is not included in the StaticVarToGetter
       template so that we can avoid either the Get or Set
       conversion for cases where it is not legal (or not desired).
       If they were both in one class, both Get and Set would _have_
       to be legal.
    */
template <typename PropertyType, PropertyType * const SharedVar>
struct VarToSetter : XAccessorSetterType
  {
  /** Implements the v8::AccessorSetter() interface. */
  inline static void Set(Value property, Value value, const internal::JSAccessorInfo& info)
    {
    *SharedVar = Convert::from<PropertyType>( value );
    }
  };

/**
        A proxy for both VarToGetter and VarToSetter, providing both
        Get() and Set() functions.
    */
template <typename PropertyType, PropertyType * const SharedVar>
struct VarToAccessors : VarToGetter<PropertyType,SharedVar>,
    VarToSetter<PropertyType,SharedVar>
  {};

/**
       This template creates a v8::AcessorGetter which binds directly to
       a non-const native class member variable.

       Requirements:

       - T must be convertible to (T*) via Convert::from<T>().
       - MemVar must be an accessible member of T.
       - PropertyType must be convertible via CastToJS<PropertyType>().

       If the underlying native 'this' object cannot be found (that
       is, if Convert::from<T>() fails) then this routine will
       trigger a JS exception.
    */
template <typename T, typename PropertyType, PropertyType T::*MemVar>
struct MemberToGetter : XAccessorGetterType
  {
  /** Implements the v8::AccessorGetter() interface. */
  inline static Value Get(Value property, const internal::JSAccessorInfo &info)
    {
    typedef typename TypeInfo<T>::Type Type;
    typedef typename Convert::internal::JSToNative<T>::ResultType NativeHandle;
    NativeHandle self = Convert::from<T>( info.calleeThis() );
    return ( ! self )
        ? toss( "Native member property getter '#' could not access native 'this' object!" )
        : Convert::to( (self->*MemVar) );
    }
  };

/**
       This is the Setter counterpart of MemberToGetter.

       Requirements:

       - T must be convertible to (T*) via Convert::from<T>().
       - PropertyType must be convertible via CastToJS<PropertyType>().
       - MemVar must be an accessible member of T.

       If the underlying native This object cannot be found then this
       routine will trigger a JS exception.
    */
template <typename T, typename PropertyType, PropertyType T::*MemVar>
struct MemberToSetter : XAccessorSetterType
  {
  /** Implements the v8::AccessorSetter() interface. */
  inline static void Set(Value property, Value value, const internal::JSAccessorInfo& info)
    {
    typedef typename TypeInfo<T>::Type Type;
    typedef typename Convert::internal::JSToNative<T>::ResultType NativeHandle;
    NativeHandle self = Convert::from<T>( info.calleeThis() );
    if( self ) self->*MemVar = Convert::from<PropertyType>( value );
    else toss( "Native member property setter '#' could not access native 'this' object!" );
    }
  };

/**
        A proxy for both MemberToGetter and MemberToSetter, providing both
        Get() and Set() functions.

        This should be called MembersToAccessors (plural Members).
    */
template <typename T, typename PropertyType, PropertyType T::*MemVar>
struct MemberToAccessors : MemberToGetter<T,PropertyType,MemVar>,
    MemberToSetter<T,PropertyType,MemVar>
  {};

/**
       An AccessorSetter() implementation which always triggers a JS exception.
       Can be used to enforce "pedantically read-only" variables. Note that
       JS does not allow us to assign an AccessorSetter _without_ assigning
       an AccessorGetter. Also note that there is no AccessorGetterThrow,
       because a write-only var doesn't make all that much sense (use
       methods for those use cases).
    */
struct ThrowingSetter : XAccessorSetterType
  {
  inline static void Set(Value, Value, const internal::JSAccessorInfo &)
    {
    //toss(QString("Native member property setter '%1' is configured to throw an exception when modifying this read-only member!").arg(Convert::from<QString>(property)));
    }
  };

/**
       Implements the v8::AccessorGetter interface to bind a JS
       member property to a native getter function. This function
       can be used as the getter parameter to
       v8::ObjectTemplate::SetAccessor().

       Sig must be a function-signature-style type and Getter must
       capable of being called with no arguments and returning a
       value which can be CastToJS() to a JS value.

       If Getter() throws a native exception it is converted to a JS
       exception.
    */
template <typename Sig, typename XFunctionSignature<Sig>::FunctionType Getter>
struct FunctionToGetter : XAccessorGetterType
  {
  inline static Value Get( Value property, const internal::JSAccessorInfo & info )
    {
    return CastToJS( (*Getter)() );
    }
  };

/**
       Implements the v8::AccessorSetter interface to bind a JS
       member property to a native getter function. This function
       can be used as the getter parameter to
       v8::ObjectTemplate::SetAccessor().

       SigSet must be function-signature-style pattern
       for the setter function. The native
       function must follow conventional mutator signature:

       ReturnType ( PropertyType )

       PropertyType may differ from the return type. PropertyType
       may not be void but the ReturnType may be. Any return value
       from the setter is ignored by the JS engine.

       Note that the v8 API appears to not allow us to just set
       a setter, but not a getter. We have to set a getter without
       a setter, a getter with a setter, or neither. At least that's
       been my experience.

       If Setter() throws a native exception it is converted to a JS
       exception.
    */
template <typename Sig, typename XFunctionSignature<Sig>::FunctionType Func>
struct FunctionToSetter : XAccessorSetterType
  {
  inline static void Set( Value property, Value value, const internal::JSAccessorInfo &info)
    {
    typedef XFunctionSignature<Sig> FT;
    (*Func)( Convert::from<typename sl::At<0,FT>::Type>( value ) );
    }
  };


/**
       Implements the v8::AccessorGetter interface to bind a JS
       member property to a native getter function. This function
       can be used as the getter parameter to
       v8::ObjectTemplate::SetAccessor().

       Sig must be a function-pointer-style argument with a
       non-void return type convertible to v8 via CastToJS(), and
       Getter must be function capable of being called with 0
       arguments (either because it has none or they have
       defaults).
    */
template <typename T, typename Sig, typename XMethodSignature<T,Sig>::FunctionType Getter>
struct XMethodToGetter : XAccessorGetterType
  {
  typedef typename TypeInfo<T>::Type Type;
  typedef typename Convert::internal::JSToNative<T>::ResultType NativeHandle;

  inline static Value Get( Value property, const internal::JSAccessorInfo & info )
    {
    NativeHandle self = Convert::from<T>( info.calleeThis() );
    return self
        ? Convert::to( (self->*Getter)() )
        : toss( QString("Native member property getter '%1' could not access native This object!").arg(Convert::from<QString>(property)) );
    }
  inline static void GetDart(internal::DartArguments argv)
    {
    internal::DartArgumentsWithThis args(argv);
    xAssert(args.length() == 0);
    NativeHandle const self = Convert::from<T>( args.calleeThis() );
    if(!self)
      {
      toss(QString("Native member property getter '%1' could not access native This object!"));
      }

    argv.setReturnValue(Convert::to( (self->*Getter)() ));
    }
  };

/**
       Overload for const native getter functions.
    */
template <typename T, typename Sig, typename XConstMethodSignature<T,Sig>::FunctionType Getter>
struct XConstMethodToGetter : XAccessorGetterType
  {
  typedef typename TypeInfo<T>::Type Type;
  typedef typename Convert::internal::JSToNative<T>::ResultType NativeHandle;

  inline static Value Get( Value property, const internal::JSAccessorInfo & info )
    {
    NativeHandle const self = Convert::from<T>( info.calleeThis() );
    return self
        ? Convert::to( (self->*Getter)() )
        : toss(QString("Native member property getter '%1' could not access native This object!").arg(Convert::from<QString>(property)));
    }
  inline static void GetDart( internal::DartArguments argv )
    {
    internal::DartArgumentsWithThis args(argv);
    xAssert(args.length() == 0);
    NativeHandle const self = Convert::from<T>( args.calleeThis() );
    if(!self)
      {
      toss(QString("Native member property getter '%1' could not access native This object!"));
      }

    argv.setReturnValue(Convert::to( (self->*Getter)() ));
    }
  };

template <typename T, typename InputArg, typename XMethodSignature<T,void (InputArg)>::FunctionType Setter>
struct XMethodToSetter : XAccessorSetterType
  {
  typedef typename TypeInfo<T>::Type Type;
  typedef typename Convert::internal::JSToNative<T>::ResultType NativeHandle;

  static void Set(Value property, Value value, const internal::JSAccessorInfo &info)
    {
    NativeHandle self = Convert::from<NativeHandle>( info.calleeThis() );
    if( ! self )
      {
      toss( QString("Native member property setter '%1' could not access native This object!").arg(Convert::from<QString>(property)) );
      }
    else
      {
      typedef typename TypeInfo<InputArg>::NativeHandle NativeHandle;
      typedef typename Convert::internal::JSToNative<InputArg>::ResultType Result;
      Result handle = Convert::from<InputArg>( value );

      bool valid = true;
      InputArg in = Convert::TypeMatcher<InputArg, Result>::match(&handle, valid);
      if(!valid)
        {
        toss(QString("Native member property setter '%1' could convert input argument!").arg(Convert::from<QString>(property) ));
        }
      else
        {
        (self->*Setter)( in );
        }
      }
    }

  static void SetDart(internal::DartArguments argv)
    {
    internal::DartArgumentsWithThis args(argv);
    xAssert(args.length() == 1);
    NativeHandle self = Convert::from<NativeHandle>( args.calleeThis() );
    if(!self)
      {
      toss( QString("Native member property setter '%1' could not access native This object!") );
      }
    else
      {
      typedef typename TypeInfo<InputArg>::NativeHandle NativeHandle;
      typedef typename Convert::internal::JSToNative<InputArg>::ResultType Result;
      Result handle = Convert::from<InputArg>( args.at(0) );

      bool valid = true;
      InputArg in = Convert::TypeMatcher<InputArg, Result>::match(&handle, valid);
      if(!valid)
        {
        toss(QString("Native member property setter '%1' could convert input argument!") );
        }
      else
        {
        (self->*Setter)( in );
        }
      }
    }
  };

template <typename Ftor, typename Sig>
struct XFunctorToGetter
  {
  inline static Value Get( Value property, const internal::JSAccessorInfo & info )
    {
    //const static Ftor f();
    return CastToJS(Ftor()());
    }
  };

template <typename Ftor, typename Sig>
struct XFunctorToSetter
  {
  inline static void Set(Value property, Value value, const internal::JSAccessorInfo &info)
    {
    typedef typename sl::At< 0, XSignature<Sig> >::Type ArgT;
    Ftor()( Convert::from<ArgT>( value ) );
    }
  };

/**
        SetterCatcher is the AccessorSetter equivalent of InCaCatcher, and
        is functionality identical except that its 4th template parameter
        must be-a AccessorSetterType instead of an InCa type.
    */
template < typename ExceptionT,
           typename SigGetMsg,
           typename XConstMethodSignature<ExceptionT,SigGetMsg>::FunctionType Getter,
           typename SetterT,
           bool PropagateOtherExceptions = false
           >
struct XSetterCatcher : XAccessorSetterType
  {
  static void Set(Value property, Value value, const internal::JSAccessorInfo &info)
    {
    try
    {
    SetterT::Set( property, value, info );
    }
    catch( ExceptionT const & e2 )
    {
      toss((e2.*Getter)());
      }
    catch( ExceptionT const * e2 )
    {
      toss((e2->*Getter)());
      }
    catch(...)
    {
    if( PropagateOtherExceptions ) throw;
    else toss("Unknown native exception thrown!");
    }
    }
  };

/**
        A convenience form of SetterCatcher which catches std::exception
        and subtyes. See InCaCatcher_std for full details - this type is
        identical except that its first template parameter must be-a
        AccessorSetterType instead of InCa type.
    */
template <
    typename SetterT,
    typename ConcreteException = std::exception,
    bool PropagateOtherExceptions = !XSameType< std::exception, ConcreteException >::Value
    >
struct XSetterCatcher_std :
    XSetterCatcher<ConcreteException,
    char const * (),
&ConcreteException::what,
SetterT,
PropagateOtherExceptions
>
  {};

/**
        GetterCatcher is the AccessorSetter equivalent of InCaCatcher, and
        is functionality identical except that its 4th template parameter
        must be-a AccessorGetterType instead of an InCa type.
    */
template < typename ExceptionT,
           typename SigGetMsg,
           typename XConstMethodSignature<ExceptionT,SigGetMsg>::FunctionType Getter,
           typename GetterT,
           bool PropagateOtherExceptions = false
           >
struct XGetterCatcher : XAccessorGetterType
  {
  static Value Get( Value property, const internal::JSAccessorInfo & info )
    {
    try
    {
    return GetterT::Get( property, info );
    }
    catch( ExceptionT const & e2 )
    {
      return toss(CastToJS((e2.*Getter)()));
      }
    catch( ExceptionT const * e2 )
    {
      return toss(CastToJS((e2->*Getter)()));
      }
    catch(...)
    {
    if( PropagateOtherExceptions ) throw;
    else return toss("Unknown native exception thrown!");
    }
    }
  };

/**
        A convenience form of GetterCatcher which catches std::exception
        and subtyes. See InCaCatcher_std for full details - this type is
        identical except that its first template parameter must be-a
        AccessorGetterType instead of InCa type.
    */
template <
    typename GetterT,
    typename ConcreteException = std::exception,
    bool PropagateOtherExceptions = !XSameType< std::exception, ConcreteException >::Value
    >
struct XGetterCatcher_std :
    XGetterCatcher<ConcreteException,
    char const * (),
&ConcreteException::what,
GetterT,
PropagateOtherExceptions
>
  {};

template <typename T, typename Sig, typename XMethodSignature<T,Sig>::FunctionType Getter>
struct XMethodToIndexedGetter : XAccessorGetterType
  {
  inline static Value Get( xuint32 property, const internal::JSAccessorInfo &info )
    {
    typedef typename TypeInfo<T>::Type Type;
    typedef typename Convert::internal::JSToNative<T>::ResultType NativeHandle;
    NativeHandle self = Convert::from<T>( info.calleeThis() );
    if(self)
      {
      return Convert::to( (self->*Getter)(property) );
      }
    return Value::newEmpty();
    }
  inline static void GetDart(internal::DartArguments argv)
    {
    internal::DartArgumentsWithThis args(argv);
    typedef typename TypeInfo<T>::Type Type;
    typedef typename Convert::internal::JSToNative<T>::ResultType NativeHandle;
    NativeHandle self = Convert::from<T>( args.calleeThis() );
    if(self)
      {
      argv.setReturnValue(Convert::to( (self->*Getter)(Convert::from<xuint32>(args.at(0))) ));
      }
    }
  };

template <typename T, typename Sig, typename XMethodSignature<T,Sig>::FunctionType Getter>
struct XMethodToNamedGetter : XAccessorGetterType
  {
  inline static Value Get( Value property, const internal::JSAccessorInfo &info )
    {
    typedef typename TypeInfo<T>::Type Type;
    typedef typename Convert::internal::JSToNative<T>::ResultType NativeHandle;
    NativeHandle self = Convert::from<T>( info.calleeThis() );
    if(self)
      {
      typename XMethodSignature<T,Sig>::ReturnType rt = (self->*Getter)(Convert::from<QString>(property));
      if(rt)
        {
        return Convert::to(rt);
        }
      }
    return Value::newEmpty();
    }
  };


#if 0
/**
        AccessAdder is a convenience class for use when applying several
        (or more) accessor bindings to a prototype object.

        Example:

        @code
        AccessorAdder acc(myPrototype);
        acc("foo", MemberToGetter<MyType,int,&MyType::anInt>(),
                   MemberToSetter<MyType,int,&MyType::anInt>())
           ("bar", MethodToGetter<const MyType,int(),&MyType:getInt>(),
                   MethodToSetter<MyType,void(int),&MyType:setInt>())
           ...
           ;
        @endcode
    */
class AccessorAdder
  {
private:
  v8::Handle<v8::ObjectTemplate> const & proto;

  /** A "null" setter which does nothing. Used only as default
            argument to operator(). Its Set() is not actually used
            but this type is used as a placeholder for a NULL
            v8::AccessorSetter.
        */
  struct NullSetter
    {
    /** The v8::AccessorSetter() interface. */
    static void Set(Value, Value, const internal::JSAccessorInfo &)
      {
      }
    };
public:
  /**
            Initializes this object so that calls to operator() will
            apply to p. p must out-live this object. More specifically,
            operator() must not be called after p has been destroyed.
        */
  explicit AccessorAdder( v8::Handle<v8::ObjectTemplate> const & p )
    : proto(p)
    {}
  AccessorAdder const & operator()( char const * name,
                                    v8::AccessorGetter g,
                                    v8::AccessorSetter s = NULL,
                                    v8::Handle< v8::Value > data=v8::Handle< v8::Value >(),
                                    v8::AccessControl settings=v8::DEFAULT,
                                    v8::PropertyAttribute attribute=v8::None) const
    {
    proto->SetAccessor(v8::String::New(name), g, s, data, settings, attribute);
    return *this;
    }
  /**
            Adds GetterT::Get and SetterT::Set as accessors for the
            given property in the prototype object.

            GetterT must be-a AccessorGetterType. SetterT must be-a
            AccessorSetterType. Note that their values are not used,
            but GetterT::Get and SetterT::Set are used
            directly. The objects are only passed in to keep the
            client from having to specify them as template
            parameters (which is clumsy for operator()), as their
            types can be deduced.

            The 3rd and higher arguments are as documented (or not!)
            for v8::ObjectTemplate::SetAccessor().

            Returns this object, for chaining calls.
        */
  template <typename GetterT, typename SetterT>
  AccessorAdder const & operator()( char const * name,
                                    GetterT const &,
                                    SetterT const & = NullSetter(),
                                    v8::Handle< v8::Value > data=v8::Handle< v8::Value >(),
                                    v8::AccessControl settings=v8::DEFAULT,
                                    v8::PropertyAttribute attribute=v8::None) const
    {
    // jump through a small hoop to ensure identical semantics vis-a-vis
    // the other overload.
    return this->operator()( name, GetterT::Get,
                             XSameType<NullSetter,SetterT>::Value ? NULL : SetterT::Set,
                             data, settings, attribute);
    }
  };
#endif

} // namespaces

#endif // XPROPERTIES_H
