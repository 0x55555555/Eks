#ifndef XDEBUGMANAGERIMPL_H
#define XDEBUGMANAGERIMPL_H

#include "XDebugManager.h"
#include "XDebugController.h"
#include "QObject"
#include "QBuffer"
#include "Containers/XUnorderedMap.h"

class QTcpSocket;
class QTcpServer;

namespace Eks
{

class DebugManagerImpl : private QObject
  {
  Q_OBJECT

public:
  DebugManagerImpl(DebugManager *m, bool client);
  ~DebugManagerImpl();

  DebugController *_controller;
  DebugManager::Watcher *_watcher;

  Eks::UnorderedMap<xuint32, DebugInterface *> _interfaceMap;
  QList<DebugInterface *> _interfaces;

  QByteArray _preConnectClientData;
  QDataStream _clientStream;

  QBuffer _scratchImpl;
  QDataStream _scratchBuffer;

  DebugInterface *_outputLocked;

  QTcpServer *_server;
  QTcpSocket *_client;

  xuint32 _readingID;
  xuint32 _bytesNeeded;

  DebugManager *_manager;

  void setupController();
  void clear();

  void flush();
  void setupClient();
  void addInterfaceLookup(DebugInterface *ifc);

private Q_SLOTS:
  void onNewConnection();
  void onDataReady();
  void onConnected();
  };

}

#endif // XDEBUGMANAGERIMPL_H
