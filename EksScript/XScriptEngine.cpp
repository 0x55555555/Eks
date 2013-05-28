#include "XScriptGlobal.h"
#include "XScriptEngine.h"
#include "XScriptValue.h"
#include "XScriptInterface.h"
#include "XQObjectWrapper.h"
#include "XQtWrappers.h"
#include "QFile"
#include "QFileInfo"

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
    currentInterface = 0;

    (void)debugging;
    xsize idx = 0;
    (void)idx;

    memset(engines, 0, sizeof(engines));

#ifdef X_SCRIPT_ENGINE_ENABLE_JAVASCRIPT
    engines[idx++] = createV8Interface(debugging);
#endif

#ifdef X_SCRIPT_ENGINE_ENABLE_DART
    engines[idx++] = createDartInterface(debugging);
#endif
    }

  ~StaticEngine()
  {
    xForeach(auto &a, interfaces)
      {
      a->clear();
      }

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

  Eks::UnorderedMap<int, InterfaceBase*> interfaces;
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
    EngineScope s(eng);
    eng->addInterface(i);
    }
  }

void Engine::removeInterface(const InterfaceBase *i)
  {
  xForeach(EngineInterface *eng, g_engine->engines)
    {
    EngineScope s(eng);
    eng->removeInterface(i);
    }
  }

EngineInterface *Engine::findInterface(const QFile *f)
  {
  QFileInfo info(*f);
  return findInterface(info.suffix());
  }

EngineInterface *Engine::findInterface(const QString &extension)
  {
  xForeach(EngineInterface *eng, g_engine->engines)
    {
    if(eng->supportsExtension(extension))
      {
      return eng;
      }
    }
  return 0;
  }

Eks::UnorderedMap<int, InterfaceBase*> *Engine::internalInterfaceLookup()
  {
  xAssert(g_engine);
  return &g_engine->interfaces;
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

EngineScope::EngineScope(EngineInterface* eng) : _currentInterface(eng)
  {
  xAssert(_currentInterface);
  _oldInterface = Engine::beginScope(_currentInterface);
  }

EngineScope::~EngineScope()
  {
  Engine::endScope(_currentInterface, _oldInterface);
  }
}
