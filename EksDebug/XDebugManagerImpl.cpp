#include "XDebugManagerImpl.h"
#include "XDebugInterface.h"
#include "QBuffer"
#include "QTcpServer"
#include "QTcpSocket"

namespace Eks
{

DebugManagerImpl::DebugManagerImpl(bool client)
  : _clientStream(&_preConnectClientData, QIODevice::WriteOnly),
    _scratchBuffer(&_scratchImpl),
    _client(0),
    _server(0),
    _outputLocked(0),
    _controller(0),
    _readingID(X_UINT32_SENTINEL),
    _bytesNeeded(0)
  {
  _scratchImpl.open(QIODevice::WriteOnly);

  if(client)
    {
    _client = new QTcpSocket(this);
    _client->connectToHost(QHostAddress::LocalHost, 12345);

    connect(_client, SIGNAL(connected()), this, SLOT(onConnected()));
    setupClient();
    }
  else
    {
    _server = new QTcpServer(this);
    connect(_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    _server->listen(QHostAddress::Any, 12345);
    }
  }

DebugManagerImpl::~DebugManagerImpl()
  {
  _client = 0;
  _server = 0;
  }

void DebugManagerImpl::setupClient()
  {
  connect(_client, SIGNAL(readyRead()), this, SLOT(onDataReady()));
  }

void DebugManagerImpl::addInterfaceLookup(DebugInterface *ifc)
  {
  xAssert(!_interfaceMap.contains(ifc->interfaceID()));
  xAssert(ifc->interfaceID() != X_UINT32_SENTINEL);
  _interfaceMap[ifc->interfaceID()] = ifc;

  xAssert(_interfaces.size() == _interfaceMap.size());
  }

void DebugManagerImpl::flush()
  {
  xuint32 id = _outputLocked->interfaceID();

  QByteArray& buf = _scratchImpl.buffer();

  _clientStream << id << (xuint32)buf.length();
  _clientStream.writeRawData(buf, buf.length());
  }

void DebugManagerImpl::onConnected()
  {
  _clientStream.setDevice(_client);
  _clientStream.writeRawData(_preConnectClientData, _preConnectClientData.size());
  _preConnectClientData.clear();
  _preConnectClientData.squeeze();
  }

void DebugManagerImpl::onDataReady()
  {
  xAssert(!_outputLocked);

  static const xsize HeaderSize = 8;

  xAssert(_interfaces.size() == _interfaceMap.size());

  // while headers are available
  xuint32 av = _client->bytesAvailable();
  while(av > HeaderSize)
    {
    if(_readingID == X_UINT32_SENTINEL)
      {
      _clientStream >> _readingID >> _bytesNeeded;
      av -= HeaderSize;
      }

    // not enough data yet
    if(av < _bytesNeeded)
      {
      return;
      }

    DebugInterface *ifc = _interfaceMap.value(_readingID, 0);
    xAssert(ifc);

    ifc->onDataRecieved(_clientStream);
    _readingID = X_UINT32_SENTINEL;

    av = _client->bytesAvailable();
    }
  }

void DebugManagerImpl::onNewConnection()
  {
  while(QTcpSocket *s = _server->nextPendingConnection())
    {
    if(_client)
      {
      qCritical() << "New client detected, arp, need to wipe stuff down!";
      _client = 0;
      xAssertFail();
      }
    _client = s;

    onConnected();

    setupClient();
    }
  }

}
