#include "XQObjectWrapper.h"
#include "XQtWrappers.h"
#include "QMetaProperty"
#include "XScriptValueV8Internals.h"
#include "QVarLengthArray"
#include "XFunctions.h"

#ifdef X_DEBUG
# define X_TYPE_DEBUG
#endif

class XQObjectConnectionList : public QObject
  {
public:
  XQObjectConnectionList();
  ~XQObjectConnectionList();

  struct Connection {
    Connection()
      : needsDestroy(false) {}
    Connection(const Connection &other)
      : thisObject(other.thisObject), function(other.function), needsDestroy(false) {}
    Connection &operator=(const Connection &other) {
      thisObject = other.thisObject;
      function = other.function;
      needsDestroy = other.needsDestroy;
      return *this;
      }

    v8::Persistent<v8::Object> thisObject;
    v8::Persistent<v8::Function> function;

    void dispose() {
      thisObject.Dispose();
      function.Dispose();
      }

    bool needsDestroy;
    };

  struct ConnectionList : public QList<Connection> {
    ConnectionList() : connectionsInUse(0), connectionsNeedClean(false) {}
    int connectionsInUse;
    bool connectionsNeedClean;
    };

  typedef QHash<int, ConnectionList> SlotHash;
  SlotHash slotHash;
  bool needsDestroy;
  int inUse;

  virtual void objectDestroyed(QObject *);
  virtual int qt_metacall(QMetaObject::Call, int, void **);
  };

XQObjectConnectionList::XQObjectConnectionList() : needsDestroy(false), inUse(0)
  {
  }

XQObjectConnectionList::~XQObjectConnectionList()
  {
  for(SlotHash::Iterator iter = slotHash.begin(); iter != slotHash.end(); ++iter)
    {
    QList<Connection> &connections = *iter;
    for(int ii = 0; ii < connections.count(); ++ii)
      {
      connections[ii].thisObject.Dispose();
      connections[ii].function.Dispose();
      }
    }
  slotHash.clear();
  }

void XQObjectConnectionList::objectDestroyed(QObject *object)
  {
  XQObjectWrapper::instance()->_connections.remove(object);

  if(inUse)
    {
    needsDestroy = true;
    }
  else
    {
    delete this;
    }
  }

int XQObjectConnectionList::qt_metacall(QMetaObject::Call method, int index, void **metaArgs)
  {
  if (method == QMetaObject::InvokeMetaMethod)
    {
    SlotHash::Iterator iter = slotHash.find(index);
    if (iter == slotHash.end())
      {
      return -1;
      }

    ConnectionList &connectionList = *iter;
    if (connectionList.isEmpty())
      {
      return -1;
      }

    inUse++;

    connectionList.connectionsInUse++;

    QList<Connection> connections = connectionList;

    QMetaMethod snd = sender()->metaObject()->method(senderSignalIndex());
    QList<QByteArray> types = snd.parameterTypes();

#ifndef X_DART
    v8::Locker l;
    v8::Handle<v8::Context> ctxt = getV8EngineInternal();

    v8::HandleScope handle_scope;
    v8::Context::Scope scope(ctxt);

    int argCount = types.size();
    QVarLengthArray<XScriptValue, 9> args(argCount);

    for(int ii = 0; ii < argCount; ++ii)
      {
      const QByteArray typeStr = types[ii];
      int type = QVariant::Invalid;
      if(typeStr.length())
        {
        type = QMetaType::type(typeStr.constData());
        }

      if(type == qMetaTypeId<QVariant>())
        {
        args[ii] = XScriptValue(*((QVariant *)metaArgs[ii + 1]));
        }
      else
        {
        args[ii] = XScriptValue(QVariant(type, metaArgs[ii + 1]));
        }
      }

    for(int ii = 0; ii < connections.count(); ++ii)
      {
      Connection &connection = connections[ii];
      if(connection.needsDestroy)
        {
        continue;
        }

      v8::TryCatch trycatch;

      v8::Handle<v8::Value> result;
      try
        {
        xAssert(!connection.function.IsEmpty());
        xAssert(connection.function->IsFunction());
        if(connection.thisObject.IsEmpty())
          {
          result = connection.function->Call(ctxt->Global(), argCount, getV8Internal(args.data()));
          }
        else
          {
          result = connection.function->Call(connection.thisObject, argCount, getV8Internal(args.data()));
          }

        if(result.IsEmpty())
          {
          trycatch.ReThrow();
          qCritical() << XScriptConvert::from<QString>(fromHandle(trycatch.StackTrace()));
          qCritical() << XScriptConvert::from<QString>(fromHandle(trycatch.Exception()));
          }
        }
      catch(...)
        {
        xAssertFail();
        }
      }

    connectionList.connectionsInUse--;
    if (connectionList.connectionsInUse == 0 && connectionList.connectionsNeedClean) {
      for (QList<Connection>::Iterator iter = connectionList.begin();
           iter != connectionList.end(); ) {
        if (iter->needsDestroy) {
          iter->dispose();
          iter = connectionList.erase(iter);
          } else {
          ++iter;
          }
        }
      }

    inUse--;
    if (inUse == 0 && needsDestroy)
      {
      delete this;
      }
#endif
    }

  return -1;
  }

