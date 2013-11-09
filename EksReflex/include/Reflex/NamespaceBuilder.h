#pragma once

#include "Reflex/Symbol.h"
#include "XGlobal"

namespace Eks
{
namespace Reflex
{

class BuilderBase
  {
public:
  virtual Symbol lookupSymbol(const char*) = 0;
  };

class REFLEX_EXPORT NamespaceBuilder : public BuilderBase
  {
public:
  NamespaceBuilder(const char* name, BuilderBase &parent)
      : _parent(&parent),
        _name(lookupSymbol(name))
    {
    }

  Symbol lookupSymbol(const char*) X_OVERRIDE;

private:
  BuilderBase *_parent;
  Symbol _name;
  };

}
}

#define REFLEX_NAMESPACE(name, parent) Eks::Reflex::NamespaceBuilder(name, parent)
