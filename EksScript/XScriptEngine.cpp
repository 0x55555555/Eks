#include "XScriptGlobal.h"
#include "XScriptEngine.h"
#include "XScriptValue.h"
#include "XInterface.h"
#include "XConvertToScript.h"
#include "XQObjectWrapper.h"
#include "XQtWrappers.h"

namespace XScript
{

#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
EngineInterface *createV8Interface(bool debugging);
#endif

#ifdef X_SCRIPT_ENGINE_ENABLE_DART
EngineInterface *createDartInterface(bool debugging);
#endif

struct StaticEngine
  {
  StaticEngine(bool debugging)
    {
    (void)debugging;
    xsize idx = 0;

#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
    engines[idx++] = createV8Interface(debugging);
#endif

#ifdef X_SCRIPT_ENGINE_ENABLE_DART
    engines[idx++] = createDartInterface(debugging);
#endif
    }

  ~StaticEngine()
    {
    xForeach(EngineInterface *i, engines)
      {
      delete i;
      }
    }

  enum
    {
    EngineAllocation = Engine::InterfaceCount < 1 ? 1 : Engine::InterfaceCount
    };

  EngineInterface *engines[EngineAllocation];

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

void Engine::addInterface(const InterfaceBase *i)
  {
  xForeach(EngineInterface *eng, g_engine->engines)
    {
    eng->addInterface(i);
    }
  }

void Engine::removeInterface(const InterfaceBase *i)
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

xsize Engine::getIndex(EngineInterface *in)
  {
  xsize index = 0;
  xForeach(EngineInterface *eng, g_engine->engines)
    {
    if(eng == in)
      {
      return index;
      }
    ++index;
    }

  xAssertFail();
  return X_SIZE_SENTINEL;
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
  (void)ifc;
  g_engine->currentInterface = oldIfc;
  }

}
