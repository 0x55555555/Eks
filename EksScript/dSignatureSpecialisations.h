#ifndef XSIGNATURESPECIALISATIONS_H
#define XSIGNATURESPECIALISATIONS_H

template <typename RV>
struct XSignature< RV () >
{
    typedef RV ReturnType;
    enum { IsConst = 0 };
    typedef void Context;
    typedef RV (*FunctionType)();
    typedef XNilType Head;
    typedef Head Tail;
};
template <typename RV>
struct XSignature< RV (*)() > : XSignature<RV ()>
{};

template <typename T, typename RV>
struct XSignature< RV (T::*)() > : XSignature<RV ()>
{
    typedef T Context;
    typedef RV (T::*FunctionType)();
};
template <typename T, typename RV>
struct XSignature< RV (T::*)() const > : XSignature<RV ()>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)() const;
    enum { IsConst = 1 };
};
//! Specialization for 1 arg(s).
template <typename RV, typename A1>
struct XSignature< RV (A1) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1);
    typedef A1 Head;
    typedef XSignature< RV () > Tail;
};

//! Specialization for 1 arg(s).
template <typename RV, typename A1>
struct XSignature< RV (*)(A1) > : XSignature<RV (A1)>
{};

//! Specialization for T non-const methods taking 1 arg(s).
template <typename T, typename RV, typename A1>
struct XSignature< RV (T::*)(A1) > : XSignature<RV (A1)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1);
};

//! Specialization for T const methods taking 1 arg(s).
template <typename T, typename RV, typename A1>
struct XSignature< RV (T::*)(A1) const > : XSignature<RV (A1)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1) const;
};

//! Specialization for 2 arg(s).
template <typename RV, typename A1, typename A2>
struct XSignature< RV (A1, A2) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2);
    typedef A1 Head;
    typedef XSignature<RV ( A2)> Tail;
};

//! Specialization for 2 arg(s).
template <typename RV, typename A1, typename A2>
struct XSignature< RV (*)(A1, A2) > : XSignature<RV (A1, A2)>
{};

//! Specialization for T non-const methods taking 2 arg(s).
template <typename T, typename RV, typename A1, typename A2>
struct XSignature< RV (T::*)(A1, A2) > : XSignature<RV (A1, A2)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2);
};

//! Specialization for T const methods taking 2 arg(s).
template <typename T, typename RV, typename A1, typename A2>
struct XSignature< RV (T::*)(A1, A2) const > : XSignature<RV (A1, A2)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2) const;
};

//! Specialization for 3 arg(s).
template <typename RV, typename A1, typename A2, typename A3>
struct XSignature< RV (A1, A2, A3) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3)> Tail;
};

//! Specialization for 3 arg(s).
template <typename RV, typename A1, typename A2, typename A3>
struct XSignature< RV (*)(A1, A2, A3) > : XSignature<RV (A1, A2, A3)>
{};

//! Specialization for T non-const methods taking 3 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3>
struct XSignature< RV (T::*)(A1, A2, A3) > : XSignature<RV (A1, A2, A3)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3);
};

//! Specialization for T const methods taking 3 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3>
struct XSignature< RV (T::*)(A1, A2, A3) const > : XSignature<RV (A1, A2, A3)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3) const;
};

//! Specialization for 4 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4>
struct XSignature< RV (A1, A2, A3, A4) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4)> Tail;
};

//! Specialization for 4 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4>
struct XSignature< RV (*)(A1, A2, A3, A4) > : XSignature<RV (A1, A2, A3, A4)>
{};

//! Specialization for T non-const methods taking 4 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4>
struct XSignature< RV (T::*)(A1, A2, A3, A4) > : XSignature<RV (A1, A2, A3, A4)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4);
};

//! Specialization for T const methods taking 4 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4>
struct XSignature< RV (T::*)(A1, A2, A3, A4) const > : XSignature<RV (A1, A2, A3, A4)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4) const;
};

