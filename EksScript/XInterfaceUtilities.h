#ifndef XINTERFACEUTILITIES_H
#define XINTERFACEUTILITIES_H

#include "XScriptObject.h"

namespace XScript
{

template <typename T> Object Object::newInstance(Interface<T>* i)
  {
  return i->newInstance(0, NULL);
  }

}

#endif // XINTERFACEUTILITIES_H
