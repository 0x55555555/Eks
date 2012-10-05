#include "XScriptException.h"
#include "XScriptValue.h"
#include "XScriptEngine.h"

namespace XScript
{

Value toss(const QString &err)
  {
  Value ret;
  currentInterface()->throwError(&ret, err);
  return ret;
  }
}