//! Specialization for 5 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5>
struct XSignature< RV (A1, A2, A3, A4, A5) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5)> Tail;
};

//! Specialization for 5 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5>
struct XSignature< RV (*)(A1, A2, A3, A4, A5) > : XSignature<RV (A1, A2, A3, A4, A5)>
{};

//! Specialization for T non-const methods taking 5 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5) > : XSignature<RV (A1, A2, A3, A4, A5)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5);
};

//! Specialization for T const methods taking 5 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5) const > : XSignature<RV (A1, A2, A3, A4, A5)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5) const;
};

//! Specialization for 6 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct XSignature< RV (A1, A2, A3, A4, A5, A6) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5, A6)> Tail;
};

//! Specialization for 6 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct XSignature< RV (*)(A1, A2, A3, A4, A5, A6) > : XSignature<RV (A1, A2, A3, A4, A5, A6)>
{};

//! Specialization for T non-const methods taking 6 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6) > : XSignature<RV (A1, A2, A3, A4, A5, A6)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6);
};

//! Specialization for T const methods taking 6 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6) const > : XSignature<RV (A1, A2, A3, A4, A5, A6)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6) const;
};

//! Specialization for 7 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct XSignature< RV (A1, A2, A3, A4, A5, A6, A7) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5, A6, A7)> Tail;
};

//! Specialization for 7 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct XSignature< RV (*)(A1, A2, A3, A4, A5, A6, A7) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7)>
{};

//! Specialization for T non-const methods taking 7 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7);
};

//! Specialization for T const methods taking 7 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7) const > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7) const;
};

//! Specialization for 8 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct XSignature< RV (A1, A2, A3, A4, A5, A6, A7, A8) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5, A6, A7, A8)> Tail;
};

//! Specialization for 8 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct XSignature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8)>
{};

//! Specialization for T non-const methods taking 8 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8);
};

//! Specialization for T const methods taking 8 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8) const > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8) const;
};

//! Specialization for 9 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct XSignature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5, A6, A7, A8, A9)> Tail;
};

//! Specialization for 9 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct XSignature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{};

//! Specialization for T non-const methods taking 9 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
};

//! Specialization for T const methods taking 9 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const;
};

//! Specialization for 10 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
struct XSignature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10)> Tail;
};

//! Specialization for 10 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
struct XSignature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{};

//! Specialization for T non-const methods taking 10 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10);
};

//! Specialization for T const methods taking 10 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const;
};

//! Specialization for 11 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
struct XSignature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)> Tail;
};

//! Specialization for 11 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
struct XSignature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{};

//! Specialization for T non-const methods taking 11 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11);
};

//! Specialization for T const methods taking 11 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) const > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) const;
};

//! Specialization for 12 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
struct XSignature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)> Tail;
};

//! Specialization for 12 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
struct XSignature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{};

//! Specialization for T non-const methods taking 12 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12);
};

//! Specialization for T const methods taking 12 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) const > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) const;
};

//! Specialization for 13 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
struct XSignature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)> Tail;
};

//! Specialization for 13 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
struct XSignature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{};

//! Specialization for T non-const methods taking 13 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13);
};

//! Specialization for T const methods taking 13 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) const > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) const;
};

//! Specialization for 14 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
struct XSignature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)> Tail;
};

//! Specialization for 14 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
struct XSignature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{};

//! Specialization for T non-const methods taking 14 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14);
};

//! Specialization for T const methods taking 14 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) const > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) const;
};

//! Specialization for 15 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15>
struct XSignature< RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) >
{
    typedef RV ReturnType;
    typedef void Context;
    typedef RV (*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15);
    typedef A1 Head;
    typedef XSignature<RV ( A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15)> Tail;
};

//! Specialization for 15 arg(s).
template <typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15>
struct XSignature< RV (*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15)>
{};

//! Specialization for T non-const methods taking 15 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15)>
{
    typedef T Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15);
};

