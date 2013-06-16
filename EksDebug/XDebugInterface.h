#ifndef XDEBUGINTERFACE_H
#define XDEBUGINTERFACE_H

#include "XDebugGlobal.h"
#include "XAssert"
#include "XDebugManager.h"

class QAbstractItemModel;

#define X_DEBUG_INTERFACE(cls) public: cls(DebugManager *m, bool client=true); \
  QString typeName() X_OVERRIDE { static QString n(#cls); return n; } \
  private:

#ifdef X_ENABLE_APPLICATION_DEBUGGING
# define X_REGISTER_INTERFACE(cls) \
  namespace DebugInterfaces { static Eks::DebugInterfaceRegisterer<cls> REG_##cls(#cls); }
#else
# define X_REGISTER_INTERFACE(cls)
#endif


#define X_IMPLEMENT_DEBUG_INTERFACE(cls) \
  X_REGISTER_INTERFACE(cls)

namespace Eks
{

class DebugInterfaceType
  {
public:
  QString type;
  DebugManager::CreateInterfaceFunction create;
  DebugManager::DestroyInterfaceFunction destroy;
  DebugInterfaceType *next;
  };

class EKSDEBUG_EXPORT DebugInterface
  {
XProperties:
  XProperty(xuint32, interfaceID, setInterfaceID);
  XROProperty(QObject *, dataModel);

public:
  virtual ~DebugInterface();

  virtual QString typeName() = 0;

  void onDataRecieved(QDataStream &data);

protected:
  struct Reciever
    {
    typedef void (*RecieveFunction)(xuint32 type, DebugInterface* ifc, QDataStream& dataSize);

    Reciever(xuint32 t, RecieveFunction r)
        : type(t), fn(r)
      {
      }

    xuint32 type;
    RecieveFunction fn;
    };

  class OutputTunnel
    {
  public:
    OutputTunnel(DebugInterface *ifc);
    ~OutputTunnel();

    QDataStream& stream() { return _stream; }

  private:
    QDataStream &_stream;
    };

  DebugInterface();

  void setRecievers(const Reciever*, xsize recCount);

  template <typename T> void sendData(const T &data)
    {
    xAssert(T::DebugMessageType < X_UINT8_SENTINEL);
    OutputTunnel t(this);
    t.stream() << (xuint8)T::DebugMessageType << data;
    }

  X_CONST_EXPR template <typename T,
                         typename CLS,
                         void (CLS::*FN)(const T& data)> Reciever recieveFunction()
    {
    Reciever::RecieveFunction fn = DebugInterface::recieveImpl<T, CLS, FN>;
    return Reciever(
      T::DebugMessageType,
      fn
      );
    }

  template <typename T> Eks::UniquePointer<T> createDataModel()
    {
    Eks::UniquePointer<T> res;
    res.create(Eks::Core::defaultAllocator());
    _dataModel = res.value();
    return res;
    }

private:
  template <typename T, typename CLS, void (CLS::*FN)(const T& data)>
      static void recieveImpl(xuint32 type, DebugInterface *ifc, QDataStream &data)
    {
    (void)type;

    T t;
    data >> t;

    CLS* cls = static_cast<CLS*>(ifc);

    (cls->*FN)(t);
    }

  const Reciever* _recievers;
  xsize _recieverCount;
  };

template <typename T> class DebugInterfaceRegisterer
  {
public:
  DebugInterfaceRegisterer(const char* typeName)
    {
    type.create = createFn;
    type.destroy = destroyFn;
    type.type = typeName;

    DebugManager::registerInterfaceType(&type);
    }

private:
  static DebugInterface *createFn(DebugManager *m, bool client)
    {
    return Eks::Core::defaultAllocator()->create<T>(m, client);
    }

  static void destroyFn(DebugInterface *ifc)
    {
    return Eks::Core::defaultAllocator()->destroy(static_cast<T*>(ifc));
    }

  DebugInterfaceType type;
  };

}

#endif // XDEBUGINTERFACE_H
