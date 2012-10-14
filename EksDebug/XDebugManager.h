#ifndef XDEBUGMANAGER_H
#define XDEBUGMANAGER_H

#include "XDebugGlobal.h"

class XDebugInterface;
class XDebugManagerImpl;
class XDebugInterfaceType;
class QIODevice;
class XDebugController;

class EKSDEBUG_EXPORT XDebugManager
  {
#ifdef X_ENABLE_APPLICATION_DEBUGGING
public:
#else
private:
#endif
  XDebugManager(bool client);

public:
  ~XDebugManager();

  typedef XDebugInterface *(*CreateInterfaceFunction)(bool client);
  static const XDebugInterfaceType *findInterfaceType(const QString &);
  static void registerInterfaceType(XDebugInterfaceType *);
  static void registerInterface(XDebugInterface *ifc);
  static void unregisterInterface(XDebugInterface *ifc);
  static void addInterfaceLookup(XDebugInterface *ifc);

  static QDataStream &lockOutputStream(XDebugInterface *ifc);
  static void unlockOutputStream();

private:
  typedef XDebugManagerImpl Impl;
  Impl *_impl;
  XDebugController *_controller;
  };

#endif // XDEBUGMANAGER_H
