#ifndef XSCRIPTTYPEINFO_H
#define XSCRIPTTYPEINFO_H

namespace XScript
{

template <typename T, typename NHT = T *> struct TypeInfoCore
  {
  typedef T Type;
  typedef NHT NativeHandle;
  };

template <typename T> struct TypeInfoBase : TypeInfoCore<T, T*> {};

template <typename T> struct TypeInfoBase<T const> : TypeInfoBase<T> {};
template <typename T> struct TypeInfoBase<T *> : TypeInfoBase<T> {};
template <typename T> struct TypeInfoBase<T const *> : TypeInfoBase<T> {};
template <typename T> struct TypeInfoBase<T const &> : TypeInfoBase<T> {};
template <typename T> struct TypeInfoBase<T &> : TypeInfoBase<T> {};

template <typename T> class TypeInfo : public TypeInfoBase<T> {};

}

#endif // XSCRIPTTYPEINFO_H
