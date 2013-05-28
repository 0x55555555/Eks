#ifndef XSCRIPTFUNCTIONWRAPPER_H
#define XSCRIPTFUNCTIONWRAPPER_H

#include <tuple>

namespace XScript
{

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


}

#endif // XSCRIPTFUNCTIONWRAPPER_H