//! Specialization for T const methods taking 15 arg(s).
template <typename T, typename RV, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10, typename A11, typename A12, typename A13, typename A14, typename A15>
struct XSignature< RV (T::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) const > : XSignature<RV (A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15)>
{
    typedef T const Context;
    typedef RV (T::*FunctionType)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15) const;
};

template <typename T, typename RV,  typename A0 >
struct XMethodSignature< T, RV ( A0) > : XSignature< RV (T::*)( A0) >
{
};
template <typename T, typename RV,  typename A0 >
struct XMethodSignature< T, RV (*)( A0) > : XMethodSignature< T, RV ( A0) >
{
};

template <typename T, typename RV,  typename A0 >
struct XMethodSignature< T, RV (T::*)( A0) > :
    XMethodSignature< T, RV ( A0) >
{};

template <typename T, typename RV,  typename A0 >
struct XMethodSignature< T const, RV ( A0) > :
    XConstMethodSignature< T, RV ( A0) >
{};

template <typename T, typename RV,  typename A0 >
struct XMethodSignature< T const, RV (T::*)( A0) > :
    XMethodSignature< T const, RV ( A0) >
{};

#if 1 // msvc? Apparently this works.
template <typename T, typename RV,  typename A0 >
struct XMethodSignature< T const, RV (T::*)( A0) const > :
    XMethodSignature< T const, RV ( A0) >
{};
#endif


template <typename T, typename RV,  typename A0 >
struct XConstMethodSignature< T, RV ( A0) > : XSignature< RV (T::*)( A0) const >
{
};
template <typename T, typename RV,  typename A0 >
struct XConstMethodSignature< T, RV (T::*)( A0) const > :
    XConstMethodSignature< T, RV ( A0) >
{};
template <typename T, typename RV,  typename A0, typename A1 >
struct XMethodSignature< T, RV ( A0, A1) > : XSignature< RV (T::*)( A0, A1) >
{
};
template <typename T, typename RV,  typename A0, typename A1 >
struct XMethodSignature< T, RV (*)( A0, A1) > : XMethodSignature< T, RV ( A0, A1) >
{
};

template <typename T, typename RV,  typename A0, typename A1 >
struct XMethodSignature< T, RV (T::*)( A0, A1) > :
    XMethodSignature< T, RV ( A0, A1) >
{};

template <typename T, typename RV,  typename A0, typename A1 >
struct XMethodSignature< T const, RV ( A0, A1) > :
    XConstMethodSignature< T, RV ( A0, A1) >
{};

template <typename T, typename RV,  typename A0, typename A1 >
struct XMethodSignature< T const, RV (T::*)( A0, A1) > :
    XMethodSignature< T const, RV ( A0, A1) >
{};

#if 1 // msvc? Apparently this works.
template <typename T, typename RV,  typename A0, typename A1 >
struct XMethodSignature< T const, RV (T::*)( A0, A1) const > :
    XMethodSignature< T const, RV ( A0, A1) >
{};
#endif


template <typename T, typename RV,  typename A0, typename A1 >
struct XConstMethodSignature< T, RV ( A0, A1) > : XSignature< RV (T::*)( A0, A1) const >
{
};
template <typename T, typename RV,  typename A0, typename A1 >
struct XConstMethodSignature< T, RV (T::*)( A0, A1) const > :
    XConstMethodSignature< T, RV ( A0, A1) >
{};
template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct XMethodSignature< T, RV ( A0, A1, A2) > : XSignature< RV (T::*)( A0, A1, A2) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct XMethodSignature< T, RV (*)( A0, A1, A2) > : XMethodSignature< T, RV ( A0, A1, A2) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct XMethodSignature< T, RV (T::*)( A0, A1, A2) > :
    XMethodSignature< T, RV ( A0, A1, A2) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct XMethodSignature< T const, RV ( A0, A1, A2) > :
    XConstMethodSignature< T, RV ( A0, A1, A2) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2) > :
    XMethodSignature< T const, RV ( A0, A1, A2) >
{};

