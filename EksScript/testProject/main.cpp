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


template <typename... ArgsIn> struct ArgList : std::tuple<ArgsIn...>
  {
  typedef std::tuple<ArgsIn...> ArgTuple;
  static const size_t ArgCount = sizeof...(ArgsIn);

  template <size_t i> struct Arg
    {
    typedef typename std::tuple_element<i, ArgTuple>::type type;
    };

  template <xsize N> struct Unpacker
    {
    template <typename Fn, typename... Args> static void unpackAndCall(const Fn &fn, const ArgTuple &argTuple, Args... args)
      {
      auto &&myArg = std::get<N-1>(argTuple);
      Unpacker<N-1>::unpackAndCall(fn, argTuple, myArg, std::forward<Args>(args)...);
      }
    };

  template <> struct Unpacker<0>
    {
    template <typename Fn, typename... Args> static void unpackAndCall(const Fn &fn, const ArgTuple &, Args... args)
      {
      fn(std::forward<Args>(args)...);
      }
    };
  };

class NoClass { };
template<typename T> struct Traits;

template<typename Class, typename Ret, typename... ArgsIn> struct Traits<Ret (Class::*)(ArgsIn...)>
{
  typedef Ret (Class::*Sig)(ArgsIn...);
  typedef Class ClassType;
  typedef Ret ReturnType;
  typedef ArgList<ArgsIn...> Args;

  template <typename SIG, SIG Fn> static void call(ReturnType *r, ClassType *c, Args *a)
  {
    (void)r;
    auto fn = [&c](ArgsIn... args) { (c->*Fn)(std::forward<ArgsIn>(args)...); };
    Args::Unpacker<Args::ArgCount>::unpackAndCall(fn, *a);
    }
  };

template<typename Class, typename Ret, typename... ArgsIn> struct Traits<Ret (Class::*)(ArgsIn...) const>
{
  typedef Ret (Class::*Sig)(ArgsIn...) const;
  typedef Class ClassType;
  typedef Ret ReturnType;
  typedef ArgList<ArgsIn...> Args;

  template <typename SIG, SIG Fn> static void call(ReturnType *r, ClassType *c, Args *a)
  {
    (void)r;
    auto fn = [&c](ArgsIn... args) { (c->*Fn)(std::forward<ArgsIn>(args)...); };
    Args::Unpacker<Args::ArgCount>::unpackAndCall(fn, *a);
    }
  };

template<typename Ret, typename... ArgsIn> struct Traits<Ret (*)(ArgsIn...)>
  {
  typedef Ret (*Sig)(ArgsIn...);
  typedef NoClass ClassType;
  typedef Ret ReturnType;
  typedef ArgList<ArgsIn...> Args;

  template <typename SIG, SIG Fn> static void call(ReturnType *r, ClassType *c, Args *a)
    {
    (void)c;
    (void)r;
    auto fn = [](ArgsIn... args) { Fn(std::forward<ArgsIn>(args)...); };
    Args::Unpacker<Args::ArgCount>::unpackAndCall(fn, *a);
    }
  };

template <typename SIG, SIG T> class Binder
  {
public:
  typedef Traits<SIG> Traits;

  static void call(typename Traits::ReturnType *result, typename Traits::ClassType *cls, typename Traits::Args *args)
    {
    Traits::call<SIG, T>(result, cls, args);
    }
  };

#define X_BIND_FUNCTION(fn) Binder<decltype(fn), fn>

X_SCRIPTABLE_TYPE(Nummer)

int main(int , char* [])
{
  typedef X_BIND_FUNCTION(&Nummer::crapSalad) CallA;
  typedef X_BIND_FUNCTION(&Nummer::cake) CallB;
  typedef X_BIND_FUNCTION(&thing) CallC;
  typedef X_BIND_FUNCTION(&Nummer::cake2) CallD;

  CallA::call(0, 0, 0);
  CallB::call(0, 0, 0);
  CallC::call(0, 0, 0);
  CallD::call(0, 0, 0);

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