Q_DECLARE_METATYPE(QObjectList)

XQObjectWrapper::XQObjectWrapper()
  {
  _context = 0;
  }

XQObjectWrapper::~XQObjectWrapper()
{
  _objects.remove(&QObject::staticMetaObject);
  _objects.remove(&QWidget::staticMetaObject);
  foreach(XInterfaceBase *b, _objects)
    {
    if(b != XInterface<QObject>::lookup())
      {
      delete b;
      }
    }
  _objects.clear();
  }

QString formatClassName(const QString &n)
  {
  if(n[0] == 'Q')
    {
    return n.right(n.length()-1);
    }
  return n;
  }

const char *qobjectName = "QObject";
void XQObjectWrapper::initiate(XScriptEngine *c)
  {
#ifndef X_DART
  qRegisterMetaType<XScriptObject>("XScriptObject");
  qRegisterMetaType<XScriptFunction>("XScriptFunction");

  instance()->_context = c;

  // build up custom QObject wrapper
  XInterface<QObject>* interface = XInterface<QObject>::create(qobjectName);
  interface->addNativeConvertConstructor();
  buildInterface(interface, &QObject::staticMetaObject);
  interface->seal();
  c->addInterface(interface);
  instance()->_objects.insert(&QObject::staticMetaObject, interface);


  // build up custom QWidget wrapper
  XInterface<QWidget>* widget = XInterface<QWidget>::createWithParent("QWidget", interface, interface);
  widget->addNativeConvertConstructor();

  widget->addConstMethod<QPoint (QWidget*, const QPoint&), &QWidget::mapTo>("mapTo");
  widget->addConstMethod<QPoint (const QPoint&), &QWidget::mapToGlobal>("mapToGlobal");

  buildInterface(widget, &QWidget::staticMetaObject);
  widget->seal();

  instance()->_objects.insert(&QWidget::staticMetaObject, widget);
#endif
  }

XQObjectWrapper *XQObjectWrapper::instance()
  {
  static XQObjectWrapper wrap;
  return &wrap;
  }

XScriptObject XQObjectWrapper::wrap(QObject *obj)
  {
  const XInterfaceBase* interface = findInterface(obj->metaObject());

  XScriptValue vals[1] = { XScriptValue(obj) };
  XScriptObject self = interface->newInstance(1, vals);
  xAssert(self.isValid());

  return self;
  }

XInterfaceBase *XQObjectWrapper::findInterface(const QMetaObject *object)
  {
  XInterfaceBase *base = _objects[object];
  if(base)
    {
    return base;
    }

  const QMetaObject *parent = object->superClass();
  XInterfaceBase *parentInterface = 0;
  if(parent)
    {
    parentInterface = findInterface(parent);
    }


  XInterfaceBase* qobject = ::findInterface(qMetaTypeId<QObject*>());
  base = new XInterface<QObject>(qobject->typeId(), 0, qobject->typeId(), qobject->nonPointerTypeId(), formatClassName(object->className()), parentInterface);

  _objects.insert(object, base);

  buildInterface(base, object);

  base->seal();
  _context->addInterface(base);

  return base;
  }

template<typename A, typename B, typename C, typename D, typename E>
class MaxSizeOf5
  {
  template<typename Z, typename X> struct SMax {
    static const xsize Size = sizeof(Z) > sizeof(X) ? sizeof(Z) : sizeof(X);
    };
public:

  static const size_t Size = SMax<A, SMax<B, SMax<C, SMax<D, E> > > >::Size;
  };

