#include "XQtWrappers.h"
#include "XScriptEngine.h"
#include "XInterface.h"

namespace XScriptConvert
{
namespace internal
{
template <> struct JSToNative<QIODevice::OpenMode>
  {
  typedef QIODevice::OpenMode ResultType;

  ResultType operator()(XScriptValue const &h) const
    {
    QString n = XScriptConvert::from<QString>(h);
    if(n == "write")
      {
      return QIODevice::WriteOnly;
      }
    else if(n == "read")
      {
      return QIODevice::ReadOnly;
      }
    return QIODevice::ReadWrite;
    }
  };
}
}


namespace XQtWrappers
{

template <typename T> void setupBindings(XInterface<T> *templ);

template <typename T> struct Binder
  {
  static void init(XScriptEngine *engine, const char *name)
    {
    XInterface<T> *templ = XInterface<T>::create(name);
    setupBindings<T>(templ);
    templ->seal();
    engine->addInterface(templ);
    }

  template <typename PARENT, typename BASE> static void initWithParent(XScriptEngine *engine, const char *name)
    {
    const XInterface<PARENT> *parent = XInterface<PARENT>::lookup();
    const XInterface<BASE> *base = XInterface<BASE>::lookup();
    XInterface<T> *templ = XInterface<T>::createWithParent(name, parent, base);

    setupBindings<T>(templ);
    templ->seal();
    engine->addInterface(templ);
    }
  };

template <typename T> void setupBindings(XInterface<T> *templ)
  {
  (void)templ;
  }

template <> void setupBindings<QPointF>(XInterface<QPointF> *templ)
  {
  templ->addDefaultConstructor();
  templ->addCopyConstructor();
  templ->addConstructor<QPointF *(float, float)>();
  templ->addProperty<qreal, qreal, &QPointF::x, &QPointF::setX>("x");
  templ->addProperty<qreal, qreal, &QPointF::y, &QPointF::setY>("y");
  }

template <> void setupBindings<QPoint>(XInterface<QPoint> *templ)
  {
  templ->addDefaultConstructor();
  templ->addCopyConstructor();
  templ->addConstructor<QPoint *(int, int)>();
  templ->addProperty<int, int, &QPoint::x, &QPoint::setX>("x");
  templ->addProperty<int, int, &QPoint::y, &QPoint::setY>("y");
  }

template <> void setupBindings<QSizeF>(XInterface<QSizeF> *templ)
  {
  templ->addDefaultConstructor();
  templ->addCopyConstructor();
  templ->addProperty<qreal, qreal, &QSizeF::width, &QSizeF::setWidth>("width");
  templ->addProperty<qreal, qreal, &QSizeF::height, &QSizeF::setHeight>("height");
  }

template <> void setupBindings<QSize>(XInterface<QSize> *templ)
  {
  templ->addDefaultConstructor();
  templ->addCopyConstructor();
  templ->addProperty<int, int, &QSize::width, &QSize::setWidth>("width");
  templ->addProperty<int, int, &QSize::height, &QSize::setHeight>("height");
  }

template <> void setupBindings<QRectF>(XInterface<QRectF> *templ)
  {
  templ->addDefaultConstructor();
  templ->addCopyConstructor();
  templ->addProperty<qreal, qreal, &QRectF::left, &QRectF::setLeft>("left");
  templ->addProperty<qreal, qreal, &QRectF::right, &QRectF::setRight>("right");
  templ->addProperty<qreal, qreal, &QRectF::top, &QRectF::setTop>("top");
  templ->addProperty<qreal, qreal, &QRectF::bottom, &QRectF::setBottom>("bottom");
  templ->addProperty<QPointF, const QPointF &, &QRectF::topLeft, &QRectF::setTopLeft>("topLeft");
  }

template <> void setupBindings<QRect>(XInterface<QRect> *templ)
  {
  templ->addDefaultConstructor();
  templ->addCopyConstructor();
  templ->addProperty<int, int, &QRect::left, &QRect::setLeft>("left");
  templ->addProperty<int, int, &QRect::right, &QRect::setRight>("right");
  templ->addProperty<int, int, &QRect::top, &QRect::setTop>("top");
  templ->addProperty<int, int, &QRect::bottom, &QRect::setBottom>("bottom");
  templ->addProperty<QPoint, const QPoint &, &QRect::topLeft, &QRect::setTopLeft>("topLeft");
  }

template <> void setupBindings<QFile>(XInterface<QFile> *templ)
  {
  templ->addDefaultConstructor();
  templ->addNativeConvertConstructor();
  templ->addConstructor<QFile *(const QString &)>("fromFilename");
  }

template <> void setupBindings<QIODevice>(XInterface<QIODevice> *templ)
  {
  templ->addMethod<bool(QIODevice::OpenMode), &QIODevice::open>("open");

  typedef XScript::MethodToInCa<QIODevice, qint64(const QByteArray &), &QIODevice::write> FunctionType;
  templ->addFunction("write", 1, FunctionType::Arity, FunctionType::Call, FunctionType::CallDart);

  templ->addMethod<void(), &QIODevice::close>("close");
  }

void initiate(XScriptEngine *eng)
  {
#define BIND(t) Binder<Q##t>::init(eng, #t)
#define BIND_WITH_PARENT(t, p, b) Binder<Q##t>::initWithParent<Q##p, Q##b>(eng, #t)

  BIND(PointF);
  BIND(RectF);
  BIND(Point);
  BIND(Rect);
  BIND(Url);
  BIND(Brush);
  BIND(Pen);
  BIND(Locale);
  BIND(SizeF);
  BIND(Size);
  BIND(Font);
  BIND(Icon);
  BIND(Cursor);
  BIND(SizePolicy);
  BIND(Palette);
  BIND(Region);

  BIND(IODevice);
  BIND_WITH_PARENT(File, IODevice, IODevice);
#undef BIND
#undef BIND_WITH_PARENT
  }
}
