#ifndef XINTERFACE_H
#define XINTERFACE_H

#include "XScriptGlobal.h"
#include "XConvertToScript.h"
#include "XProperties.h"
#include "XScriptInterfaceBase.h"

namespace XScript
{

template <typename T> class Interface : public InterfaceBase
  {
public:
  typedef typename XPlainType<T>::Type Type;

  void wrapInstance(Object object, T *native) const
    {
    InterfaceBase::wrapInstance(object, (void*)native);
    }

  void unwrapInstance(Object object) const
    {
    return InterfaceBase::unwrapInstance(object);
    }

  template <typename ValueT> inline void set(char const *name, ValueT val)
    {
    set(name, Convert::to(val));
    }

  X_CONST_EXPR ConstructorDef defaultConstructor()
    {
    return constructor<T *()>();
    }

  X_CONST_EXPR ConstructorDef copyConstructor()
    {
    return constructor<T *(const T&)>();
    }

  X_CONST_EXPR ConstructorDef nativeConstructor()
    {
    ConstructorDef d =
      {
      "_internal",
      0,
      CtorNativeWrap<T, Interface<T>::weak_dtor>::CallDart,
      1
      };

    return d;
    }

  template <typename TYPE>
      X_CONST_EXPR ConstructorDef constructor(const char *name="")
    {
    typedef CtorFunctionWrapper<T, TYPE, Interface<T>::weak_dtor> Wrapper;

#ifdef X_SCRIPT_ENGINE_ENABLE_JAVSCRIPT
### err, js ctors?
#endif

    return ConstructorDef {
      name,
      0,
      Wrapper::CallDart,
      Wrapper::Arity
      };
    }

  template <typename TYPE,
            typename XConstMethodSignature<T, TYPE ()>::FunctionType GETTERMETHOD,
            typename XMethodSignature<T, void (TYPE)>::FunctionType SETTERMETHOD>
      X_CONST_EXPR static PropertyDef property(const char *name)
    {
    return property<TYPE, TYPE, GETTERMETHOD, SETTERMETHOD>(name);
    }

  template <typename GETTYPE,
            typename SETTYPE,
            typename XConstMethodSignature<T, GETTYPE ()>::FunctionType GETTERMETHOD,
            typename XMethodSignature<T, void (SETTYPE)>::FunctionType SETTERMETHOD>
      X_CONST_EXPR static PropertyDef property(const char *name)
    {
    typedef XConstMethodToGetter<T, GETTYPE (), GETTERMETHOD> Getter;
    typedef XMethodToSetter<T, SETTYPE, SETTERMETHOD> Setter;

    return PropertyDef {
      name,
      Getter::Get,
      Setter::Set,
      Getter::GetDart,
      Setter::SetDart
      };
    }

  template <typename Getter,
            typename Setter>
      X_CONST_EXPR static PropertyDef property(const char *name)
    {
    return PropertyDef {
      name,
      Getter::Get,
      Setter::Set,
      Getter::GetDart,
      Setter::SetDart
      };
    }

  template <typename Getter>
      X_CONST_EXPR static PropertyDef property(const char *name)
    {
    return PropertyDef {
      name,
      Getter::Get,
      0,
      Getter::GetDart,
      0
      };
    }

  template <typename GETTYPE,
            typename XConstMethodSignature<T, GETTYPE ()>::FunctionType GETTERMETHOD>
      X_CONST_EXPR static PropertyDef property(const char *name)
    {
    typedef XConstMethodToGetter<T, GETTYPE (), GETTERMETHOD> Getter;

    return PropertyDef {
      name,
      Getter::Get,
      0,
      Getter::GetDart,
      0
      };
    }

  template <typename GETTYPE,
            typename XMethodSignature<T, GETTYPE ()>::FunctionType GETTERMETHOD>
      X_CONST_EXPR static PropertyDef accessProperty(const char *name)
    {
    typedef XMethodToGetter<T, GETTYPE (), GETTERMETHOD> Getter;

    return PropertyDef {
      name,
      Getter::Get,
      0,
      Getter::GetDart,
      0
      };
    }

  template <typename SIG,
            typename XMethodSignature<T, SIG>::FunctionType METHOD>
      X_CONST_EXPR static FunctionDef method(const char *name)
    {
    typedef MethodToInCa<T, SIG, METHOD> FunctionType;

    return method<FunctionType>(name);
    }

  template <typename SIG,
            typename XConstMethodSignature<T, SIG>::FunctionType METHOD>
      X_CONST_EXPR static FunctionDef constMethod(const char *name)
    {
    typedef ConstMethodToInCa<T, SIG, METHOD> FunctionType;

    return method<FunctionType>(name);
    }

  template <typename FunctionType>
      X_CONST_EXPR static FunctionDef method(const char *name)
    {
    return FunctionDef {
      name,
      FunctionType::Call,
      FunctionType::CallDart,
      FunctionType::Arity,
      false // not static method
      };
    }

  template <typename FunctionType>
      static FunctionDef function(const char *name)
    {
    return FunctionDef {
      name,
      FunctionType::Call,
      FunctionType::CallDart,
      FunctionType::Arity,
      true // is static method
      };
    }

  template <typename SIG,
            typename XFunctionSignature<SIG>::FunctionType METHOD>
      X_CONST_EXPR static FunctionDef staticMethod(const char *name)
    {
    typedef XScript::FunctionToInCa<SIG, METHOD> FunctionType;

    return function<FunctionType>(name);
    }

//  template <typename RETTYPE, typename XMethodSignature<T, RETTYPE (xsize i)>::FunctionType METHOD>
//  void setIndexAccessor()
//    {
//    IndexedGetter fn = XScript::XMethodToIndexedGetter<T, RETTYPE (xsize i), METHOD>::Get;

//    XInterfaceBase::setIndexAccessor(fn);
//    }

//  template <typename RETTYPE, typename XMethodSignature<T, RETTYPE (const QString &name)>::FunctionType METHOD>
//  void setNamedAccessor()
//    {
//    NamedGetter fn = XScript::XMethodToNamedGetter<T, RETTYPE (const QString &name), METHOD>::Get;

//    XInterfaceBase::setNamedAccessor(fn);
//    }

  /**
     Destroys the given object by disconnecting its associated
     native object and calling the native destructor function
     for it.

     If jo cannot be converted to a T then false is
     returned. Otherwise the true is returned and the native
     object referenced by jo is no longer valid (it should not
     be used by JS code).

     Native functions bound to that object should take care to
     bail out with an exception once the native pointer is gone,
     as opposed to blindly stepping on its null/dangling pointer
     (which _might_ have been re-allocated to a different
     object, even of a different type, in the mean time).
  */
  static bool destroyObject( Object const & jo )
    {
    xAssertFail();
    /*
    T * t = XScriptConvert::from<T>(jo);
    if( ! t ) return false;
    else
      {
      v8::Persistent<v8::Object> p( v8::Persistent<v8::Object>::New( jo ) );
      p.ClearWeak(); // avoid a second call to weak_dtor() via gc!
      weak_dtor( p, t );
      return true;
      }*/
    return true;
    }
  /**
     If jv is empty or !jv->IsObject() then false is returned,
     otherwise it returns the result of
     DestroyObject(Handle<Object>).
  */
  static bool destroyObject( Value const & jv )
    {
    return !jv.isObject() ? false : destroyObject(Object(jv));
    }

#if 0
  /**
     A v8::InvocationCallback implementation which calls
     DestroyObject( argv.calleeThis() ).

     It is intended to be used as a "manual destructor" for
     classes which need it. The canonical examples are
     Stream.close() and Database.close().

     This function is not called DestroyObject to avoid name
     collisions during binding using Set(...,DestroyObjectCallback).
  */
  static Value destroyObjectCallback( XScriptArguments const & argv )
    {
    return destroyObject(argv.calleeThis()) ? true : false;
    }

#endif

  static Interface *create(const QString &name)
    {
    xsize id = (xsize)qMetaTypeId<T*>();
    xsize nonPointerId = (xsize)internal::QMetaTypeIdOrInvalid<T>::id();

    Interface &bob = instance(name, id, nonPointerId, id, nonPointerId, 0);
    registerInterface(&bob);

    xAssert(!bob.isSealed());
    return &bob;
    }

  template <typename PARENT, typename BASE>
  static Interface *createWithParent(const QString &name, const Interface<PARENT> *constParent, const Interface<BASE> *constBase)
    {
    xsize baseId = constBase->typeId();
    xsize baseNonPointerId = (xsize)constBase->nonPointerTypeId();

    xsize id = qMetaTypeId<T*>();
    xsize nonPointerId = internal::QMetaTypeIdOrInvalid<T>::id();

    xAssert(baseId != id);
    xAssert(nonPointerId == 0 || nonPointerId != baseNonPointerId);

    Interface &bob = instance(name, id, nonPointerId, baseId, baseNonPointerId, constParent);

    typedef T* (*TCastFn)(BASE *ptr);
    TCastFn typedFn = Convert::castFromBase<T, BASE>;

    UpCastFn fn = (UpCastFn)typedFn;

    Interface<BASE>* base = const_cast<Interface<BASE>*>(constBase);
    base->addChildInterface(qMetaTypeId<T*>(), fn);

    registerInterface(&bob);

    xAssert(!bob.isSealed());
    return &bob;
    }

  static const Interface *lookup()
    {
    static const Interface *bob = static_cast<Interface<T>*>(findInterface(qMetaTypeId<T*>()));
    xAssert(bob->isSealed());
    return bob;
    }

  Interface(xsize typeId,
    xsize nonPointerTypeId,
    xsize baseTypeId,
    xsize baseNonPointerTypeId,
    const QString &name,
    const InterfaceBase* parent)
    : InterfaceBase(typeId, nonPointerTypeId, baseTypeId, baseNonPointerTypeId,
                                name,
                                parent)
    {
      {
      typedef Value (*TVal)(T * const &);
      TVal val = Convert::to<T*>;

      ToScriptFn unTyped = (ToScriptFn)val;
      setToScript(unTyped);
      }

      {
      typedef T *(*FVal)(Value const &);
      FVal from = Convert::from<T*>;

      FromScriptFn unTyped = (FromScriptFn)from;
      setFromScript(unTyped);
      }
    }


  static void weak_dtor(PersistentValue pv, void * )
    {
    Value val = pv.asValue();
    Object jobj(val);
    typedef typename Factory::ReturnType BT;
    typedef typename Convert::internal::JSToNative<T>::ResultType NT;
    NT native = Convert::from<T>( val );
    //BT base = native;
    if( !native )
      {
      /* see: http://code.google.com/p/v8-juice/issues/detail?id=27

          When i call pv.Dispose(), this function is getting called twice,
          and the second time won't work. i'm going to igore (return w/o
          side-effects) this for now for the sake of avoiding a crash
          which i'm seeing only on 64-bit platforms.

          However, even if i return here, v8 is crashing with a
          !NEAR_DEATH assertion right after the second call is made.

          The extra pair of Dispose()/Clear() calls seems to eliminate that
          crash, but the fact that this code block is hit AT ALL is a
          sign of a problem - the dtor shouldn't be called twice!
          */
      pv.dispose();
#if 1 /* i believe this problem was fixed. If you are reading this b/c
  you followed an assert() message, please report this as a bug.
  */
      assert( 0 && "weak_dtor() got no native object!");
#endif
      return;
      }
    else
      {
      /**
             Reminder: the FindHolder() bits are here to
             assist when the bound native exists somewhere in the
             prototype chain other than jobj itself. In that case,
             jobj is valid but we cannot clear out the native handle
             internal field on it because it has no internal fields
             (or none that belong to us).

             To fix this properly we have to be able to know
             _exactly_ which JS object in the prototype chain nh is
             bound to.
          */
      //Object nholder = FindHolder( jobj, base );
#if 0 /* reminder: i've never actually seen this error happen, i'm just pedantic about checking... */
      assert( ! nholder.isValid() );
      WeakWrap::Unwrap( nholder /*jobj? subtle difference!*/, native );
      if( nholder.isValid() || (nholder->InternalFieldCount() != InternalFields::Count) )
        {
        xAssertFail();
        /*  cvv8::StringBuffer msg;
              msg << "SERIOUS INTERNAL ERROR:\n"
                  << "ClassCreator<T>::weak_dtor() "
                  << "validated that the JS/Native belong together, but "
                  << "FindHolder() returned an "
                  << (nholder.IsEmpty() ? "empty" : "invalid")
                  << " handle!\n"
                  << "From JS=@"<<(void const *)nobj
                  << ", Converted to Native=@"<<(void const *)native
                  << ", nholder field count="<<nholder->InternalFieldCount()
                  << ", jobj field count="<<jobj->InternalFieldCount()
                  << "\nTHIS MAY LEAD TO A CRASH IF THIS JS HANDLE IS USED AGAIN!!!\n"
                  ;*/
        Factory::Delete(native);
        pv.Dispose(); pv.Clear(); /* see comments below!*/
        Toss("SERIOUS INTERNAL ERROR");
        return;
        }
      else
        {
        findInterface<T>(native)->unwrapInstance(nholder);
        Factory::Delete(native);
        }
#else
      findInterface<T>(native)->unwrapInstance(&jobj);
      Factory::Delete(native);
#endif
      }
    /*
        According to the v8 gurus i need to call pv.Dispose()
        instead of pv.Clear(), but if i do then this dtor is
        being called twice. If i don't call it, v8 is crashing
        sometime after this function with a !NEAR_DEATH
        assertion.
      */
    pv.dispose();
    }

private:
  typedef XScript::ClassCreator_Factory<T> Factory;

  static Interface &instance(const QString &name, xsize id, xsize nonPointerId, xsize baseId, xsize baseNonPointerId, const InterfaceBase* parent)
    {
    static Interface bob(id, nonPointerId, baseId, baseNonPointerId,name, parent);
    return bob;
    }

#if 0
  template <typename BASE>
      static Object FindHolder( Object const & jo, BASE const * nh )
    {
    if( !nh || !jo.isValid() ) return Object();
    Value proto(jo);
    void const * ext = NULL;
    typedef XScript::ClassCreator_SearchPrototypeForThis<T> SPFT;
    while( !ext && proto.isValid() && proto.isObject() )
      {
      Object obj(proto);
      ext = (obj.internalFieldCount() != (xsize)InternalFields::Count)
            ? NULL
            : obj.internalField( InternalFields::NativeIndex );
      // FIXME: if InternalFields::TypeIDIndex>=0 then also do a check on that one.
      /*
              If !ext, there is no bound pointer. If (ext &&
              (ext!=nh)) then there is one, but it's not the droid
              we're looking for. In either case, (possibly) check the
              prototype...
          */
      if( ext == nh )
        {
        return obj;
        }
      else if( !SPFT::Value )
        {
        break;
        }
      else
        {
        proto = obj.getPrototype();
        }
      }
    return Object();
    }
#endif
#if 0
  /**
     Gets installed as the NewInstance() handler for T.
   */
  static Value ctor_proxy( XScriptArguments const & argv )
    {
    if(XScript::ClassCreator_AllowCtorWithoutNew<T>::Value)
      {
      /**
             Allow construction without 'new' by forcing this
             function to be called in a ctor context...
          */
      if (!argv.isConstructCall())
        {
        Value val;
        argv.callee().callAsConstructor(&val, argv);
        return val;
        }
      }
    else
      {
      /**
             Why have this limitation? If we don't, v8 pukes
             when the ctor is called, with
             "v8::Object::SetInternalField() Writing internal
             field out of bounds".
          */
      if (!argv.isConstructCall())
        {
        return Toss("This constructor cannot be called as function!");
        }
      }

    /*CastToJS<T>(*nobj)

      We are not yet far enough
      along in the binding that
      CastToJS() can work. And it
      can't work for the generic
      case, anyway.
      */
    Object jobj(argv.calleeThis());
    if( !jobj.isValid() )
      {
      return jobj /* assume exception*/;
      }

    XPersistentScriptValue persistent(jobj);
    Object self(persistent.asValue());
    typename Factory::ReturnType nobj = NULL;
    try
      {
      nobj = Factory::Create( self, argv );
      if( ! nobj )
        {
        return toss("Native contructor failed");
        }

      typedef typename TypeInfo<T>::NativeHandle NativeHandle;
      NativeHandle native = static_cast<NativeHandle>(nobj);
      persistent.makeWeak( nobj, weak_dtor );
      findInterface<T>(native)->wrapInstance(self, nobj);
      }
    catch(std::exception const &ex)
      {
      typedef typename TypeInfo<T>::NativeHandle NativeHandle;
      NativeHandle native = static_cast<NativeHandle>(nobj);
      if( nobj ) Factory::Delete( native );
      persistent.dispose();
      return Toss(ex.what());
      }
    catch(...)
      {
      typedef typename TypeInfo<T>::NativeHandle NativeHandle;
      NativeHandle native = static_cast<NativeHandle>(nobj);
      if( nobj ) Factory::Delete( native );
      persistent.dispose();
      return Toss("Native constructor threw an unknown exception!");
      }
    return self;
    }
#endif
  };

template <typename T> struct NativeToJSCopyableType
  {
  Value operator()(T const *n) const
    {
    T *out = 0;
    const InterfaceBase* interface = findInterface<T>(n);
    Object obj = interface->newInstance(0, 0);

    out = Convert::from<T>(obj);
    *out = *n;
    return obj;
    }
  Value operator()(T const &n) const
    {
    return this->operator()(&n);
    }
  };

template <typename T> struct NativeToJSConvertableType
  {
  Value operator()(T *n) const
    {
    if(!n)
      {
      return Value();
      }
    const InterfaceBase* interface = findInterface<T>(n);
    Value vals[1] = { Value(n) };
    Object self = interface->newInstance(1, vals);
    return self;
    }
  Value operator()(T &n) const
    {
    return this->operator()(&n);
    }
  };

template <typename T, typename BASE> struct NativeToJSConvertableTypeInherited
  {
  Value operator()(T *n) const
    {
    if(!n)
      {
      return Value();
      }
    BASE* base = n;
    const InterfaceBase* interface = findInterface<T>(n);
    Value vals[1] = { Value(base) };
    Object self = interface->newInstance(1, vals);
    return self;
    }
  Value operator()(T &n) const
    {
    return this->operator()(&n);
    }
  };

#define X_SCRIPTABLE_CONSTRUCTOR_DEF(variable, type, n) variable,

#define X_SCRIPTABLE_BUILD_CONSTRUCTABLE_TYPEDEF(name, type, ...)typedef XSignature< type (X_EXPAND_ARGS(X_SCRIPTABLE_CONSTRUCTOR_DEF, type, __VA_ARGS__) XScript::CtorForwarder<type *(const type &)>, XScript::CtorForwarder<type *()> )> name;

#define X_SCRIPTABLE_BUILD_CONSTRUCTABLE_COPYABLE(type, ...) \
  X_SCRIPTABLE_BUILD_CONSTRUCTABLE_TYPEDEF(type##Ctors, type, __VA_ARGS__)

#define X_SCRIPTABLE_BUILD_CONSTRUCTABLE(type, ...) \
  X_SCRIPTABLE_BUILD_CONSTRUCTABLE_TYPEDEF(type##Ctors, type, __VA_ARGS__)

#define X_SCRIPTABLE_MATCHERS(type) \
  template <> inline const type& match<const type&, type*>(type **in, bool& valid) { return ptrMatcher<type>(in, valid); }

#define X_SCRIPTABLE_TYPE_BASE(type)  \
  namespace XScript { namespace Convert { namespace internal { \
  template <> struct JSToNative<type> : JSToNativeObject<type> {}; } \
  X_SCRIPTABLE_MATCHERS(type) } } \
  Q_DECLARE_METATYPE(type *);

#define X_SCRIPTABLE_TYPE_COPYABLE(type, ...) X_SCRIPTABLE_TYPE_BASE(type) \
  namespace XScript { namespace Convert { namespace internal { \
  template <> struct NativeToJS<type> : public NativeToJSCopyableType<type> {}; } } } \
  Q_DECLARE_METATYPE(type);

#define X_SCRIPTABLE_TYPE(type, ...) X_SCRIPTABLE_TYPE_BASE(type) \
  namespace XScript { namespace Convert { namespace internal { \
  template <> struct NativeToJS<type> : public NativeToJSConvertableType<type> {}; } } }

#define X_SCRIPTABLE_ABSTRACT_TYPE(type, ...) X_SCRIPTABLE_TYPE_BASE(type) \
  namespace XScript { namespace Convert { namespace internal { \
  template <> struct NativeToJS<type> : public XScript::NativeToJSConvertableType<type> {}; } } }


#define X_SCRIPTABLE_TYPE_BASE_INHERITED(type, base)  \
  namespace XScript { namespace Convert { namespace internal { \
  template <> struct JSToNative<type> : JSToNativeObjectInherited<type, base> {}; } \
  X_SCRIPTABLE_MATCHERS(type) } } \
  Q_DECLARE_METATYPE(type *);

#define X_SCRIPTABLE_TYPE_INHERITS(type, base, ...) X_SCRIPTABLE_TYPE_BASE_INHERITED(type, base) \
  namespace XScript { namespace Convert { namespace internal { \
  template <> struct NativeToJS<type> : public XScript::NativeToJSConvertableTypeInherited<type, base> {}; } } }


#define X_SCRIPTABLE_ABSTRACT_TYPE_INHERITS(type, base, ...) X_SCRIPTABLE_TYPE_BASE_INHERITED(type, base) \
  namespace XScript { namespace Convert { namespace internal { \
  template <> struct NativeToJS<type> : public NativeToJSConvertableTypeInherited<type, base> {}; } } }


#define X_SCRIPTABLE_TYPE_NOT_COPYABLE(type) X_SCRIPTABLE_TYPE_BASE(type)

}

#endif // XINTERFACE_H