struct CallArgument
  {
  inline CallArgument()
    : type(QVariant::Invalid)
    {
    }
  inline ~CallArgument()
    {
    cleanup();
    }
  inline void *dataPtr()
    {
    if (type == -1)
      return qvariantPtr->data();
    else
      return (void *)&allocData;
    }

  void initAsType(int callType)
    {
    if (type != 0) { cleanup(); type = 0; }
    if (callType == 0) return;

    /*if (callType == qMetaTpeId<QJSValue>())
      {
      qjsValuPtr = new (&allocData) QJSValue();
      type = callType;
      }
    else */if (callType == QMetaType::Int ||
               callType == QMetaType::UInt ||
               callType == QMetaType::Bool ||
               callType == QMetaType::Double ||
               callType == QMetaType::Float)
      {
      type = callType;
      }
    else if (callType == QMetaType::QObjectStar)
      {
      qobjectPtr = 0;
      type = callType;
      }
    else if (callType == QMetaType::QString)
      {
      qstringPtr = new (&allocData) QString();
      type = callType;
      }
    else if (callType == QMetaType::QVariant)
      {
      type = callType;
      qvariantPtr = new (&allocData) QVariant();
      }
    else if (callType == qMetaTypeId<QList<QObject *> >())
      {
      type = callType;
      qlistPtr = new (&allocData) QList<QObject *>();
      }
    /*else if (callType == qMetaTypeId<QQmlV8Handle>())
      {
      type = callType;
      handlePtr = new (&allocData) QQmlV8Handle;
      }*/
    else
      {
      type = -1;
      qvariantPtr = new (&allocData) QVariant(callType, (void *)0);
      }
    }

  void fromValue(int callType, v8::Handle<v8::Value> value)
    {
    if (type != 0)
      {
      cleanup();
      type = 0;
      }

    /*if (callType == qMetaTypeId<QJSValue>()) {
        qjsValuePtr = new (&allocData) QJSValue(QJSValuePrivate::get(new QJSValuePrivate(engine, value)));
        type = qMetaTypeId<QJSValue>();
        } else */
    if (callType == QMetaType::Int)
      {
      intValue = quint32(value->Int32Value());
      type = callType;
      }
    else if (callType == QMetaType::UInt)
      {
      intValue = quint32(value->Uint32Value());
      type = callType;
      }
    else if (callType == QMetaType::Bool)
      {
      boolValue = value->BooleanValue();
      type = callType;
      }
    else if (callType == QMetaType::Double)
      {
      doubleValue = double(value->NumberValue());
      type = callType;
      }
    else if (callType == QMetaType::Float)
      {
      floatValue = float(value->NumberValue());
      type = callType;
      }
    else if (callType == QMetaType::QString)
      {
      if (value->IsNull() || value->IsUndefined())
        {
        qstringPtr = new (&allocData) QString();
        }
      else
        {
        qstringPtr = new (&allocData) QString(XScriptConvert::from<QString>(fromHandle(value)));
        }
      type = callType;
    }
    else if (callType == QMetaType::QObjectStar)
    {
      qobjectPtr = XScriptConvert::from<QObject>(fromHandle(value));
      type = callType;
    }
    else if (callType == QMetaType::QWidgetStar)
    {
      qobjectPtr = XScriptConvert::from<QWidget>(fromHandle(value));
      type = callType;
    }
    else if (callType == qMetaTypeId<QVariant>())
      {
      qvariantPtr = new (&allocData) QVariant(fromHandle(value).toVariant());
      type = callType;
      }
    else if (callType == qMetaTypeId<QList<QObject*> >())
      {
      qlistPtr = new (&allocData) QList<QObject *>();
      if (value->IsArray())
        {
        v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(value);
        uint32_t length = array->Length();
        for (uint32_t ii = 0; ii < length; ++ii)
          {
          qlistPtr->append(XScriptConvert::from<QObject>(fromHandle(array->Get(ii))));
          }
        }
      else
        {
        qlistPtr->append(XScriptConvert::from<QObject>(fromHandle(value)));
        }
      type = callType;
      }
    /*else if (callType == qMetaTypeId<QQmlV8Handle>())
        {
        handlePtr = new (&allocData) QQmlV8Handle(QQmlV8Handle::fromHandle(value));
        type = callType;
        }*/
    else
      {
      qvariantPtr = new (&allocData) QVariant();
      type = -1;

      QVariant v = fromHandle(value).toVariant(callType);
      if (v.userType() == callType)
        {
        *qvariantPtr = v;
        }
      else if (v.canConvert((QVariant::Type)callType))
        {
        *qvariantPtr = v;
        qvariantPtr->convert((QVariant::Type)callType);
        }
      else /*if (const QMetaObject *mo = ep ? ep->rawMetaObjectForType(callType) : 0)
          {
          QObject *obj = ep->toQObject(v);

          if (obj)
            {
            const QMetaObject *objMo = obj->metaObject();
            while (objMo && objMo != mo) objMo = objMo->superClass();
            if (!objMo) obj = 0;
            }

          *qvariantPtr = QVariant(callType, &obj);
          }
        else*/
        {
        *qvariantPtr = QVariant(callType, (void *)0);
        }
      }
    }

  v8::Handle<v8::Value> toValue()
    {
    /*if (type == qMetaTypeId<QJSValue>())
      {
      return QJSValuePrivate::get(*qjsValuePtr)->asV8Value(engine);
      }
    else */if (type == QMetaType::Int)
      {
      return v8::Integer::New(int(intValue));
      }
    else if (type == QMetaType::UInt)
      {
      return v8::Integer::NewFromUnsigned(intValue);
      }
    else if (type == QMetaType::Bool)
      {
      return v8::Boolean::New(boolValue);
      }
    else if (type == QMetaType::Double)
      {
      return v8::Number::New(doubleValue);
      }
    else if (type == QMetaType::Float)
      {
      return v8::Number::New(floatValue);
      }
    else if (type == QMetaType::QString)
      {
      return getV8Internal(XScriptConvert::to(*qstringPtr));
      }
    else if (type == QMetaType::QObjectStar)
      {
      return getV8Internal(XScriptConvert::to(qobjectPtr));
      }
    else if (type == qMetaTypeId<QList<QObject *> >())
      {
      // XXX Can this be made more by using Array as a prototype and implementing
      // directly against QList<QObject*>?
      QList<QObject *> &list = *qlistPtr;
      v8::Local<v8::Array> array = v8::Array::New(list.count());
      for (int ii = 0; ii < list.count(); ++ii)
        {
        array->Set(ii, getV8Internal(XScriptConvert::to(list.at(ii))));
        }
      return array;
      }/*
    else if(type == qMetaTypeId<QQmlV8Handle>())
      {
      return handlePtr->toHandle();
      }*/
    else if (type == -1 || type == qMetaTypeId<QVariant>())
      {
      return getV8Internal(XScriptValue(*qvariantPtr));
      }
    else
      {
      return v8::Undefined();
      }
    }

private:
  CallArgument(const CallArgument &);
  inline void cleanup()
    {
    if (type == QMetaType::QString)
      {
      qstringPtr->~QString();
      }
    else if (type == -1 || type == QMetaType::QVariant)
      {
      qvariantPtr->~QVariant();
      }/*
    else if (type == qMetaTypeId<QJSValue>())
      {
      qjsValuePtr->~QJSValue();
      }*/
    else if (type == qMetaTypeId<QList<QObject *> >())
      {
      qlistPtr->~QList<QObject *>();
      }
    }


  union {
    float floatValue;
    double doubleValue;
    quint32 intValue;
    bool boolValue;
    QObject *qobjectPtr;

    char allocData[MaxSizeOf5<QVariant,
    QString,
    QList<QObject *>,
    void *,
    void *>::Size];

    qint64 q_for_alignment;
    };

  // Pointers to allocData
  union {
    QString *qstringPtr;
    QVariant *qvariantPtr;
    QList<QObject *> *qlistPtr;
    };

  int type;
  };

