#include "XDebugManager.h"
#include "XDebugInterface.h"
#include "XDebugController.h"
#include "XDebugManagerImpl.h"
#include "XAssert"

namespace Eks
{

DebugInterfaceType *g_lastInterface = 0;
DebugManagerImpl *g_manager = 0;
DebugManager::DebugManager(bool client)
  {
  xAssert(!g_manager);
  g_manager = new Impl(client);
  _controller = new DebugController(client);

  g_manager->_controller = _controller;
  g_manager->addInterfaceLookup(_controller);

  // fake connected straight away, data is buffered locally.
  _controller->onDebuggerConnected(client);
  }

DebugManager::~DebugManager()
  {
  delete _controller;

  xAssert(g_manager);
  delete g_manager;
  g_manager = 0;
  }

const DebugInterfaceType *DebugManager::findInterfaceType(const QString &t)
  {
  for(auto ifc = g_lastInterface; ifc != 0; ifc = ifc->next)
    {
    if(ifc->type == t)
      {
      return ifc;
      }
    }

  return 0;
  }

void DebugManager::registerInterfaceType(DebugInterfaceType *t)
  {
  t->next = g_lastInterface;
  g_lastInterface = t;
  }

void DebugManager::registerInterface(DebugInterface *ifc)
  {
  g_manager->_interfaces << ifc;
  }

void DebugManager::unregisterInterface(DebugInterface *ifc)
  {
  g_manager->_interfaces.removeAll(ifc);
  }

void DebugManager::addInterfaceLookup(DebugInterface *ifc)
  {
  g_manager->addInterfaceLookup(ifc);
  }

QDataStream &DebugManager::lockOutputStream(DebugInterface *ifc)
  {
  xAssert(!g_manager->_outputLocked);

  if(ifc->interfaceID() == X_UINT32_SENTINEL)
    {
    g_manager->_controller->setupInterface(ifc);

    g_manager->addInterfaceLookup(ifc);
    }

  g_manager->_outputLocked = ifc;

  g_manager->_scratchImpl.buffer().clear();
  g_manager->_scratchImpl.reset();
  return g_manager->_scratchBuffer;
  }

void DebugManager::unlockOutputStream()
  {
  xAssert(g_manager->_outputLocked);

  g_manager->flush();

  g_manager->_outputLocked = 0;
  }

}
