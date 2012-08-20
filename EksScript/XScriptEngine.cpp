#include "XScriptValueDartInternals.h"
#include "XScriptValueV8Internals.h"

#ifndef X_DART
# include "v8-debug.h"
#endif

#include "XScriptGlobal.h"
#include "XScriptEngine.h"
#include "XScriptValue.h"
#include "XInterface.h"
#include "XConvertToScript.h"
#include "XQObjectWrapper.h"
#include "XQtWrappers.h"

namespace XScript
{
EngineInterface *createV8Interface(bool debugging);
EngineInterface *createDartInterface(bool debugging);

xCompileTimeAssert(Engine::InterfaceCount == 2);


struct StaticEngine
  {
  StaticEngine(bool debugging)
    {
    engines[0] = createV8Interface(debugging);
    engines[1] = createDartInterface(debugging);
    }

  ~StaticEngine()
    {
    xForeach(EngineInterface *i, engines)
      {
      delete i;
      }
    }

  EngineInterface *engines[Engine::MaxInterfaces];

  EngineInterface *currentInterface;
  };

StaticEngine *g_engine = 0;

void Engine::initiate(bool debugging)
  {
  if(g_engine)
    {
    xAssertFail();
    return;
    }

  g_engine = new StaticEngine(debugging);
  }

void Engine::terminate()
  {
  delete g_engine;
  }

void Engine::initiateQtWrappers()
  {
  QObjectWrapper::initiate();
  QtWrappers::initiate();
  }

QString getDartUrl(const XInterfaceBase* i)
  {
  return i->typeName();
  }

void Engine::addInterface(const XInterfaceBase *i)
  {
  xForeach(EngineInterface *eng, g_engine->engines)
    {
    eng->addInterface(i);
    }
  }

void Engine::removeInterface(const XInterfaceBase *i)
  {
  xForeach(EngineInterface *eng, g_engine->engines)
    {
    eng->removeInterface(i);
    }
  }

Engine::Walker Engine::interfaces()
  {
  Engine::Walker wlk = { g_engine->engines, g_engine->engines + InterfaceCount };
  return wlk;
  }

void Engine::adjustAmountOfExternalAllocatedMemory(int in)
  {
  (void)in;
  }

EngineInterface *currentInterface()
  {
  xAssert(g_engine->currentInterface);
  return g_engine->currentInterface;
  }

EngineInterface *Engine::beginScope(EngineInterface *ifc)
  {
  EngineInterface *old = g_engine->currentInterface;
  g_engine->currentInterface = ifc;
  return old;
  }


void Engine::endScope(EngineInterface *ifc, EngineInterface *oldIfc)
  {
  g_engine->currentInterface = oldIfc;
  }

}
