#ifndef XTEMPLATEMETAHELPERS_H
#define XTEMPLATEMETAHELPERS_H

struct XNilType {};
typedef XNilType XNil;

/** A convenience base type for metafunctions holding a constant value. */
template <typename ValType, ValType Val>
struct XConstVal
{
    static const ValType Value = Val;
};
/** A metafunction holding an integer constant. */
template <int Val>
struct XIntVal : XConstVal<int,Val> {};
/** A metafunction holding an unsigned short constant. */
template <unsigned short Val>
struct XUShortVal : XConstVal<unsigned short,Val> {};
/** A metafunction holding a bool constant. */
template <bool Val>
struct XBoolVal : XConstVal<bool,Val> {};

/** A metatype whos Value member is true if X and Y are the same type. */
template <typename X,typename Y>
struct XSameType : XBoolVal<false> {};
/** Specialization for X==Y. */
template <typename X>
struct XSameType<X,X> : XBoolVal<true> {};

template <typename T>
struct XIdentity
{
    typedef T Type;
};

template <typename T>
struct XPlainType
{
    typedef T Type;
};

template <typename T>
struct XPlainType<T const> : XPlainType<T> {};
template <typename T>
struct XPlainType<T *> : XPlainType<T> {};
template <typename T>
struct XPlainType<T const *> : XPlainType<T> {};

template <typename T>
struct XIsConst : XBoolVal<false> {};
template <typename T>
struct XIsConst<T const> : XBoolVal<true> {};
template <typename T>
struct XIsConst<T const &> : XIsConst<T const> {};
template <typename T>
struct XIsConst<T const *> : XIsConst<T const> {};


template <typename T>
struct XIsNil : XSameType<T, XNilType> {};

#endif // XTEMPLATEMETAHELPERS_H
