#pragma once

#include "XGlobal"

#ifdef EKSREFLEX_BUILD
# define REFLEX_EXPORT X_DECL_EXPORT
#else
# define REFLEX_EXPORT X_DECL_IMPORT
#endif

#include <tuple>

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

#undef EMBEDDED_TYPE

template <typename Helper, typename ArgTuple> class ArgumentHelper
  {
  template <xsize... Is> struct indices { };
  template <xsize N, xsize... Is> struct build_indices : build_indices<N-1, N-1, Is...> { };
  template <xsize... Is> struct build_indices<0, Is...> : indices<Is...> { };

  template <typename Inv, typename... Unpacked> static void invoke(Unpacked&&... args)
    {
    typedef build_indices<std::tuple_size<ArgTuple>::value> indices;

    f(std::tuple_element<Tuple, indices>(std::forward<Tuple>(t)))...

    Inv(Helper::unpack<Arg, sizeof...(Args)>(), args);
    }
  };

template <typename FnType> class FunctionHelper;

template <typename Rt, typename Cls, typename... Args> class FunctionHelper<Rt(Cls::*)(Args...)>
  {
  public:
  typedef std::integral_constant<bool, false> Const;
  typedef std::integral_constant<bool, false> Static;

  typedef Cls Class;
  typedef Rt ReturnType;
  typedef std::tuple<Args...> Arguments;

  };

template <typename Rt, typename Cls, typename... Args> class FunctionHelper<Rt(Cls::*)(Args...) const>
  {
  public:
  typedef std::integral_constant<bool, true> Const;
  typedef std::integral_constant<bool, false> Static;

  typedef Cls Class;
  typedef Rt ReturnType;
  typedef std::tuple<Args...> Arguments;
  };

template <typename Rt, typename... Args> class FunctionHelper<Rt (*)(Args...)>
  {
  public:
  typedef std::integral_constant<bool, false> Const;
  typedef std::integral_constant<bool, true> Static;

  typedef void Class;
  typedef Rt ReturnType;
  typedef std::tuple<Args...> Arguments;
  };

};

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
  typedef FunctionHelper<FnType> Helper;
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

    return findType<ArgumentAt<N>::Type>();
    }

  template <typename T> typename T::Result buildInvocation()
    {
    auto fn = functionWrap<T::ArgumentExtractor>;
    }

private:
  template <typename Arg> static void functionWrap(typename Arg::Data arg)
    {
    ArgumentHelper::invoke<
    }

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


#define REFLEX_CLASS(cls) Reflex::ClassWrap<cls>(); typedef cls ReflexClass
#define REFLEX_METHOD_PTR(name) & ReflexClass::name
#define REFLEX_METHOD(name) Reflex::FunctionWrap<decltype(REFLEX_METHOD_PTR(name)), REFLEX_METHOD_PTR(name)>(#name)

}
