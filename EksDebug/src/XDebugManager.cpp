#include "XDebugManager.h"
#include "XDebugInterface.h"
#include "XDebugController.h"
#include "XDebugManagerImpl.h"
#include "Utilities/XAssert.h"

namespace Eks
{

DebugInterfaceType *g_lastInterface = 0;
DebugManagerImpl *g_manager = 0;
DebugManager::DebugManager(bool client, Watcher *w)
  {
  xAssert(!g_manager);
  g_manager = new Impl(this, client);

  g_manager->setupController();

  g_manager->_watcher = w;

  // fake connected straight away, data is buffered locally.
  g_manager->_controller->onDebuggerConnected(client);
  }

DebugManager::~DebugManager()
  {
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
  if(g_manager->_watcher)
    {
    g_manager->_watcher->onInterfaceUnregistered(ifc);
    }

  g_manager->_interfaces.removeAll(ifc);

  for(auto it = g_manager->_interfaceMap.begin(); it != g_manager->_interfaceMap.end();)
    {
    if (it->second == ifc)
      {
      it = g_manager->_interfaceMap.erase(it);
      }
    else
      {
      ++it;
      }
    }
  }

void DebugManager::addInterfaceLookup(DebugInterface *ifc)
  {
  g_manager->addInterfaceLookup(ifc);
  }

QDataStream &DebugManager::lockOutputStream(DebugInterface *ifc)
  {
  xAssert(!g_manager->_outputLocked);

  if(ifc->interfaceID() == std::numeric_limits<xuint32>::max())
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
