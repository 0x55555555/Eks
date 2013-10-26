#pragma once

namespace Eks
{
namespace Reflex
{

/// \brief Utility to wrap a class for reflection.
template <typename T> class ClassWrap
  {
public:
  template <typename Type, Type Fn>
  void add(const FunctionWrap<Type, Fn>& wrapped)
    {
    (void)wrapped;
    }
  };

#define REFLEX_CLASS(cls) Eks::Reflex::ClassWrap<cls>(); REFLEX_FUNCTION_HELPER(cls)

}
}
