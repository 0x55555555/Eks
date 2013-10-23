#pragma once

#include "XGlobal"

#ifdef EKSREFLEX_BUILD
# define REFLEX_EXPORT X_DECL_EXPORT
#else
# define REFLEX_EXPORT X_DECL_IMPORT
#endif

#include <tuple>

namespace Eks
{

namespace Reflex
{

class Type
  {
  };

namespace detail
{

enum EmbeddedType
  {
  embedded_void,
  embedded_bool,
  embedded_int,
  embedded_float,
  embedded_double,

  EmbeddedTypeCount
  };

REFLEX_EXPORT const Type *findEmbeddedType(EmbeddedType t);

template <typename T> struct TypeResolver;

#define EMBEDDED_TYPE(type) \
  template <> struct TypeResolver<type> { \
  static const Type *find() { return findEmbeddedType(embedded_##type); } };

EMBEDDED_TYPE(void)
EMBEDDED_TYPE(bool)
EMBEDDED_TYPE(int)
EMBEDDED_TYPE(float)
EMBEDDED_TYPE(double)

#undef EMBEDDED_TYPE

template <typename ArgTuple> class ArgumentHelper
  {
public:
  template <typename Inv, typename... Unpacked> static void invoke(Inv &i)
    {
    typedef build_indices<std::tuple_size<ArgTuple>::value> indices;

    invokeImpl(i, indices());
    }

private:
  template <xsize... Is> struct indices { };
  template <xsize N, xsize... Is> struct build_indices : build_indices<N-1, N-1, Is...> { };
  template <xsize... Is> struct build_indices<0, Is...> : indices<Is...> { };

  template <typename Inv, typename xsize... Indices> static void invokeImpl(
        Inv &i,
        indices<Indices...>)
    {
    i(i.unpack<Indices>()...);
    }
  };

template <typename InvHelper, typename FunctionHelper, typename FunctionHelper::Signature Fn>
    struct CallHelper
  {
  static void functionWrap(typename InvHelper::CallData data)
    {
    CallHelper callHelper{ data };

    detail::ArgumentHelper<FunctionHelper::Arguments>::invoke<CallHelper>(callHelper);
    }

  template <xsize Index>
      typename std::tuple_element<Index, typename FunctionHelper::Arguments>::type unpack()
    {
    return InvHelper::unpackArgument<Index, FunctionHelper::Arguments>(data);
    }

  template <typename... Args> void operator()(Args... args)
    {
    auto ths = InvHelper::getThis<FunctionHelper::Class*>(data);

    FunctionHelper::call<Fn>(ths, std::forward<Args>(args)...);
    }

  typename InvHelper::CallData &data;
  };

template <typename FnType> class FunctionHelper;

template <typename Rt, typename Cls, typename... Args>
    class FunctionHelper<Rt(Cls::*)(Args...)>
  {
public:
  typedef std::integral_constant<bool, false> Const;
  typedef std::integral_constant<bool, false> Static;

  typedef Cls Class;
  typedef Rt ReturnType;
  typedef std::tuple<Args...> Arguments;
  typedef Rt(Class::*Signature)(Args...);

  template <Signature Fn, typename... Args> static ReturnType call(Class* cls, Args... args)
    {
    return (cls->*Fn)(std::forward<Args>(args)...);
    }
  };

template <typename Rt, typename Cls, typename... Args>
    class FunctionHelper<Rt(Cls::*)(Args...) const>
  {
  public:
  typedef std::integral_constant<bool, true> Const;
  typedef std::integral_constant<bool, false> Static;

  typedef Cls Class;
  typedef Rt ReturnType;
  typedef std::tuple<Args...> Arguments;
  typedef Rt(Class::*Signature)(Args...) const;

  template <Signature Fn, typename... Args> static ReturnType call(Class* cls, Args... args)
    {
    return (cls->*Fn)(std::forward<Args>(args)...);
    }
  };

template <typename Rt, typename... Args>
    class FunctionHelper<Rt (*)(Args...)>
  {
  public:
  typedef std::integral_constant<bool, false> Const;
  typedef std::integral_constant<bool, true> Static;

  typedef void Class;
  typedef Rt ReturnType;
  typedef std::tuple<Args...> Arguments;
  typedef ReturnType (*Signature)(Args...);

  template <Signature Fn, typename... Args> static ReturnType call(Class* cls, Args... args)
    {
    return Fn(std::forward<Args>(args)...);
    }
  };

}

template <typename T> inline const Type *findType()
  {
  typedef typename std::remove_reference<T>::type NonReferenced;
  typedef typename std::remove_const<NonReferenced>::type NonConst;
  typedef typename std::remove_pointer<NonConst>::type NonPointer;

  typedef detail::TypeResolver<NonPointer> Resolver;

  return Resolver::find();
  }

template <typename FnType, FnType Fn> class FunctionWrap
  {
public:
  typedef detail::FunctionHelper<FnType> Helper;
  typedef typename Helper::ReturnType ReturnType;
  typedef typename Helper::Arguments Arguments;

  FunctionWrap(const char* name) : _name(name)
    {
    }

  const Type *returnType() { return findType<ReturnType>(); }
  size_t argumentCount() { return std::tuple_size<Arguments>::value; }
  template <size_t N> const Type *argumentType()
    {
    typedef typename std::tuple_element<N, Arguments>::type Type;

    return findType<Type>();
    }

  template <typename T> typename T::Signature buildInvocation()
    {
    return detail::CallHelper<T, Helper, Fn>::functionWrap;
    }

private:
  const char* _name;
  };

template <typename T> class ClassWrap
  {
public:
  template <typename Type, Type Fn>
  void add(const FunctionWrap<Type, Fn>& wrapped)
    {
    (void)wrapped;
    }
  };


#define REFLEX_CLASS(cls) Eks::Reflex::ClassWrap<cls>(); typedef cls ReflexClass
#define REFLEX_METHOD_PTR(name) & ReflexClass::name
#define REFLEX_METHOD(name) Eks::Reflex::FunctionWrap<decltype(REFLEX_METHOD_PTR(name)), REFLEX_METHOD_PTR(name)>(#name)

}

}
