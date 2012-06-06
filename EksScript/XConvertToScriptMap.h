#ifndef XCONVERTTOSCRIPTMAP_H
#define XCONVERTTOSCRIPTMAP_H

#include "XScriptTypeInfo.h"

template <typename T> struct XNativeToJSMap
  {
private:
  typedef XScriptTypeInfo<T> TI;
  typedef typename TI::Type Type;
  /**
           The native type to bind to.
        */
  typedef typename TI::NativeHandle NativeHandle;
  /** The type for holding the JS 'this' object. */
  typedef XPersistentScriptValue JSObjHandle;
  typedef std::pair<NativeHandle,JSObjHandle> ObjBindT;
  typedef std::map<void const *, ObjBindT> OneOfUsT;
  /** Maps (void const *) to ObjBindT.

            Reminder to self: we might need to make this map a static
            non-function member to work around linking problems (at
            least on Windows) which lead to multiple instances of
            the returned map being created when the types being
            bound are loaded from multiple DLLs. The out-of-class
            initialization of the member is going to require a really
            ugly set of template parameters, though.
        */
  static OneOfUsT & Map()
    {
    static OneOfUsT bob;
    return bob;
    }
public:
  /** Maps obj as a lookup key for jself. Returns false if !obj,
         else true. */
  static bool Insert( JSObjHandle const & jself,
                      NativeHandle obj )
    {
    return obj
        ? (Map().insert( std::make_pair( obj, std::make_pair( obj, jself ) ) ),true)
        : 0;
    }

  /**
           Removes any mapping of the given key. Returns the
           mapped native, or 0 if none is found.
        */
  static NativeHandle Remove( void const * key )
    {
    typedef typename OneOfUsT::iterator Iterator;
    OneOfUsT & map( Map() );
    Iterator it = map.find( key );
    if( map.end() == it )
      {
      return 0;
      }
    else
      {
      NativeHandle victim = (*it).second.first;
      map.erase(it);
      return victim;
      }
    }

  /**
           Returns the native associated (via Insert())
           with key, or 0 if none is found.
        */
  static NativeHandle GetNative( void const * key )
    {
    if( ! key ) return 0;
    else
      {
      typename OneOfUsT::iterator it = Map().find(key);
      return (Map().end() == it)
          ? 0
          : (*it).second.first;
      }
    }

  /**
           Returns the JS object associated with key, or
           an empty handle if !key or no object is found.
        */
  static XScriptValue GetJSObject( void const * key )
    {
    if( ! key ) return XScriptValue();
    typename OneOfUsT::const_iterator it = Map().find(key);
    if( Map().end() == it ) return XScriptValue();
    else return (*it).second.second;
    }

  /**
            A base NativeToJS<T> implementation for classes which use NativeToJSMap<T>
            to hold their native-to-JS bindings. To be used like this:

            @code
            // must be in the v8::convert namespace!
            template <>
            struct NativeToJS<MyType> : NativeToJSMap<MyType>::NativeToJSImpl {};
            @endcode
        */
  struct NativeToJSImpl
    {
    XScriptValue operator()( Type const * n ) const
      {
      typedef XNativeToJSMap<T> BM;
      XScriptValue const & rc( BM::GetJSObject(n) );
      if( !rc.isValid() ) return XScriptValue();
      else return rc;
      }
    XScriptValue operator()( Type const & n ) const
      {
      return this->operator()( &n );
      }
    };

#if 0
  //! Experimental
  template <typename ParentType>
  struct NativeToJSImpl_Subclass
    {
    v8::Handle<v8::Value> operator()( Type const * n ) const
      {
      typedef NativeToJSMap<T> BM;
      v8::Handle<v8::Value> const & rc( BM::GetJSObject(n) );
      if( rc.IsEmpty() )
        {
        typedef typename NativeToJSMap<ParentType>::NativeToJSImpl PI;
        return PI()(n);
#if 0
        typedef typename XScriptTypeInfo<ParentType>::NativeHandle PH;
        rc = CastToJS<ParentType>(n);
        if( rc.IsEmpty() ) return v8::Null();
        else return rc;
#endif
        }
      else return rc;
      }
    v8::Handle<v8::Value> operator()( Type const & n ) const
      {
      return this->operator()( &n );
      }
    };
#endif
  };

#endif // XCONVERTTOSCRIPTMAP_H