#if 1 // msvc? Apparently this works.
template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2) const > :
    XMethodSignature< T const, RV ( A0, A1, A2) >
{};
#endif


template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct XConstMethodSignature< T, RV ( A0, A1, A2) > : XSignature< RV (T::*)( A0, A1, A2) const >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2 >
struct XConstMethodSignature< T, RV (T::*)( A0, A1, A2) const > :
    XConstMethodSignature< T, RV ( A0, A1, A2) >
{};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct XMethodSignature< T, RV ( A0, A1, A2, A3) > : XSignature< RV (T::*)( A0, A1, A2, A3) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct XMethodSignature< T, RV (*)( A0, A1, A2, A3) > : XMethodSignature< T, RV ( A0, A1, A2, A3) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct XMethodSignature< T, RV (T::*)( A0, A1, A2, A3) > :
    XMethodSignature< T, RV ( A0, A1, A2, A3) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct XMethodSignature< T const, RV ( A0, A1, A2, A3) > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3) > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3) >
{};

#if 1 // msvc? Apparently this works.
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3) const > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3) >
{};
#endif


template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct XConstMethodSignature< T, RV ( A0, A1, A2, A3) > : XSignature< RV (T::*)( A0, A1, A2, A3) const >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3 >
struct XConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3) const > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3) >
{};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct XMethodSignature< T, RV ( A0, A1, A2, A3, A4) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct XMethodSignature< T, RV (*)( A0, A1, A2, A3, A4) > : XMethodSignature< T, RV ( A0, A1, A2, A3, A4) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct XMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4) > :
    XMethodSignature< T, RV ( A0, A1, A2, A3, A4) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct XMethodSignature< T const, RV ( A0, A1, A2, A3, A4) > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4) > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4) >
{};

#if 1 // msvc? Apparently this works.
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4) const > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4) >
{};
#endif


template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4) const >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4 >
struct XConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4) const > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4) >
{};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4, A5) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct XMethodSignature< T, RV (*)( A0, A1, A2, A3, A4, A5) > : XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct XMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5) > :
    XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5) > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4, A5) > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5) >
{};

#if 1 // msvc? Apparently this works.
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4, A5) const > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5) >
{};
#endif


template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4, A5) const >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct XConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5) const > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5) >
{};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct XMethodSignature< T, RV (*)( A0, A1, A2, A3, A4, A5, A6) > : XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct XMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6) > :
    XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6) > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4, A5, A6) > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6) >
{};

#if 1 // msvc? Apparently this works.
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4, A5, A6) const > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6) >
{};
#endif


template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6) const >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct XConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6) const > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6) >
{};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct XMethodSignature< T, RV (*)( A0, A1, A2, A3, A4, A5, A6, A7) > : XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct XMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) > :
    XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6, A7) > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6, A7) >
{};

#if 1 // msvc? Apparently this works.
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) const > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6, A7) >
{};
#endif


template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) const >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct XConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7) const > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7) >
{};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct XMethodSignature< T, RV (*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) > : XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct XMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) > :
    XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{};

#if 1 // msvc? Apparently this works.
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) const > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{};
#endif


template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) const >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct XConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8) const > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8) >
{};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct XMethodSignature< T, RV (*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) > : XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{
};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct XMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) > :
    XMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{};

template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{};

#if 1 // msvc? Apparently this works.
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct XMethodSignature< T const, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const > :
    XMethodSignature< T const, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{};
#endif


template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) > : XSignature< RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const >
{
};
template <typename T, typename RV,  typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct XConstMethodSignature< T, RV (T::*)( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) const > :
    XConstMethodSignature< T, RV ( A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{};

#endif // XSIGNATURESPECIALISATIONS_H
