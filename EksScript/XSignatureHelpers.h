#ifndef XSIGNATUREHELPERS_H
#define XSIGNATUREHELPERS_H

#include "XSignature.h"
#include "XTemplateMetaHelpers.h"

namespace sl
{
  /**
      Metafunction whose Value member evaluates to the length of the
      given XSignature.
  */
  template < typename ListT >
  struct Length : XIntVal<
          XIsNil<typename ListT::Head>::Value ? 0 : (1 + Length<typename ListT::Tail>::Value)
          > {};

  //! End-of-list specialization.
  template <>
  struct Length<XNil> : XIntVal<0> {};

  /**
      Metafunction whose Type member evaluates to the type of the
      I'th argument type in the given XSignature. Fails to compile
      if I is out of range.
  */
  template < unsigned short I, typename ListT >
  struct At : At<I-1, typename ListT::Tail>
  {
      typedef char AssertIndex[ (I >= Length<ListT>::Value) ? -1 : 1 ];
  };

  //! Beginning-of-list specialization.
  template < typename ListT >
  struct At<0, ListT>
  {
       typedef typename ListT::Head Type;
  };

#if 0
  /**
      End-of-list specialization. i don't think we need this, actually.
  */
  template <unsigned short I>
  struct At<I, tmp::nil> : tmp::Identity<tmp::nil>
  {};
#endif

  /**
      Metafunction whose Type Value member evaluates to the 0-based
      index of the first occurrance of the the type T in the
      given XSignature's argument list. Evaluates to -1 if T is not
      contained in the argument list. XSignature::ReturnType and
      XSignature::Context are not evaluated.

      Clients _must not_ pass a value for the 3rd template parameter.
  */
  template < typename T, typename ListT, unsigned short Internal = 0 >
  struct Index : XIntVal< XSameType<T, typename ListT::Head>::Value
                          ? Internal
                          : Index<T, typename ListT::Tail, Internal+1>::Value>
  {
  };

  //! End-of-list specialization.
  template < typename T, unsigned short Internal >
  struct Index<T,XNil,Internal> : XIntVal<-1> {};

  /**
      Convenience form of Index<T,ListT> which evaluates to true
      if Index returns a non-negative value, else it evaluates
      to false.
  */
#if 0
  template < typename T, typename ListT>
  struct Contains : XBoolVal< Index<T,ListT>::Value >= 0  > {};
#endif

  /**
      A metatype which calculates the number of arguments in the given
      typelist, but evaluates to -1 if SigT's only argument is
      (XScriptArguments const &), as such function signatures are considered
      to be n-arity.
  */
  template <typename SigT>
  struct Arity
  {
    enum {
      Value = ((1==Length<SigT>::Value)
              && (0==Index<XScriptArguments const &,SigT>::Value))
              ? -1
              : Length<SigT>::Value
    };
  };

  /**
      This metafunction evaluates to true if SigT appears to be
      "InvocationCallback-like" (returns any type and takes one
      (XScriptArguments const &) parameter).

      We could implement this a number of different ways. The
      current impl simply checks if the arity is -1.
  */
  template <typename SigT>
  struct IsInCaLike : XBoolVal< -1 == Arity<SigT>::Value > {};

  /**
      A metafunction which has a true Value if the XSignature type SigT
      represents a non-member function.
  */
  template <typename SigT>
  struct IsFunction : XBoolVal< XSameType<void, typename SigT::Context>::Value > {};

  /**
      A metafunction which has a true Value if the XSignature type SigT
      represents a member function (const or not).
  */
  template <typename SigT>
  struct IsMethod : XBoolVal< !XSameType<void, typename SigT::Context>::Value > {};

  /**
      A metafunction which has a true Value if the XSignature type SigT
      represents a non-const member function.
  */
  template <typename SigT>
  struct IsNonConstMethod : XBoolVal< !XIsConst< typename SigT::Context >::Value && IsMethod<SigT>::Value > {};

  /**
      A metafunction which has a true Value if the XSignature type SigT
      represents a const member function.
  */
  template <typename SigT>
  struct IsConstMethod : XBoolVal< XIsConst< typename SigT::Context >::Value && IsMethod<SigT>::Value > {};

}

/**
   A metatemplate who's Type member resolves to IF if Cond is
   true, or ELSE if Cond is false. Its Value member evaluates
   to 1 or 0, accordingly.
*/
template <bool Cond, typename IF, typename ELSE> struct XIfElse : sl::At< Cond ? 0 : 1, XSignature<void (IF,ELSE)> > {};


#endif // XSIGNATUREHELPERS_H
