#include "XReflexTest.h"
#include "XReflex.h"
#include <QtTest>

class A
  {
public:
  int pork1(const float&);
  int pork2(A *) const;
  static A* pork3(const float&);
  };

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

class InvocationBuilder
  {
  class Arguments
    {
    };

  class Result
    {
    void (*call)(Arguments *);
    };


  };

void EksReflexTest::functionWrapTest()
  {
  typedef A ReflexClass;

  auto method1 = REFLEX_METHOD(pork1);
  auto method2 = REFLEX_METHOD(pork2);
  auto method3 = REFLEX_METHOD(pork3);

  QCOMPARE(method1.returnType(), Reflex::findType<int>());
  QCOMPARE(method2.returnType(), Reflex::findType<int>());
  QCOMPARE(method3.returnType(), Reflex::findType<A*>());

  QCOMPARE(method1.argumentCount(), 1U);
  QCOMPARE(method2.argumentCount(), 1U);
  QCOMPARE(method3.argumentCount(), 1U);

  QCOMPARE(method1.argumentType<0>(), Reflex::findType<const float&>());
  QCOMPARE(method2.argumentType<0>(), Reflex::findType<A *>());
  QCOMPARE(method3.argumentType<0>(), Reflex::findType<const float &>());

  QCOMPARE(decltype(method1)::Helper::Const::value, false);
  QCOMPARE(decltype(method2)::Helper::Const::value, true);
  QCOMPARE(decltype(method3)::Helper::Const::value, false);

  QCOMPARE(decltype(method1)::Helper::Static::value, false);
  QCOMPARE(decltype(method2)::Helper::Static::value, false);
  QCOMPARE(decltype(method3)::Helper::Static::value, true);

  QCOMPARE(Reflex::findType<decltype(method1)::Helper::Class>(), Reflex::findType<A>());
  QCOMPARE(Reflex::findType<decltype(method2)::Helper::Class>(), Reflex::findType<A>());
  QCOMPARE(Reflex::findType<decltype(method3)::Helper::Class>(), Reflex::findType<void>());
  }

void EksReflexTest::functionInvokeTest()
  {
  auto method1 = REFLEX_METHOD(pork1);
  auto method2 = REFLEX_METHOD(pork2);
  auto method3 = REFLEX_METHOD(pork3);

  auto inv1 = method1.buildInvocation<InvocationBuilder>();
  auto inv2 = method2.buildInvocation<InvocationBuilder>();
  auto inv3 = method3.buildInvocation<InvocationBuilder>();
  }

void EksReflexTest::classWrapTest()
  {
  auto cls = REFLEX_CLASS(A);

  auto method = REFLEX_METHOD(pork);
  auto method2 = REFLEX_METHOD(pork2);
  auto method3 = REFLEX_METHOD(pork3);

  cls.add(method);
  cls.add(method2);
  cls.add(method3);
  }

QTEST_APPLESS_MAIN(EksReflexTest)
