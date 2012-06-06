#ifndef XQTWRAPPERS_H
#define XQTWRAPPERS_H

#include "XInterface.h"
#include "QPointF"
#include "QRectF"
#include "QUrl"
#include "QBrush"
#include "QPen"
#include "QLocale"
#include "QSize"
#include "QSizeF"
#include "QFont"
#include "QIcon"
#include "QCursor"
#include "QSizePolicy"
#include "QPalette"
#include "QRegion"
#include "QFile"

class XScriptEngine;

namespace XQtWrappers
{
EKSSCRIPT_EXPORT void initiate(XScriptEngine *eng);
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

namespace XScriptConvert
{
namespace internal
{
template <> struct NativeToJS<QFile> : public XScript::NativeToJSConvertableTypeInherited<QFile, QIODevice> {};
}
}

#endif // XQTWRAPPERS_H
