#include "XDebugManager.h"
#include "XDebugInterface.h"
#include "XDebugController.h"
#include "XDebugManagerImpl.h"
#include "XAssert"

XDebugInterfaceType *g_lastInterface = 0;
XDebugManagerImpl *g_manager = 0;
XDebugManager::XDebugManager(bool client)
  {
  xAssert(!g_manager);
  g_manager = new Impl(client);
  _controller = new XDebugController(client);

  g_manager->_controller = _controller;
  g_manager->addInterfaceLookup(_controller);

  // fake connected straight away, data is buffered locally.
  _controller->onDebuggerConnected(client);
  }

XDebugManager::~XDebugManager()
  {
  delete _controller;

  xAssert(g_manager);
  delete g_manager;
  g_manager = 0;
  }

const XDebugInterfaceType *XDebugManager::findInterfaceType(const QString &t)
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

void XDebugManager::registerInterfaceType(XDebugInterfaceType *t)
  {
  t->next = g_lastInterface;
  g_lastInterface = t;
  }

void XDebugManager::registerInterface(XDebugInterface *ifc)
  {
  g_manager->_interfaces << ifc;
  }

void XDebugManager::unregisterInterface(XDebugInterface *ifc)
  {
  g_manager->_interfaces.removeAll(ifc);
  }

void XDebugManager::addInterfaceLookup(XDebugInterface *ifc)
  {
  g_manager->addInterfaceLookup(ifc);
  }

QDataStream &XDebugManager::lockOutputStream(XDebugInterface *ifc)
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

void XDebugManager::unlockOutputStream()
  {
  xAssert(g_manager->_outputLocked);

  g_manager->flush();

  g_manager->_outputLocked = 0;
  }
