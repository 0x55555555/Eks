#include "XDebugInterface.h"
#include "QDebug"


XDebugInterface::OutputTunnel::OutputTunnel(XDebugInterface *ifc)
    : _stream(XDebugManager::lockOutputStream(ifc))
  {
  }

XDebugInterface::OutputTunnel::~OutputTunnel()
  {
  XDebugManager::unlockOutputStream();
  }

XDebugInterface::XDebugInterface() : _interfaceID(X_UINT32_SENTINEL)
  {
  XDebugManager::registerInterface(this);
  }

XDebugInterface::~XDebugInterface()
  {
  XDebugManager::unregisterInterface(this);
  }

void XDebugInterface::setRecievers(const Reciever *r, xsize c)
  {
  _recievers = r;
  _recieverCount = c;
  }

void XDebugInterface::onDataRecieved(QDataStream& data)
  {
  xuint8 id;
  data >> id;

  for(xsize i = 0; i < _recieverCount; ++i)
    {
    const Reciever &r = _recievers[i];

    if(r.type == id)
      {
      r.fn(id, this, data);
      return;
      }
    }

  xAssertFail();
  qWarning() << "Unknown message type recieved";
  }
