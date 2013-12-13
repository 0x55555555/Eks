#include "XDebugController.h"
#include "XCore.h"
#include "QDebug"


namespace Eks
{

X_IMPLEMENT_DEBUG_INTERFACE(DebugController)

#define VERSION 1

struct Init
  {
  enum
    {
    DebugMessageType = 1
    };
  xuint32 version;
  };

QDataStream &operator<<(QDataStream& s, const Init& i)
  {
  return s << i.version;
  }

QDataStream &operator>>(QDataStream& s, Init& i)
  {
  return s >> i.version;
  }

struct SetupInterface
  {
  enum
    {
    DebugMessageType = 2
    };
  xuint32 id;
  QString typeName;
  };

QDataStream &operator<<(QDataStream& s, const SetupInterface& i)
  {
  return s << i.id << i.typeName;
  }

QDataStream &operator>>(QDataStream& s, SetupInterface& i)
  {
  return s >> i.id >> i.typeName;
  }

DebugController::DebugController(DebugManager *m, bool client)
    : _createdInterfaces(Eks::Core::defaultAllocator())
  {
  _manager = m;
  _isClient = client;
  _maxInteface = 0;
  setInterfaceID(0);

  static Reciever recv[] =
    {
    recieveFunction<Init, DebugController, &DebugController::onInit>(),
    recieveFunction<SetupInterface, DebugController, &DebugController::onSetupInterface>()
    };

  setRecievers(recv, X_ARRAY_COUNT(recv));
  }

DebugController::~DebugController()
  {
  xForeach(DebugInterface *ifc, _createdInterfaces)
    {
    const DebugInterfaceType *def = DebugManager::findInterfaceType(ifc->typeName());

    DebugManager::unregisterInterface(ifc);
    def->destroy(ifc);
    }
  }

void DebugController::onDebuggerConnected(bool client)
  {
  if(client)
    {
    Init init;
    init.version = VERSION;
    sendData(init);
    }
  }

void DebugController::setupInterface(DebugInterface *ifc)
  {
  xAssert(ifc->interfaceID() == X_UINT32_SENTINEL);
  ifc->setInterfaceID(++_maxInteface);

  SetupInterface setup;
  setup.typeName = ifc->typeName();
  setup.id = ifc->interfaceID();
  sendData(setup);
  }

void DebugController::onInit(const Init &i)
  {
  qDebug() << "Debugger Connected";

  if(i.version != VERSION)
    {
    qWarning() << "Invalid client version";
    }
  }

void DebugController::onSetupInterface(const SetupInterface &ifcDesc)
  {
  const DebugInterfaceType *def = DebugManager::findInterfaceType(ifcDesc.typeName);

  if(!def)
    {
    xAssertFail();
    qCritical() << "Interface" << ifcDesc.typeName << "not registered";
    }

  DebugInterface *ifc = def->create(_manager, _isClient);
  if(!ifc)
    {
    qCritical() << "Creation of interface" << def->type << "failed";
    }

  ifc->setInterfaceID(ifcDesc.id);
  DebugManager::addInterfaceLookup(ifc);

  _createdInterfaces << ifc;
  }

}
