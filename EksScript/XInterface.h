#ifndef XINTERFACE_H
#define XINTERFACE_H

#include "XScriptGlobal.h"
#include "XMetaType"
#include "XProperty"
#include "XVariant"
#include "XMacroHelpers"
#include "XScriptFunction.h"
#include "XInterfaceUtilities.h"
#include "XScriptObject.h"
#include "XScriptException.h"
#include "XFunctions.h"
#include "XInterfaceUtilities.h"
#include "XUnorderedMap"

template <typename T, bool HasQMetaType = QMetaTypeId<T>::Defined> struct XQMetaTypeIdOrInvalid
  {
  static int id()
    {
    return 0;
    }
  };

template <typename T> struct XQMetaTypeIdOrInvalid<T, true>
  {
  static int id()
    {
    return qMetaTypeId<T>();
    }
  };

class EKSSCRIPT_EXPORT XInterfaceBase
  {
public:
  typedef XScriptValue (*ToScriptFn)(const void * const &);
  typedef void *(*FromScriptFn)(XScriptValue const &);

  typedef void* (*UpCastFn)(void *ptr);
  typedef XUnorderedMap<int, UpCastFn> UpCastMap;

XProperties:
  XROProperty(QString, typeName);
  XROProperty(int, typeId);
  XROProperty(int, baseTypeId);
  XROProperty(int, nonPointerTypeId);
  XROProperty(int, baseNonPointerTypeId);

  XROProperty(xsize, internalFieldCount);
  XROProperty(xsize, typeIdField);
  XROProperty(xsize, nativeField);

  XRORefProperty(QString, functionSource);

  XProperty(ToScriptFn, toScript, setToScript);
  XProperty(FromScriptFn, fromScript, setFromScript);

  XROProperty(const XInterfaceBase *, parent);

  XRORefProperty(UpCastMap, upcasts);

public:
  typedef XScriptValue (*NativeCtor)(XScriptArguments const &argv);
  XInterfaceBase(int typeID,
                 int nonPointerTypeID,
                 int baseTypeID,
                 int baseNonPointerTypeID,
                 const QString &typeName,
                 NativeCtor ctor,
                 xsize typeIdField,
                 xsize nativeField,
                 xsize internalFieldCount,
                 const XInterfaceBase *parent,
                 ToScriptFn convertTo=0,
                 FromScriptFn convertFrom=0);

  XInterfaceBase(xsize typeID,
                 xsize nonPointerTypeID,
                 const QString &typeName,
                 const XInterfaceBase *parent);
  ~XInterfaceBase();

  void seal();
  bool isSealed() const
    {
    return _isSealed;
    }

  QVariant toVariant(const XScriptValue &val, int typeHint);
  XScriptValue copyValue(const QVariant &) const;

  void wrapInstance(XScriptObject obj, void *object) const;
  void unwrapInstance(XScriptObject object) const;

  XScriptObject newInstance(int argc, XScriptValue argv[], const QString& name="") const;
  void set(const char *name, XScriptValue val);

  typedef XScriptValue (*Function)( XScriptArguments const & argv );
  typedef void (*FunctionDart)( XScriptDartArguments argv );
  typedef XScriptValue (*Getter)(XScriptValue property,const XAccessorInfo& info);
  typedef void (*Setter)(XScriptValue property, XScriptValue value, const XAccessorInfo& info);
  typedef XScriptValue (*NamedGetter)(XScriptValue, const XAccessorInfo& info);
  typedef XScriptValue (*IndexedGetter)(xuint32, const XAccessorInfo& info);

  void addConstructor(const char *name, xsize extraArgs, xsize argCount, Function, FunctionDart);
  void addProperty(const char *name, Getter, FunctionDart, Setter, FunctionDart);
  void addFunction(const char *name, xsize extraArgs, xsize argCount, Function, FunctionDart);
  void setIndexAccessor(IndexedGetter);
  void setNamedAccessor(NamedGetter);

  void addClassTo(const QString &thisClassName, XScriptObject const &dest) const;

  void addChildInterface(int typeId, UpCastFn fn);

  void *prototype();

protected:
  void inherit(const XInterfaceBase* parentType);

  mutable bool _isSealed;
  NativeCtor _nativeCtor;
  void *_constructor;
  void *_prototype;

#ifndef X_DART
  XScriptFunction constructorFunction() const;
#endif
  };

