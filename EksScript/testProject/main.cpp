#include "XScriptEngine.h"
#include "XInterface.h"
#include "XAssert"
#include <tuple>

struct Nummer
  {
  double num;

  Nummer(float n) : num(n) {}

  float crapSalad(float newNum)
    {
    num += newNum;

    return num;
  }

  static float cake(int)
    {
    return 0.0f;
    }

  float cake2(int, float) const
    {
    return 0.0f;
    } 
  };

void thing()
  {
  }

namespace detail
{

template <typename... Fn> struct FunctionList : std::tuple<Fn...>
  {
  static const size_t size = sizeof...(Fn);
  template <size_t i> struct at : std::tuple_element<i, std::tuple<Fn...>>::type
    {
    };
  };

template <xsize N> struct ArgUnpacker
  {
  template <typename Fn, typename ArgPack, typename... Args> static void unpackAndCall(const Fn &fn, ArgPack &argPack, Args... args)
    {
    ArgUnpacker<N-1>::unpackAndCall(fn, argPack, argPack.get<N-1>(), std::forward<Args>(args)...);
    }
  };

template <> struct ArgUnpacker<0>
  {
  template <typename Fn, typename ArgPack, typename... Args> static void unpackAndCall(const Fn &fn, ArgPack &, Args... args)
    {
    fn(std::forward<Args>(args)...);
    }
  };

template <typename... ArgsIn> struct ArgList : std::tuple<ArgsIn...>
  {
  typedef std::tuple<ArgsIn...> ArgTuple;
  static const size_t ArgCount = sizeof...(ArgsIn);

  ArgList(ArgsIn... args) : std::tuple<ArgsIn...>(std::make_tuple(std::forward<ArgsIn>(args)...))
    {
    }

  template <size_t i> struct Arg
    {
    typedef typename std::tuple_element<i, ArgTuple>::type Type;
    };

  template <xsize I> typename Arg<I>::Type get()
    {
    return std::get<I>(*this);
    }
  };

class NoClass { };
template<typename T> struct Traits;

template<typename Class, typename... ArgsIn> struct Traits<void (Class::*)(ArgsIn...)>
{
  typedef void (Class::*Sig)(ArgsIn...);
  typedef Class ClassType;
  typedef void ReturnType;
  typedef ArgList<ArgsIn...> ArgData;

  template <typename SIG, SIG Fn, typename Args> static void call(ReturnType *, ClassType *c, Args *a)
    {
    auto fn = [c](ArgsIn... args) { (c->*Fn)(std::move(args)...); };

    ArgUnpacker<ArgData::ArgCount>::unpackAndCall(fn, *a);
    }
  };

template<typename Class, typename... ArgsIn> struct Traits<void (Class::*)(ArgsIn...) const>
{
  typedef void (Class::*Sig)(ArgsIn...) const;
  typedef Class ClassType;
  typedef void ReturnType;
  typedef ArgList<ArgsIn...> ArgData;

  template <typename SIG, SIG Fn, typename Args> static void call(ReturnType *, ClassType *c, Args *a)
    {
    auto fn = [&c](ArgsIn... args) { (c->*Fn)(std::move(args)...); };

    ArgUnpacker<ArgData::ArgCount>::unpackAndCall(fn, *a);
    }
  };

template<typename... ArgsIn> struct Traits<void (*)(ArgsIn...)>
  {
  typedef void (*Sig)(ArgsIn...);
  typedef NoClass ClassType;
  typedef void ReturnType;
  typedef ArgList<ArgsIn...> ArgData;

  template <typename SIG, SIG Fn, typename Args> static void call(ReturnType *, ClassType *, Args *a)
    {
    auto fn = [](ArgsIn... args) { Fn(std::move(args)...); };

    ArgUnpacker<ArgData::ArgCount>::unpackAndCall(fn, *a);
    }
  };

template<typename Class, typename Ret, typename... ArgsIn> struct Traits<Ret (Class::*)(ArgsIn...)>
{
  typedef Ret (Class::*Sig)(ArgsIn...);
  typedef Class ClassType;
  typedef Ret ReturnType;
  typedef ArgList<ArgsIn...> ArgData;

  template <typename SIG, SIG Fn, typename Args> static void call(ReturnType *r, ClassType *c, Args *a)
    {
    auto fn = [r, c](ArgsIn... args) { *r = (c->*Fn)(std::move(args)...); };

    ArgUnpacker<ArgData::ArgCount>::unpackAndCall(fn, *a);
    }
  };

template<typename Class, typename Ret, typename... ArgsIn> struct Traits<Ret (Class::*)(ArgsIn...) const>
{
  typedef Ret (Class::*Sig)(ArgsIn...) const;
  typedef Class ClassType;
  typedef Ret ReturnType;
  typedef ArgList<ArgsIn...> ArgData;

  template <typename SIG, SIG Fn, typename Args> static void call(ReturnType *r, ClassType *c, Args *a)
    {
    auto fn = [r, c](ArgsIn... args) { *r = (c->*Fn)(std::move(args)...); };

    ArgUnpacker<ArgData::ArgCount>::unpackAndCall(fn, *a);
    }
  };

template<typename Ret, typename... ArgsIn> struct Traits<Ret (*)(ArgsIn...)>
  {
  typedef Ret (*Sig)(ArgsIn...);
  typedef NoClass ClassType;
  typedef Ret ReturnType;
  typedef ArgList<ArgsIn...> ArgData;

  template <typename SIG, SIG Fn, typename Args> static void call(ReturnType *r, ClassType *, Args *a)
    {
    auto fn = [r](ArgsIn... args) { *r = Fn(std::move(args)...); };

    ArgUnpacker<ArgData::ArgCount>::unpackAndCall(fn, *a);
    }
  };
}

