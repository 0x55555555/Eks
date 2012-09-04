#ifndef XSCRIPTDEFINITIONS_H
#define XSCRIPTDEFINITIONS_H

#include "XAssert"

namespace XScript
{

class InterfaceBase;

template <typename T> const InterfaceBase* findInterface(const T*);

template <typename T> class ClassCreator_Factory
  {
public:
  typedef T * ReturnType;
  template <typename Args> static ReturnType Create( Object &, Args const & )
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
