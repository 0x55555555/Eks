#include "XFunctions.h"
#include "XScriptValueV8Internals.h"

struct UnlockerImpl
  {
  v8::Unlocker data;
  };
xCompileTimeAssert(sizeof(UnlockerImpl) == sizeof(XScript::Unlock));

namespace XScript
{
Unlock::Unlock()
  {
  //new(this) UnlockerImpl;
  }

Unlock::~Unlock()
  {
  //UnlockerImpl *impl = (UnlockerImpl*)this;
  //impl->~UnlockerImpl();
  }
}
