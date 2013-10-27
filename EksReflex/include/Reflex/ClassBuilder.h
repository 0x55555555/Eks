#pragma once
#include "Reflex/FunctionBuilder.h"
#include "Reflex/NamespaceBuilder.h"
#include "Reflex/Symbol.h"
#include "XTemporaryAllocator"
#include "XCore"
#include "XVector"

namespace Eks
{
namespace Reflex
{

/// \brief Utility to wrap a class for reflection.
template <typename Builder, typename Cls> class ClassBuilder
  {
public:
  ClassBuilder(NamespaceBuilder& ns)
      : _alloc(Eks::Core::temporaryAllocator()),
        _ns(ns)
    {
    }

  template <typename Type, Type Fn>
  void add(const FunctionBuilder<Type, Fn> &wrapped)
    {
    _functions << Function::build<FunctionBuilder<Type, Fn>>(_ns, wrapped);
    }

private:
  struct Function
    {
    template <typename Fn> static Function build(NamespaceBuilder& ns, const Fn &fn)
      {
      Function result;
      result.name = ns.lookupSymbol(fn.name());
      result.function = Builder::build<Fn>(fn);

      return result;
      }

    typename Builder::Function function;
    Symbol name;
    };

  Eks::TemporaryAllocator _alloc;
  Eks::Vector<Function> _functions;
  NamespaceBuilder& _ns;
  };

#define REFLEX_CLASS(builder, cls, ns) Eks::Reflex::ClassBuilder<builder, cls>(ns); REFLEX_FUNCTION_HELPER(cls)

}
}
