#ifndef XSCRIPTDEFINITIONS_H
#define XSCRIPTDEFINITIONS_H

#include "XAssert"

class XInterfaceBase;

template <typename T> const XInterfaceBase* findInterface(const T*);

namespace XScript
{

template <typename T> class ClassCreator_Factory
  {
public:
  typedef T * ReturnType;
  static ReturnType Create( XScriptObject &, XScriptArguments const & )
    {
    xAssertFail();
    return 0;
    }

  static void Delete( T * obj )
    {
    xAssertFail();
    delete obj;
    }
  };

}

#endif // XSCRIPTDEFINITIONS_H