struct Utils
  {
  static v8::Handle<v8::Value> signal(v8::Local<v8::String>, const v8::AccessorInfo& info)
    {
    QObject *ths = XScriptConvert::from<QObject>(fromHandle(info.This()));
    int id = XScriptConvert::from<int>(fromHandle(info.Data()));

    v8::Local<v8::String> zeroV8 = v8::String::New("0");
    v8::Local<v8::String> oneV8 = v8::String::New("1");

    v8::Local<v8::External> thsV8 = v8::External::New(ths);
    v8::Local<v8::Integer> idV8 = v8::Integer::New(id);

    v8::Local<v8::FunctionTemplate> conFn = v8::FunctionTemplate::New((v8::InvocationCallback)connect);
    conFn->Set(zeroV8, thsV8);
    conFn->Set(oneV8, idV8);

    v8::Local<v8::FunctionTemplate> disconFn = v8::FunctionTemplate::New((v8::InvocationCallback)connect);
    disconFn->Set(zeroV8, thsV8);
    disconFn->Set(oneV8, idV8);


    v8::Local<v8::FunctionTemplate> t = v8::FunctionTemplate::New((v8::InvocationCallback)emitSignal);
    t->Set("connect", conFn->GetFunction());
    t->Set("disconnect", conFn->GetFunction());

    t->Set(zeroV8, thsV8);
    t->Set(oneV8, idV8);

    return t->GetFunction();
    }

  static v8::Handle<v8::Value> read(v8::Local<v8::String>, const v8::AccessorInfo& info)
    {
    QObject *ths = XScriptConvert::from<QObject>(fromHandle(info.This()));
    if(!ths)
      {
      return v8::Null();
      }

    int id = XScriptConvert::from<int>(fromHandle(info.Data()));
    QMetaProperty prop(ths->metaObject()->property(id));

#ifdef X_TYPE_DEBUG
    xAssert(prop.isValid());
    const char *name = prop.name();
    (void)name;
#endif

    QVariant value = prop.read(ths);
    return getV8Internal(XScriptValue(value));
    }

  static void write(v8::Local<v8::String>, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
    {
    QObject *ths = XScriptConvert::from<QObject>(fromHandle(info.This()));
    if(!ths)
      {
      return;
      }

    int id = XScriptConvert::from<int>(fromHandle(info.Data()));
    ths->metaObject()->property(id).write(ths, fromHandle(value).toVariant());
    }

  static v8::Handle<v8::Value> method(const v8::Arguments& args)
    {
    QObject *ths = XScriptConvert::from<QObject>(fromHandle(args.This()));
    v8::Local<v8::Value> idVal = args.Callee()->Get(0);
    int id = XScriptConvert::from<int>(fromHandle(idVal));
    return methodCall(ths, id, args);
    }

  static v8::Handle<v8::Value> methodCall(QObject *ths, int id, const v8::Arguments& args)
    {
    QMetaMethod method = ths->metaObject()->method(id);
#ifdef X_TYPE_DEBUG
    const char *name = method.signature();
    (void)name;
#endif
    QList<QByteArray> types = method.parameterTypes();
    int length = args.Length();
    if(length < types.size())
      {
      return getV8Internal(Toss(QString("Too few arguments to method ") + method.signature()));
      }
    length = qMin(length, types.size());

    QVarLengthArray<CallArgument, 9> qargs(length + 1);

    const char *returnTypeStr = method.typeName();
    int returnType = QVariant::Invalid;
    if(returnTypeStr)
      {
      returnType = QMetaType::type(returnTypeStr);
      }

    qargs[0].initAsType(returnType);


    for (int ii = 0; ii < length; ++ii)
      {
      const QByteArray typeStr = types[ii];
      int type = QVariant::Invalid;
      if(typeStr.length())
        {
        type = QMetaType::type(typeStr.constData());
        }

      qargs[ii + 1].fromValue(type, args[ii]);
      }


    QVarLengthArray<void *, 9> argData(qargs.count());
    for (int ii = 0; ii < qargs.count(); ++ii)
      {
      argData[ii] = qargs[ii].dataPtr();
      }


    QMetaObject::metacall(ths, QMetaObject::InvokeMetaMethod, id, argData.data());


    return qargs[0].toValue();
    }

  static XScriptValue connect(XScriptArguments const &xArgs)
    {
    v8::Arguments const &args = *(v8::Arguments const *)&xArgs;
    if(args.Length() == 0)
      {
      return Toss("Function.prototype.connect: no arguments given");
      }

    v8::Handle<v8::Object> calleeThis = getV8Internal(xArgs.callee());
    QObject *object = (QObject *)calleeThis->Get(0).As<v8::External>()->Value();
    int id = calleeThis->Get(1)->Int32Value();

    if(id == -1)
      {
      return Toss("Function.prototype.connect: this object is not a signal");
      }

    if(!object)
      {
      return Toss("Function.prototype.connect: cannot connect to deleted QObject");
      }

    if(id < 0 || object->metaObject()->method(id).methodType() != QMetaMethod::Signal)
      {
      return Toss("Function.prototype.connect: this object is not a signal");
      }

    v8::Local<v8::Value> functionValue;
    v8::Local<v8::Value> functionThisValue;

    if (args.Length() == 1)
      {
      functionValue = args[0];
      }
    else
      {
      functionThisValue = args[0];
      functionValue = args[1];
      }

    if(!functionValue->IsFunction())
      {
      return Toss("Function.prototype.connect: target is not a function");
      }

    if(!functionThisValue.IsEmpty() && !functionThisValue->IsObject())
      {
      return Toss("Function.prototype.connect: target this is not an object");
      }

    XQObjectWrapper *qobjectWrapper = XQObjectWrapper::instance();
    XUnorderedMap<QObject *, XQObjectConnectionList *> &connections = qobjectWrapper->_connections;
    XUnorderedMap<QObject *, XQObjectConnectionList *>::Iterator iter = connections.find(object);
    if(iter == connections.end())
      {
      iter = connections.insert(object, new XQObjectConnectionList());
      }

    XQObjectConnectionList *connectionList = *iter;
    XQObjectConnectionList::SlotHash::Iterator slotIter = connectionList->slotHash.find(id);
    if(slotIter == connectionList->slotHash.end())
      {
      slotIter = connectionList->slotHash.insert(id, XQObjectConnectionList::ConnectionList());
      QMetaObject::connect(object, id, connectionList, id);
      }

    XQObjectConnectionList::Connection connection;
    if(!functionThisValue.IsEmpty())
      {
      connection.thisObject = v8::Persistent<v8::Object>::New(functionThisValue->ToObject());
      }
    connection.function =  v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(functionValue));

    slotIter->append(connection);

    return XScriptValue();
    }

  static XScriptValue disconnect(XScriptArguments const &xArgs)
    {
    v8::Arguments const &args = *(v8::Arguments const *)&xArgs;

    if(args.Length() == 0)
      {
      return Toss("Function.prototype.disconnect: no arguments given");
      }

    v8::Handle<v8::Object> calleeThis = getV8Internal(xArgs.callee());
    QObject *object = (QObject *)calleeThis->Get(0).As<v8::External>()->Value();
    int id = calleeThis->Get(1)->Int32Value();

    if(id == -1)
      {
      return Toss("Function.prototype.disconnect: this object is not a signal");
      }

    if(!object)
      {
      return Toss("Function.prototype.disconnect: cannot disconnect from deleted QObject");
      }

    if (id < 0 || object->metaObject()->method(id).methodType() != QMetaMethod::Signal)
      {
      return Toss("Function.prototype.disconnect: this object is not a signal");
      }

    v8::Local<v8::Value> functionValue;
    v8::Local<v8::Value> functionThisValue;

    if(args.Length() == 1)
      {
      functionValue = args[0];
      }
    else
      {
      functionThisValue = args[0];
      functionValue = args[1];
      }

    if(!functionValue->IsFunction())
      {
      return Toss("Function.prototype.disconnect: target is not a function");
      }

    if(!functionThisValue.IsEmpty() && !functionThisValue->IsObject())
      {
      return Toss("Function.prototype.disconnect: target this is not an object");
      }

    XQObjectWrapper *qobjectWrapper = XQObjectWrapper::instance();
    XUnorderedMap<QObject *, XQObjectConnectionList *> &connectionsList = qobjectWrapper->_connections;
    XUnorderedMap<QObject *, XQObjectConnectionList *>::Iterator iter = connectionsList.find(object);
    if(iter == connectionsList.end())
      {
      return XScriptValue(); // Nothing to disconnect from
      }

    XQObjectConnectionList *connectionList = *iter;
    XQObjectConnectionList::SlotHash::Iterator slotIter = connectionList->slotHash.find(id);
    if(slotIter == connectionList->slotHash.end())
      {
      return XScriptValue(); // Nothing to disconnect from
      }

    XQObjectConnectionList::ConnectionList &connections = *slotIter;

    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(functionValue);
  //    QPair<QObject *, int> functionData = ExtractQtMethod(engine, function);

  //    if (functionData.second != -1) {
  //      // This is a QObject function wrapper
  //      for (int ii = 0; ii < connections.count(); ++ii) {
  //        QV8QObjectConnectionList::Connection &connection = connections[ii];

  //        if (connection.thisObject.IsEmpty() == functionThisValue.IsEmpty() &&
  //            (connection.thisObject.IsEmpty() || connection.thisObject->StrictEquals(functionThisValue))) {

  //          QPair<QObject *, int> connectedFunctionData = ExtractQtMethod(engine, connection.function);
  //          if (connectedFunctionData == functionData) {
  //            // Match!
  //            if (connections.connectionsInUse) {
  //              connection.needsDestroy = true;
  //              connections.connectionsNeedClean = true;
  //              } else {
  //              connection.dispose();
  //              connections.removeAt(ii);
  //              }
  //            return v8::Undefined();
  //            }
  //          }
  //        }

  //      } else {
      // This is a normal JS function
      for(int ii = 0; ii < connections.count(); ++ii)
        {
        XQObjectConnectionList::Connection &connection = connections[ii];
        if(connection.function->StrictEquals(function) &&
            connection.thisObject.IsEmpty() == functionThisValue.IsEmpty() &&
            (connection.thisObject.IsEmpty() || connection.thisObject->StrictEquals(functionThisValue)))
          {
          // Match!
          if(connections.connectionsInUse)
            {
            connection.needsDestroy = true;
            connections.connectionsNeedClean = true;
            }
          else
            {
            connection.dispose();
            connections.removeAt(ii);
            }
          return XScriptValue();
          }
        }
      //}

    return XScriptValue();
    }

  static XScriptValue emitSignal(XScriptArguments const &xArgs)
    {
    v8::Handle<v8::Object> calleeThis = getV8Internal(xArgs.callee());
    QObject *object = (QObject *)calleeThis->Get(0).As<v8::External>()->Value();
    int id = calleeThis->Get(1)->Int32Value();
#ifdef X_TYPE_DEBUG
    const char* name = object->metaObject()->method(id).signature();
    (void)name;
#endif

    union
      {
      const XScriptArguments *aIn;
      const v8::Arguments *aOut;
      } conv;
    conv.aIn = &xArgs;

    return fromHandle(Utils::methodCall(object, id, *conv.aOut));
    }
  };

