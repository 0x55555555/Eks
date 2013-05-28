#ifndef XINTERFACE_H
#define XINTERFACE_H

#include "XScriptGlobal.h"
#include "XScriptInterfaceBase.h"

namespace XScript
{

template <typename T> class Interface : public InterfaceBase
  {
public:

  static InterfaceBase *find()
    {
    xAssertFail();
    return 0;
    }

  static InterfaceBase *createForType()
    {
    xAssertFail();
    return 0;
    }

  static InterfaceBase *createRuntime()
    {
    xAssertFail();
    InterfaceBase *ifc = 0;
    return ifc;
    }
  };

}

#define X_SCRIPTABLE_TYPE(Type)
#define X_SCRIPTABLE_TYPE_COPYABLE(Type)
#define X_SCRIPTABLE_ABSTRACT_TYPE(Type)
#define X_SCRIPTABLE_TYPE_BASE_INHERITED(T, A)
#define X_SCRIPTABLE_TYPE_INHERITS(T, A)

#endif // XINTERFACE_H
