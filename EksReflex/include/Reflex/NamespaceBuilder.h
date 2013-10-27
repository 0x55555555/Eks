#pragma once

#include "Reflex/Symbol.h"

namespace Eks
{
namespace Reflex
{

class NamespaceBuilder
  {
public:
  NamespaceBuilder(const char* name, NamespaceBuilder *parent)
      : _parent(parent),
        _name(lookupSymbol(name))
    {
    }

  Symbol lookupSymbol(const char*);

private:
  NamespaceBuilder *_parent;
  Symbol _name;
  };

}
}

#define REFLEX_NAMESPACE(name, parent) Eks::Reflex::NamespaceBuilder(name, parent)
#define REFLEX_GLOBAL_NAMESPACE(name) REFLEX_NAMESPACE(name, nullptr)
