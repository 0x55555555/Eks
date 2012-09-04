#include "XFunctions.h"

/*
struct UnlockerImpl
  {
  v8::Unlocker data;
  };
xCompileTimeAssert(sizeof(UnlockerImpl) == sizeof(XScript::Unlock));
*/

namespace XScript
{
Unlock::Unlock()
  {
  xAssertFail();
  //new(this) UnlockerImpl;
  }

Unlock::~Unlock()
  {
  //UnlockerImpl *impl = (UnlockerImpl*)this;
  //impl->~UnlockerImpl();
  }
}