void XQObjectWrapper::buildInterface(XInterfaceBase *interface, const QMetaObject *metaObject)
  {
  v8::Handle<v8::ObjectTemplate> templ = getV8Internal(interface);

  for(int i = metaObject->propertyOffset(), s = metaObject->propertyCount(); i < s; ++i)
    {
    QMetaProperty prop(metaObject->property(i));
    if(!prop.isReadable())
      {
      continue;
      }

    v8::AccessorSetter setter = prop.isWritable() ? Utils::write : 0;

    v8::Handle<v8::Integer> data = v8::Integer::New(i);

    v8::Handle<v8::String> name = v8::String::New(prop.name());
    templ->SetAccessor(name, Utils::read, setter, data);
    }

  for(int i = metaObject->methodOffset(), s = metaObject->methodCount(); i < s; ++i)
    {
    QMetaMethod method(metaObject->method(i));

    if(method.methodType() != QMetaMethod::Signal)
      {
      if(method.access() != QMetaMethod::Public)
        {
        continue;
        }

      v8::Handle<v8::Integer> id = v8::Integer::New(i);
      v8::Handle<v8::FunctionTemplate> data = v8::FunctionTemplate::New(Utils::method);
      data->Set(v8::String::New("0"), id);

      QString qname = method.signature();
      int firstBrack = qname.indexOf('(');
      qname = qname.left(firstBrack);

      v8::Handle<v8::String> name = v8::String::New((xuint16*)qname.constData(), qname.length());
      templ->Set(name, data);
      }
    else
      {
      QString qname = method.signature();
      int firstBrack = qname.indexOf('(');
      qname = qname.left(firstBrack);

      v8::Handle<v8::String> name = v8::String::New((xuint16*)qname.constData(), qname.length());
      templ->SetAccessor(name, Utils::signal, 0, v8::Integer::New(i));
      }
    }
  }
