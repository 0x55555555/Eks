#ifndef XSIGNATURE_H
#define XSIGNATURE_H

#include "XScriptFunction.h"

template <typename Sig> struct XSignature;

#define X_SCRIPT_TYPELIST(X) ::XSignature< void X >

/**
    Specialization to give "InvacationCallback-like" functions
    an Arity value of -1.

    Reminder: we can get rid of this if we factory out the Arity definition
    and use sl::Arity instead.
*/
template <typename RV>
struct XSignature<RV (XScriptArguments const &)>
{
    typedef RV ReturnType;
    typedef RV (*FunctionType)(XScriptArguments const &);
    typedef void Context;
    typedef XScriptArguments const & Head;
    typedef XSignature<RV ()> Tail;
};

template <typename RV>
struct XSignature<RV (*)(XScriptArguments const &)> : XSignature<RV (XScriptArguments const &)>
{};

template <typename T, typename RV>
struct XSignature<RV (T::*)(XScriptArguments const &)> : XSignature<RV (XScriptArguments const &)>
{
    typedef T Context;
    typedef RV (Context::*FunctionType)(XScriptArguments const &);
};


template <typename T, typename RV>
struct XSignature<RV (T::*)(XScriptArguments const &) const> : XSignature<RV (XScriptArguments const &)>
{
    typedef T const Context;
    typedef RV (Context::*FunctionType)(XScriptArguments const &) const;
};



/** @class XFunctionSignature
   Base (unimplemented) signature for XFunctionSignature
   specializations. The type passed to it must be a function
   signature.

   All implementations must define the interface described for
   XSignature and its Context typedef must be void for this type.

   Examples:

   @code
   // void func_foo():
   typedef XFunctionSignature< void () > NoArgsReturnsVoid;

   // int func_foo(double):
   typedef XFunctionSignature< int (double) > OneArgReturnsInt;

   // double func_foo(int,int):
   typedef XFunctionSignature< double (int,int) > TwoArgsReturnsDouble;
   @endcode

*/
template <typename FunctionSig>
struct XFunctionSignature : XSignature< FunctionSig > {};

/** @class XMethodSignature
   Base (unimplemented) signature for XMethodSignature
   specializations. The Sig type passed to it must match a member method
   signature of a function from the class T.
   e.g. (void (T::*)(int)) or its equivalent (void (int)).

   All implementations must have the interface called for by XSignature
   and the Context typedef must be non-cvp-qualified T.

   Examples:

   @code
   // void MyType::func():
   typedef XMethodSignature< MyType, void () > NoArgsReturnsVoid;

   // int MyType::func(double):
   typedef XMethodSignature< MyType, int (double) > OneArgReturnsInt;

   // double MyType::func(int,int):
   typedef XMethodSignature< MyType, double (int,int) > TwoArgsReturnsDouble;
   @endcode
*/
template <typename T, typename Sig>
struct XMethodSignature;

/** @class ConstMethodSignature
   Base (unimplemented) signature for ConstMethodSignature
   specializations. The Sig type passed to it must be a member
   method signature of a const function from the class T.
   e.g. (void (T::*)(int) const)

   All implementations must have the interface called for by XSignature
   and the Context typedef must be non-cvp-qualified T. The IsConst
   member (enum or static/const boolean) must be a true value.

   Examples:

   @code
   // void MyType::func() const:
   typedef ConstMethodSignature< MyType, void () > NoArgsReturnsVoid;

   // int MyType::func(double) const:
   typedef ConstMethodSignature< MyType, int (double) > OneArgReturnsInt;

   // double MyType::func(int,int) const:
   typedef ConstMethodSignature< MyType, double (int,int) > TwoArgsReturnsDouble;
   @endcode

    As of r2019 (20110723), XMethodSignature<T const,...> and
    ConstMethodSignature<T,...> are equivalent.
*/
template <typename T, typename Sig>
struct XConstMethodSignature;

template <typename T, typename RV >
struct XMethodSignature< T, RV () > : XSignature< RV () >
{
    typedef T Context;
    typedef RV (Context::*FunctionType)();
};


template <typename T, typename RV >
struct XMethodSignature< T, RV (T::*)() > : XMethodSignature<T, RV ()>
{
};

template <typename T, typename RV >
struct XMethodSignature< T const, RV () > : XConstMethodSignature< T, RV () >
{
};

template <typename T, typename RV >
struct XMethodSignature< T const, RV (T::*)() > : XMethodSignature<T const, RV ()>
{
};

template <typename T, typename RV >
struct XMethodSignature< T const, RV (T::*)() const > : XMethodSignature<T const, RV ()>
{
};

template <typename T, typename RV >
struct XConstMethodSignature< T, RV () > : XSignature< RV (T::*)() const >
{
    typedef T const Context;
    typedef RV (Context::*FunctionType)() const;
};

template <typename T, typename RV >
struct XConstMethodSignature< T, RV (T::*)() const > : XConstMethodSignature<T const, RV ()>
{
};

/**
   A "type-rich" function pointer.

   Sig must be a function signature type usable in the construct
   XFunctionSignature<Sig>. FuncPtr must be a function of that type.
*/
template <typename Sig, typename XFunctionSignature<Sig>::FunctionType FuncPtr>
struct XFunctionPtr : XFunctionSignature<Sig>
{
    /**
       This type's full "signature" type.
    */
    typedef XFunctionSignature<Sig> XSignatureType;
    /**
       The data type of FuncPtr.
     */
    typedef typename XSignatureType::FunctionType FunctionType;

    /** The function specifies in the template arguments. */
    static const FunctionType Function;
};
template <typename Sig, typename XFunctionSignature<Sig>::FunctionType FuncPtr>
typename XFunctionPtr<Sig,FuncPtr>::FunctionType const XFunctionPtr<Sig,FuncPtr>::Function = FuncPtr;

template <typename T, typename Sig, typename XMethodSignature<T,Sig>::FunctionType FuncPtr>
struct XMethodPtr : XMethodSignature<T,Sig>
{
    typedef XMethodSignature<T,Sig> XSignatureType;
    typedef typename XSignatureType::FunctionType FunctionType;
    static const FunctionType Function;
};

template <typename T, typename Sig, typename XMethodSignature<T,Sig>::FunctionType FuncPtr>
typename XMethodPtr<T,Sig,FuncPtr>::FunctionType const XMethodPtr<T,Sig,FuncPtr>::Function = FuncPtr;

template <typename T, typename Sig, typename XConstMethodSignature<T,Sig>::FunctionType FuncPtr>
struct XConstMethodPtr : XConstMethodSignature<T,Sig>
{
    typedef XConstMethodSignature<T,Sig> XSignatureType;
    typedef typename XSignatureType::FunctionType FunctionType;
    static const FunctionType Function;
};

template <typename T, typename Sig, typename XConstMethodSignature<T,Sig>::FunctionType FuncPtr>
typename XConstMethodPtr<T,Sig,FuncPtr>::FunctionType const XConstMethodPtr<T,Sig,FuncPtr>::Function = FuncPtr;


#endif // XSIGNATURE_H
