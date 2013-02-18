#ifndef XAPPLICATIONDEBUG_H
#define XAPPLICATIONDEBUG_H

#include "XVector"
#include "XDebugGlobal.h"
#include "XDebugInterface.h"

namespace Eks
{

struct Init;
struct SetupInterface;

class DebugController : public DebugInterface
  {
  X_DEBUG_INTERFACE(DebugController)

public:
  void onDebuggerConnected(bool client);
  void setupInterface(DebugInterface *ifc);

private:
  void onInit(const Init &);
  void onSetupInterface(const SetupInterface &);

  xuint32 _maxInteface;
  Eks::Vector<DebugInterface *> _createdInterfaces;
  bool _isClient;
  };

}


#endif // XAPPLICATIONDEBUG_H
