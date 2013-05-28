#ifndef XQTWRAPPERS_H
#define XQTWRAPPERS_H

#include "XScriptInterface.h"
#include "QtCore/QPointF"
#include "QtCore/QRectF"
#include "QtCore/QUrl"
#include "QtGui/QBrush"
#include "QtGui/QPen"
#include "QtCore/QLocale"
#include "QtCore/QSize"
#include "QtCore/QSizeF"
#include "QtGui/QFont"
#include "QtGui/QIcon"
#include "QtGui/QCursor"
#include "QtHelper/XQWidgets"
#include "QtGui/QPalette"
#include "QtGui/QRegion"
#include "QtCore/QFile"

namespace XScript
{
class ScriptEngine;

namespace QtWrappers
{
EKSSCRIPT_EXPORT void initiate();
}
}

X_SCRIPTABLE_TYPE_COPYABLE(QRectF)
X_SCRIPTABLE_TYPE_COPYABLE(QPointF)
X_SCRIPTABLE_TYPE_COPYABLE(QRect)
X_SCRIPTABLE_TYPE_COPYABLE(QPoint)
X_SCRIPTABLE_TYPE_COPYABLE(QUrl)
X_SCRIPTABLE_TYPE_COPYABLE(QBrush)
X_SCRIPTABLE_TYPE_COPYABLE(QPen)
X_SCRIPTABLE_TYPE_COPYABLE(QLocale)
X_SCRIPTABLE_TYPE_COPYABLE(QSize)
X_SCRIPTABLE_TYPE_COPYABLE(QSizeF)
X_SCRIPTABLE_TYPE_COPYABLE(QFont)
X_SCRIPTABLE_TYPE_COPYABLE(QIcon)
X_SCRIPTABLE_TYPE_COPYABLE(QCursor)
X_SCRIPTABLE_TYPE_COPYABLE(QSizePolicy)
X_SCRIPTABLE_TYPE_COPYABLE(QPalette)
X_SCRIPTABLE_TYPE_COPYABLE(QRegion)


X_SCRIPTABLE_ABSTRACT_TYPE(QIODevice)
X_SCRIPTABLE_TYPE_BASE_INHERITED(QFile, QIODevice)

#endif // XQTWRAPPERS_H
