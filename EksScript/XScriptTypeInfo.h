#ifndef XSCRIPTTYPEINFO_H
#define XSCRIPTTYPEINFO_H

template <typename T, typename NHT = T *> struct XScriptTypeInfoCore
  {
  typedef T Type;
  typedef NHT NativeHandle;
  };

template <typename T> struct XScriptTypeInfoBase : XScriptTypeInfoCore<T, T*> {};

template <typename T> struct XScriptTypeInfoBase<T const> : XScriptTypeInfoBase<T> {};
template <typename T> struct XScriptTypeInfoBase<T *> : XScriptTypeInfoBase<T> {};
template <typename T> struct XScriptTypeInfoBase<T const *> : XScriptTypeInfoBase<T> {};
template <typename T> struct XScriptTypeInfoBase<T const &> : XScriptTypeInfoBase<T> {};
template <typename T> struct XScriptTypeInfoBase<T &> : XScriptTypeInfoBase<T> {};

//template <typename T> struct XScriptTypeInfoBase< v8::Handle<T> >
//{
//    typedef v8::Handle<T> Type;
//    typedef v8::Handle<T> NativeHandle;
//};

template <typename T> class XScriptTypeInfo : public XScriptTypeInfoBase<T> {};

#endif // XSCRIPTTYPEINFO_H
