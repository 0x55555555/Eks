#ifndef XDEBUGMANAGERIMPL_H
#define XDEBUGMANAGERIMPL_H

#include "XDebugManager.h"
#include "XDebugController.h"
#include "QObject"
#include "QBuffer"
#include "XUnorderedMap"

class QTcpSocket;
class QTcpServer;

class XDebugManagerImpl : private QObject
  {
  Q_OBJECT

public:
  XDebugManagerImpl(bool client);
  ~XDebugManagerImpl();

  XUnorderedMap<xuint32, XDebugInterface *> _interfaceMap;
  QList<XDebugInterface *> _interfaces;

  QByteArray _preConnectClientData;
  QDataStream _clientStream;

  QBuffer _scratchImpl;
  QDataStream _scratchBuffer;

  XDebugInterface *_outputLocked;

  QTcpServer *_server;
  QTcpSocket *_client;

  xuint32 _readingID;
  xuint32 _bytesNeeded;

  XDebugController *_controller;

  void flush();
  void setupClient();
  void addInterfaceLookup(XDebugInterface *ifc);

private slots:
  void onNewConnection();
  void onDataReady();
  void onConnected();
  };

#endif // XDEBUGMANAGERIMPL_H
