#ifndef XDEBUGMANAGER_H
#define XDEBUGMANAGER_H

#include "XDebugGlobal.h"

class QIODevice;

namespace Eks
{

#ifdef X_ENABLE_APPLICATION_DEBUGGING
class DebugInterface;
class DebugManagerImpl;
class DebugInterfaceType;
class DebugController;

class EKSDEBUG_EXPORT DebugManager
  {
  DebugManager(bool client);

public:
  ~DebugManager();

  typedef DebugInterface *(*CreateInterfaceFunction)(bool client);
  static const DebugInterfaceType *findInterfaceType(const QString &);
  static void registerInterfaceType(DebugInterfaceType *);
  static void registerInterface(DebugInterface *ifc);
  static void unregisterInterface(DebugInterface *ifc);
  static void addInterfaceLookup(DebugInterface *ifc);

  static QDataStream &lockOutputStream(DebugInterface *ifc);
  static void unlockOutputStream();

private:
  typedef DebugManagerImpl Impl;
  Impl *_impl;
  DebugController *_controller;
  };

#endif

}

#endif // XDEBUGMANAGER_H
