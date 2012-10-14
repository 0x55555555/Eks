#ifndef XDEBUGINTERFACE_H
#define XDEBUGINTERFACE_H

#include "XDebugGlobal.h"
#include "XAssert"
#include "XDebugManager.h"

#define X_DEBUG_INTERFACE(cls) public: cls(bool client=true); \
  QString typeName() X_OVERRIDE { static QString n(#cls); return n; } \
  private:

#ifdef X_ENABLE_APPLICATION_DEBUGGING
# define X_REGISTER_INTERFACE(cls) \
  namespace XDebugInterfaces { static XDebugInterfaceRegisterer<cls> REG_##cls(#cls); }
#else
# define X_REGISTER_INTERFACE(cls)
#endif


#define X_IMPLEMENT_DEBUG_INTERFACE(cls) \
  X_REGISTER_INTERFACE(cls)

class XDebugInterfaceType
  {
public:
  QString type;
  XDebugManager::CreateInterfaceFunction create;
  XDebugInterfaceType *next;
  };

class EKSDEBUG_EXPORT XDebugInterface
  {
XProperties:
  XProperty(xuint32, interfaceID, setInterfaceID);

public:
  ~XDebugInterface();

  virtual QString typeName() = 0;

  void onDataRecieved(QDataStream &data);

protected:
  struct Reciever
    {
    typedef void (*RecieveFunction)(xuint32 type, XDebugInterface* ifc, QDataStream& dataSize);

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
    OutputTunnel(XDebugInterface *ifc);
    ~OutputTunnel();

    QDataStream& stream() { return _stream; }

  private:
    QDataStream &_stream;
    };

  XDebugInterface();

  void setRecievers(const Reciever*, xsize recCount);

  template <typename T> void sendData(const T &data)
    {
    OutputTunnel t(this);
    t.stream() << (xuint8)T::DebugMessageType << data;
    }

  X_CONST_EXPR template <typename T,
                         typename CLS,
                         void (CLS::*FN)(const T& data)> Reciever recieveFunction()
    {
    Reciever::RecieveFunction fn = XDebugInterface::recieveImpl<T, CLS, FN>;
    return Reciever(
      T::DebugMessageType,
      fn
      );
    }

private:
  template <typename T, typename CLS, void (CLS::*FN)(const T& data)>
      static void recieveImpl(xuint32 type, XDebugInterface *ifc, QDataStream &data)
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

template <typename T> class XDebugInterfaceRegisterer
  {
public:
  XDebugInterfaceRegisterer(const char* typeName)
    {
    type.create = createFn;
    type.type = typeName;

    XDebugManager::registerInterfaceType(&type);
    }

private:
  static XDebugInterface *createFn(bool client)
    {
    return new T(client);
    }

  XDebugInterfaceType type;
  };

#endif // XDEBUGINTERFACE_H
