#include "XReflexTest.h"
#include "XReflex.h"
#include <QtTest>

class A
  {
public:
  void pork1(const float&, double* pork);
  int pork2(A *) const;
  static A* pork3(const float&);
  };

namespace Eks
{
namespace Reflex
{
namespace detail
{
template <> struct TypeResolver<A> 
  {
  static const Type *find()
    {
    static Type t;
    return &t;
    }
  };

}
}
}

class InvocationBuilder
  {
public:
  class Arguments
    {
    void **_args;
    void *_this;
    };
  typedef Arguments *CallData;

  typedef void (*Signature)(CallData);

  template <typename T> static T getThis(CallData args)
    {
    return (T)args->_this;
    }

  template <xsize I, typename Tuple>
      static typename std::tuple_element<I, Tuple>::type unpackArgument(CallData args)
    {
    typedef std::tuple_element<I, Tuple>::type Arg;
    return (Arg*)args->_args[I];
    }
  };

void EksReflexTest::functionWrapTest()
  {
  using namespace Eks::Reflex;
  typedef A ReflexClass;

  auto method1 = REFLEX_METHOD(pork1);
  auto method2 = REFLEX_METHOD(pork2);
  auto method3 = REFLEX_METHOD(pork3);

  QCOMPARE(method1.returnType(), findType<void>());
  QCOMPARE(method2.returnType(), findType<int>());
  QCOMPARE(method3.returnType(), findType<A*>());

  QCOMPARE(method1.argumentCount(), 2U);
  QCOMPARE(method2.argumentCount(), 1U);
  QCOMPARE(method3.argumentCount(), 1U);

  QCOMPARE(method1.argumentType<0>(), findType<const float&>());
  QCOMPARE(method1.argumentType<1>(), findType<double*>());
  QCOMPARE(method2.argumentType<0>(), findType<A *>());
  QCOMPARE(method3.argumentType<0>(), findType<const float &>());

  QCOMPARE(decltype(method1)::Helper::Const::value, false);
  QCOMPARE(decltype(method2)::Helper::Const::value, true);
  QCOMPARE(decltype(method3)::Helper::Const::value, false);

  QCOMPARE(decltype(method1)::Helper::Static::value, false);
  QCOMPARE(decltype(method2)::Helper::Static::value, false);
  QCOMPARE(decltype(method3)::Helper::Static::value, true);

  QCOMPARE(findType<decltype(method1)::Helper::Class>(), findType<A>());
  QCOMPARE(findType<decltype(method2)::Helper::Class>(), findType<A>());
  QCOMPARE(findType<decltype(method3)::Helper::Class>(), findType<void>());
  }

void EksReflexTest::functionInvokeTest()
  {
  using namespace Eks::Reflex;
  typedef A ReflexClass;

  auto method1 = REFLEX_METHOD(pork1);
  auto method2 = REFLEX_METHOD(pork2);
  auto method3 = REFLEX_METHOD(pork3);

  auto inv1 = method1.buildInvocation<InvocationBuilder>();
  auto inv2 = method2.buildInvocation<InvocationBuilder>();
  auto inv3 = method3.buildInvocation<InvocationBuilder>();

  (void)inv1;
  (void)inv2;
  (void)inv3;
  }

void EksReflexTest::classWrapTest()
  {
  auto cls = REFLEX_CLASS(A);

  auto method1 = REFLEX_METHOD(pork1);
  auto method2 = REFLEX_METHOD(pork2);
  auto method3 = REFLEX_METHOD(pork3);

  cls.add(method1);
  cls.add(method2);
  cls.add(method3);
  }

QTEST_APPLESS_MAIN(EksReflexTest)
