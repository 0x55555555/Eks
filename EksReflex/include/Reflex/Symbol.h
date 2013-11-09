#pragma once
#include "XSharedPointer"
#include "Reflex/Global.h"

namespace Eks
{

namespace Reflex
{

class SymbolData;

class Symbol : private ConstSharedPointer<SymbolData>
  {
public:
  Symbol() { }
  };

class SymbolData : public Eks::detail::SharedData
  {
  };

}

}
