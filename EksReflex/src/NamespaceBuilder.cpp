#include "Reflex/NamespaceBuilder.h"

namespace Eks
{
namespace Reflex
{

Symbol NamespaceBuilder::lookupSymbol(const char* name)
  {
  if (_parent)
    {
    return _parent->lookupSymbol(name);
    }

  xAssertFail();
  return Symbol();
  }
}
}