template <typename SIG, SIG T> class Binder
  {
public:
  typedef detail::Traits<SIG> Traits;

  static void call(typename Traits::ReturnType *result, typename Traits::ClassType *cls, typename Traits::ArgData *args)
    {
    Traits::call<SIG, T>(result, cls, args);
    }
  };

#define X_BIND_FUNCTION(fn) Binder<decltype(fn), fn>

X_SCRIPTABLE_TYPE(Nummer)

int main(int , char* [])
{
  typedef detail::FunctionList<
    X_BIND_FUNCTION(&Nummer::crapSalad),
    X_BIND_FUNCTION(&Nummer::cake),
    X_BIND_FUNCTION(&thing),
    X_BIND_FUNCTION(&Nummer::cake2)
    > List;

  auto argsA = List::at<0>::Traits::ArgData(5.0f);
  auto argsB = List::at<1>::Traits::ArgData(5);
  auto argsC = List::at<2>::Traits::ArgData();
  auto argsD = List::at<3>::Traits::ArgData(5, 5);

  Nummer n(0.0);
  float val = 0.0f;

  List::at<0>::call(&val, &n, &argsA);
  List::at<1>::call(&val, 0, &argsB);
  List::at<2>::call(0, 0, &argsC);
  List::at<3>::call(&val, 0, &argsD);

  /*XScriptEngine::initiate();

  XScriptEngine eng(false);

  XInterface<Nummer>* nummer = XInterface<Nummer>::create("Nummer");
  nummer->addConstructor<Nummer* (float)>();
  nummer->addMethod<float (float), &Nummer::crapSalad>("crapSalad");
  nummer->seal();

  eng.addInterface(nummer);
  
  const char* kScriptChars =
  "#import(\"Nummer\");"
  "String main() {\n"
  "  Nummer obj = new Nummer(45.5);\n"
  "  obj.crapSalad(5.5);"
  "  return obj.crapSalad(5.0);"
  "}\n";

  XScriptValue val = eng.run(kScriptChars);

  qDebug() << val.toString();
  */
  return 0;
}