EKSSCRIPT_EXPORT XInterfaceBase *findInterface(int qMetaTypeId);
EKSSCRIPT_EXPORT void registerInterface(XInterfaceBase *interface);

template <typename T> const XInterfaceBase* findInterface(const T *)
  {
  return findInterface(qMetaTypeId<T*>());
  }

#include "XConvertToScript.h"
#include "XConvertToScriptMap.h"
#include "XProperties.h"

template <typename T> class XInterface : public XInterfaceBase
  {
public:
  typedef typename XPlainType<T>::Type Type;

  void wrapInstance(XScriptObject object, T *native) const
    {
    XInterfaceBase::wrapInstance(object, (void*)native);
    }

  void unwrapInstance(XScriptObject object) const
    {
    return XInterfaceBase::unwrapInstance(object);
    }

  template <typename ValueT> inline void set(char const *name, ValueT val)
    {
    set(name, XScriptConvert::to(val));
    }

  using XInterfaceBase::addConstructor;
  using XInterfaceBase::addProperty;
  using XInterfaceBase::addFunction;

  void addDefaultConstructor()
    {
    addConstructor<T *()>();
    }

  void addCopyConstructor()
    {
    addConstructor<T *(const T&)>();
    }

  void addNativeConvertConstructor()
    {
    addConstructor("_internal", 1, 1, 0, XScript::CtorNativeWrap<T, XInterface<T>::weak_dtor>::CallDart);
    }

  template <typename TYPE>
    void addConstructor(const char *name="")
  {
    typedef XScript::CtorFunctionWrapper<T, TYPE, XInterface<T>::weak_dtor> Wrapper;

    FunctionDart ctorDart = Wrapper::CallDart;

    addConstructor(name, 1, Wrapper::Arity, 0, ctorDart);
  }

  template <typename GETTYPE,
    typename SETTYPE,
    typename XConstMethodSignature<T, GETTYPE ()>::FunctionType GETTERMETHOD,
    typename XMethodSignature<T, void (SETTYPE)>::FunctionType SETTERMETHOD>
    void addProperty(const char *name)
  {
    typedef XScript::XConstMethodToGetter<T, GETTYPE (), GETTERMETHOD> Getter;
    typedef XScript::XMethodToSetter<T, SETTYPE, SETTERMETHOD> Setter;

    XInterfaceBase::addProperty(name, Getter::Get, Getter::GetDart, Setter::Set, Setter::SetDart);
  }

  template <typename GETTYPE,
            typename XConstMethodSignature<T, GETTYPE ()>::FunctionType GETTERMETHOD>
  void addProperty(const char *name)
    {
    Getter getter = XScript::XConstMethodToGetter<T, GETTYPE (), GETTERMETHOD>::Get;

    XInterfaceBase::addProperty(name, getter, 0);
    }

  template <typename GETTYPE,
            typename XMethodSignature<T, GETTYPE ()>::FunctionType GETTERMETHOD>
  void addAccessProperty(const char *name)
    {
    Getter getter = XScript::XMethodToGetter<T, GETTYPE (), GETTERMETHOD>::Get;

    XInterfaceBase::addProperty(name, getter, 0);
    }

  template <typename SIG,
            typename XMethodSignature<T, SIG>::FunctionType METHOD>
  void addMethod(const char *name)
    {
    typedef XScript::MethodToInCa<T, SIG, METHOD> FunctionType;

    // +1 for this
    XInterfaceBase::addFunction(name, 1, FunctionType::Arity, FunctionType::Call, FunctionType::CallDart);
    }

  template <typename SIG,
            typename XConstMethodSignature<T, SIG>::FunctionType METHOD>
  void addConstMethod(const char *name)
    {
    typedef XScript::ConstMethodToInCa<T, SIG, METHOD> FunctionType;

    // +1 for this
    XInterfaceBase::addFunction(name, 1, FunctionType::Arity, FunctionType::Call, FunctionType::CallDart);
    }

  template <typename SIG,
            typename XMethodSignature<T, SIG>::FunctionType METHOD>
  void addStaticMethod(const char *name)
    {
    Function fn = XScript::FunctionToInCa<SIG, METHOD>::Call;

    XInterfaceBase::addFunction(name, fn);
    }

  template <typename RETTYPE, typename XMethodSignature<T, RETTYPE (xsize i)>::FunctionType METHOD>
  void setIndexAccessor()
    {
    IndexedGetter fn = XScript::XMethodToIndexedGetter<T, RETTYPE (xsize i), METHOD>::Get;

    XInterfaceBase::setIndexAccessor(fn);
    }

  template <typename RETTYPE, typename XMethodSignature<T, RETTYPE (const QString &name)>::FunctionType METHOD>
  void setNamedAccessor()
    {
    NamedGetter fn = XScript::XMethodToNamedGetter<T, RETTYPE (const QString &name), METHOD>::Get;

    XInterfaceBase::setNamedAccessor(fn);
    }

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
  static bool destroyObject( XScriptObject const & jo )
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
  static bool destroyObject( XScriptValue const & jv )
    {
    return !jv.isObject() ? false : destroyObject(XScriptObject(jv));
    }

  /**
     A v8::InvocationCallback implementation which calls
     DestroyObject( argv.calleeThis() ).

     It is intended to be used as a "manual destructor" for
     classes which need it. The canonical examples are
     Stream.close() and Database.close().

     This function is not called DestroyObject to avoid name
     collisions during binding using Set(...,DestroyObjectCallback).
  */
  static XScriptValue destroyObjectCallback( XScriptArguments const & argv )
    {
    return destroyObject(argv.calleeThis()) ? true : false;
    }

  static XInterface *create(const QString &name)
    {
    xsize id = (xsize)qMetaTypeId<T*>();
    xsize nonPointerId = (xsize)XQMetaTypeIdOrInvalid<T>::id();

    XInterface &bob = instance(name, id, nonPointerId, id, nonPointerId, 0);
    registerInterface(&bob);

    xAssert(!bob.isSealed());
    return &bob;
    }

  template <typename PARENT, typename BASE>
  static XInterface *createWithParent(const QString &name, const XInterface<PARENT> *constParent, const XInterface<BASE> *constBase)
    {
    xsize id = constBase->typeId();
    xsize nonPointerId = (xsize)constBase->nonPointerTypeId();
    XInterface &bob = instance(name, qMetaTypeId<T*>(), XQMetaTypeIdOrInvalid<T>::id(), id, nonPointerId, constParent);

    typedef T* (*TCastFn)(BASE *ptr);
    TCastFn typedFn = XScriptConvert::castFromBase<T, BASE>;

    UpCastFn fn = (UpCastFn)typedFn;

    XInterface<BASE>* base = const_cast<XInterface<BASE>*>(constBase);
    base->addChildInterface(qMetaTypeId<T*>(), fn);

    registerInterface(&bob);

    xAssert(!bob.isSealed());
    return &bob;
    }

  static const XInterface *lookup()
    {
    static const XInterface *bob = static_cast<XInterface<T>*>(findInterface(qMetaTypeId<T*>()));
    xAssert(bob->isSealed());
    return bob;
    }

  XInterface(xsize typeId,
    xsize nonPointerTypeId,
    xsize baseTypeId,
    xsize baseNonPointerTypeId,
    const QString &name,
    const XInterfaceBase* parent)
    : XInterfaceBase(typeId, nonPointerTypeId, baseTypeId, baseNonPointerTypeId,
                                name,
                                ctor_proxy,
                                InternalFields::TypeIDIndex,
                                InternalFields::NativeIndex,
                                InternalFields::Count,
                                parent)
    {
      {
      typedef XScriptValue (*TVal)(T * const &);
      TVal val = XScriptConvert::to<T*>;

      ToScriptFn unTyped = (ToScriptFn)val;
      setToScript(unTyped);
      }

      {
      typedef T *(*FVal)(XScriptValue const &);
      FVal from = XScriptConvert::from<T*>;

      FromScriptFn unTyped = (FromScriptFn)from;
      setFromScript(unTyped);
      }
    }

private:
  typedef XScript::ClassCreator_InternalFields<T> InternalFields;
  typedef XScript::ClassCreator_Factory<T> Factory;

  static XInterface &instance(const QString &name, xsize id, xsize nonPointerId, xsize baseId, xsize baseNonPointerId, const XInterfaceBase* parent)
    {
    static XInterface bob(id, nonPointerId, baseId, baseNonPointerId,name, parent);
    return bob;
    }

  template <typename BASE>
      static XScriptObject FindHolder( XScriptObject const & jo, BASE const * nh )
    {
    if( !nh || !jo.isValid() ) return XScriptObject();
    XScriptValue proto(jo);
    void const * ext = NULL;
    typedef XScript::ClassCreator_SearchPrototypeForThis<T> SPFT;
    while( !ext && proto.isValid() && proto.isObject() )
      {
      XScriptObject obj(proto);
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
    return XScriptObject();
    }

  static void weak_dtor( XPersistentScriptValue pv, void * )
    {
    XScriptValue val = pv.asValue();
    XScriptObject jobj(val);
    typedef typename Factory::ReturnType BT;
    typedef typename XScriptConvert::internal::JSToNative<T>::ResultType NT;
    NT native = XScriptConvert::from<T>( val );
    BT base = native;
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
      XScriptObject nholder = FindHolder( jobj, base );
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
      findInterface<T>(native)->unwrapInstance(nholder);
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

  /**
     Gets installed as the NewInstance() handler for T.
   */
  static XScriptValue ctor_proxy( XScriptArguments const & argv )
    {
    if(XScript::ClassCreator_AllowCtorWithoutNew<T>::Value)
      {
      /**
             Allow construction without 'new' by forcing this
             function to be called in a ctor context...
          */
      if (!argv.isConstructCall())
        {
        return argv.callee().callAsConstructor(argv);
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
    XScriptObject jobj(argv.calleeThis());
    if( !jobj.isValid() )
      {
      return jobj /* assume exception*/;
      }

    XPersistentScriptValue persistent(jobj);
    XScriptObject self(persistent.asValue());
    typename Factory::ReturnType nobj = NULL;
    try
      {
      nobj = Factory::Create( self, argv );
      if( ! nobj )
        {
        return XScriptConvert::to<std::exception>(std::runtime_error("Native constructor failed."));
        }

      typedef typename XScriptTypeInfo<T>::NativeHandle NativeHandle;
      NativeHandle native = static_cast<NativeHandle>(nobj);
      persistent.makeWeak( nobj, weak_dtor );
      findInterface<T>(native)->wrapInstance(self, nobj);
      }
    catch(std::exception const &ex)
      {
      typedef typename XScriptTypeInfo<T>::NativeHandle NativeHandle;
      NativeHandle native = static_cast<NativeHandle>(nobj);
      if( nobj ) Factory::Delete( native );
      persistent.dispose();
      return Toss(ex.what());
      }
    catch(...)
      {
      typedef typename XScriptTypeInfo<T>::NativeHandle NativeHandle;
      NativeHandle native = static_cast<NativeHandle>(nobj);
      if( nobj ) Factory::Delete( native );
      persistent.dispose();
      return Toss("Native constructor threw an unknown exception!");
      }
    return self;
    }
  };

namespace XScript
{
template <typename T> struct NativeToJSCopyableType
  {
  XScriptValue operator()(T const *n) const
    {
    T *out = 0;
    const XInterfaceBase* interface = findInterface<T>(n);
    XScriptObject obj = interface->newInstance(0, 0);

    out = XScriptConvert::from<T>(obj);
    *out = *n;
    return obj;
    }
  XScriptValue operator()(T const &n) const
    {
    return this->operator()(&n);
    }
  };

template <typename T> struct NativeToJSConvertableType
  {
  XScriptValue operator()(T *n) const
    {
    if(!n)
      {
      return XScriptValue();
      }
    const XInterfaceBase* interface = findInterface<T>(n);
    XScriptValue vals[1] = { XScriptValue(n) };
    XScriptObject self = interface->newInstance(1, vals);
    return self;
    }
  XScriptValue operator()(T &n) const
    {
    return this->operator()(&n);
    }
  };

template <typename T, typename BASE> struct NativeToJSConvertableTypeInherited
  {
  XScriptValue operator()(T *n) const
    {
    if(!n)
      {
      return XScriptValue();
      }
    BASE* base = n;
    const XInterfaceBase* interface = findInterface<T>(n);
    XScriptValue vals[1] = { XScriptValue(base) };
    XScriptObject self = interface->newInstance(1, vals);
    return self;
    }
  XScriptValue operator()(T &n) const
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
  namespace XScriptConvert { namespace internal { \
  template <> struct JSToNative<type> : XScriptConvert::internal::JSToNativeObject<type> {}; } \
  X_SCRIPTABLE_MATCHERS(type) } \
  Q_DECLARE_METATYPE(type *);

#define X_SCRIPTABLE_TYPE_COPYABLE(type, ...) X_SCRIPTABLE_TYPE_BASE(type) \
  namespace XScriptConvert { namespace internal { \
  template <> struct NativeToJS<type> : public XScript::NativeToJSCopyableType<type> {}; } } \
  Q_DECLARE_METATYPE(type);

#define X_SCRIPTABLE_TYPE(type, ...) X_SCRIPTABLE_TYPE_BASE(type) \
  namespace XScriptConvert { namespace internal { \
  template <> struct NativeToJS<type> : public XScript::NativeToJSConvertableType<type> {}; } }

#define X_SCRIPTABLE_ABSTRACT_TYPE(type, ...) X_SCRIPTABLE_TYPE_BASE(type) \
  namespace XScriptConvert { namespace internal { \
  template <> struct NativeToJS<type> : public XScript::NativeToJSConvertableType<type> {}; } }


#define X_SCRIPTABLE_TYPE_BASE_INHERITED(type, base)  \
  namespace XScriptConvert { namespace internal { \
  template <> struct JSToNative<type> : XScriptConvert::internal::JSToNativeObjectInherited<type, base> {}; } \
  X_SCRIPTABLE_MATCHERS(type) } \
  Q_DECLARE_METATYPE(type *);

#define X_SCRIPTABLE_TYPE_INHERITS(type, base, ...) X_SCRIPTABLE_TYPE_BASE_INHERITED(type, base) \
  namespace XScriptConvert { namespace internal { \
  template <> struct NativeToJS<type> : public XScript::NativeToJSConvertableTypeInherited<type, base> {}; } }


#define X_SCRIPTABLE_ABSTRACT_TYPE_INHERITS(type, base, ...) X_SCRIPTABLE_TYPE_BASE_INHERITED(type, base) \
  namespace XScriptConvert { namespace internal { \
  template <> struct NativeToJS<type> : public XScript::NativeToJSConvertableTypeInherited<type, base> {}; } } \
  namespace XScript { \
  template <> class ClassCreator_Factory<type> : public ClassCreatorConvertableFactory<type, base> {}; }


#define X_SCRIPTABLE_TYPE_NOT_COPYABLE(type) X_SCRIPTABLE_TYPE_BASE(type)

}

#endif // XINTERFACE_H
