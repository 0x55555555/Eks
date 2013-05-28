#ifndef XQOBJECTWRAPPER_H
#define XQOBJECTWRAPPER_H

#include "XScriptGlobal.h"
#include "XUnorderedMap"
#include "XScriptInterface.h"
#include "XInterfaceUtilities.h"
#include "XScriptObject.h"
#include "XScriptEngine.h"
#include "QtHelper/XQWidget"

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

  Eks::UnorderedMap<const QMetaObject *, InterfaceBase *> _objects;
  Eks::UnorderedMap<QObject *, QObjectConnectionList *> _connections;
  friend class QObjectConnectionList;
  friend struct Utils;
  };

}

#define X_SCRIPTABLE_QOBJECT_TYPE(type) X_SCRIPTABLE_TYPE_INHERITS(type, QObject)

X_SCRIPTABLE_QOBJECT_TYPE(QWidget)

#endif // XQOBJECTWRAPPER_H
