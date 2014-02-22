#include "XDebugInterface.h"
#include "Utilities/XTemplateHelpers.h"
#include "QDataStream"
#include "QDebug"

namespace Eks
{

DebugInterface::OutputTunnel::OutputTunnel(DebugInterface *ifc)
    : _stream(DebugManager::lockOutputStream(ifc))
  {
  }

DebugInterface::OutputTunnel::~OutputTunnel()
  {
  DebugManager::unlockOutputStream();
  }

DebugInterface::DebugInterface()
    : _interfaceID(Eks::maxFor(_interfaceID)),
      _dataModel(0)
  {
  DebugManager::registerInterface(this);
  }

DebugInterface::~DebugInterface()
  {
  DebugManager::unregisterInterface(this);
  }

void DebugInterface::setRecievers(const Reciever *r, xsize c)
  {
  _recievers = r;
  _recieverCount = c;
  }

void DebugInterface::onDataRecieved(QDataStream& data)
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

}
