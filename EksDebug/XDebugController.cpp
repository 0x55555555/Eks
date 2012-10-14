#include "XDebugController.h"
#include "QDebug"

X_IMPLEMENT_DEBUG_INTERFACE(XDebugController)

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

XDebugController::XDebugController(bool client) : XDebugInterface()
  {
  _isClient = client;
  _maxInteface = 0;
  setInterfaceID(0);

  static Reciever recv[] =
    {
    recieveFunction<Init, XDebugController, &XDebugController::onInit>(),
    recieveFunction<SetupInterface, XDebugController, &XDebugController::onSetupInterface>()
    };

  setRecievers(recv, X_ARRAY_COUNT(recv));
  }

void XDebugController::onDebuggerConnected(bool client)
  {
  if(client)
    {
    Init init;
    init.version = VERSION;
    sendData(init);
    }
  }

void XDebugController::setupInterface(XDebugInterface *ifc)
  {
  xAssert(ifc->interfaceID() == X_UINT32_SENTINEL);
  ifc->setInterfaceID(++_maxInteface);

  SetupInterface setup;
  setup.typeName = ifc->typeName();
  setup.id = ifc->interfaceID();
  sendData(setup);
  }

void XDebugController::onInit(const Init &i)
  {
  qDebug() << "Debugger Connected";

  if(i.version != VERSION)
    {
    qWarning() << "Invalid client version";
    }
  }

void XDebugController::onSetupInterface(const SetupInterface &ifcDesc)
  {
  const XDebugInterfaceType *def = XDebugManager::findInterfaceType(ifcDesc.typeName);

  if(!def)
    {
    xAssertFail();
    qCritical() << "Interface" << ifcDesc.typeName << "not registered";
    }

  XDebugInterface *ifc = def->create(_isClient);
  if(!ifc)
    {
    qCritical() << "Creation of interface" << def->type << "failed";
    }

  ifc->setInterfaceID(ifcDesc.id);
  XDebugManager::addInterfaceLookup(ifc);

  _createdInterfaces << ifc;
  }
