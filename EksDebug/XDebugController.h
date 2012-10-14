#ifndef XAPPLICATIONDEBUG_H
#define XAPPLICATIONDEBUG_H

#include "XDebugGlobal.h"
#include "XDebugInterface.h"

struct Init;
struct SetupInterface;

class XDebugController : public XDebugInterface
  {
  X_DEBUG_INTERFACE(XDebugController)

public:
  void onDebuggerConnected(bool client);
  void setupInterface(XDebugInterface *ifc);

private:
  void onInit(const Init &);
  void onSetupInterface(const SetupInterface &);

  xuint32 _maxInteface;
  QVector<XDebugInterface *> _createdInterfaces;
  bool _isClient;
  };


#endif // XAPPLICATIONDEBUG_H
