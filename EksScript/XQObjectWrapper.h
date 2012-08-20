#ifndef XQOBJECTWRAPPER_H
#define XQOBJECTWRAPPER_H

#include "XScriptGlobal.h"
#include "XUnorderedMap"
#include "XInterface.h"
#include "XInterfaceUtilities.h"
#include "XScriptObject.h"
#include "XScriptEngine.h"
#include "QWidget"

namespace XScript
{

class QObjectConnectionList;
class EKSSCRIPT_EXPORT QObjectWrapper
  {
public:
  static void initiate(Engine *);
  static QObjectWrapper *instance();
  ~QObjectWrapper();

  XScriptObject wrap(QObject *);
  XInterfaceBase *findInterface(const QMetaObject *object);

private:
  QObjectWrapper();

  static void buildInterface(XInterfaceBase *interface, const QMetaObject *metaObject);

  XUnorderedMap<const QMetaObject *, XInterfaceBase *> _objects;
  XUnorderedMap<QObject *, QObjectConnectionList *> _connections;
  friend class QObjectConnectionList;
  friend struct Utils;
  };
}

namespace XScriptConvert {
namespace internal {
template <> struct JSToNative<QObject> : XScriptConvert::internal::JSToNativeObject<QObject> {};

template <> struct NativeToJS<QObject>
  {
  XScriptValue operator()(QObject *n) const
    {
    return XScript::QObjectWrapper::instance()->wrap(n);
    }
  XScriptValue operator()(QObject &n) const
    {
    xAssertFail();
    return this->operator()(&n);
    }
  };
}

template <> inline QWidget *castFromBase(QObject *ptr)
  {
  return qobject_cast<QWidget*>(ptr);
  }
}

#define X_SCRIPTABLE_QOBJECT_TYPE(type) X_SCRIPTABLE_TYPE_INHERITS(type, QObject)

X_SCRIPTABLE_QOBJECT_TYPE(QWidget)

#endif // XQOBJECTWRAPPER_H
