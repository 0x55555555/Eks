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
  static void initiate();
  static QObjectWrapper *instance();
  ~QObjectWrapper();

  Object wrap(QObject *);
  InterfaceBase *findInterface(const QMetaObject *object);

private:
  QObjectWrapper();

  static void buildInterface(
      InterfaceBase *interface,
      const QMetaObject *metaObject,
      FunctionDef* extraFns=0,
      xsize extraFnCount=0);

  XUnorderedMap<const QMetaObject *, InterfaceBase *> _objects;
  XUnorderedMap<QObject *, QObjectConnectionList *> _connections;
  friend class QObjectConnectionList;
  friend struct Utils;
  };

namespace Convert {
namespace internal {
template <> struct JSToNative<QObject> : JSToNativeObject<QObject> {};

template <> struct NativeToJS<QObject>
  {
  Value operator()(QObject *n) const
    {
    return QObjectWrapper::instance()->wrap(n);
    }
  Value operator()(QObject &n) const
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
}

#define X_SCRIPTABLE_QOBJECT_TYPE(type) X_SCRIPTABLE_TYPE_INHERITS(type, QObject)

X_SCRIPTABLE_QOBJECT_TYPE(QWidget)

#endif // XQOBJECTWRAPPER_H
