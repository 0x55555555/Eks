#ifndef XDEBUGMANAGER_H
#define XDEBUGMANAGER_H

#include "XDebugGlobal.h"
#include "XUniquePointer"
#include "XCore"

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
public:
  class Watcher
    {
  public:
    virtual void onInterfaceRegistered(Eks::DebugInterface *) = 0;
    virtual void onInterfaceUnregistered(Eks::DebugInterface *) = 0;
    };

  DebugManager(bool client, Watcher *watch = 0);
  ~DebugManager();


  typedef DebugInterface *(*CreateInterfaceFunction)(DebugManager *m, bool client);
  typedef void (*DestroyInterfaceFunction)(DebugInterface *ifc);
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
  };

#endif

}

#endif // XDEBUGMANAGER_H
