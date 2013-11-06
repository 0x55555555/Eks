#pragma once
#include "XSharedPointer"

class Symbol : private ConstSharedPointer<SymbolData>
  {
public:
  Symbol();
  };

class SymbolData : Eks::detail::SharedData
  {
  };
