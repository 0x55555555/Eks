#include "XQtWrappers.h"
#include "XScriptEngine.h"
#include "XScriptInterface.h"
#include "XScriptFunctionWrapper.h"

namespace XScript
{
/*namespace Convert
{

namespace internal
{
template <> struct JSToNative<QIODevice::OpenMode>
  {
  typedef QIODevice::OpenMode ResultType;

  ResultType operator()(Value const &h) const
    {
    Eks::String n = Convert::from<Eks::String>(h);
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
*/

namespace QtWrappers
{
/*
template <typename T> void setupBindings(Interface<T> *templ);

template <typename T> struct Binder
  {
  static void init(const char *name)
    {
    Interface<T> *templ = Interface<T>::create(name);
    setupBindings<T>(templ);
    templ->seal();
    Engine::addInterface(templ);
    }

  template <typename PARENT, typename BASE> static void initWithParent(const char *name)
    {
    const Interface<PARENT> *parent = Interface<PARENT>::lookup();
    const Interface<BASE> *base = Interface<BASE>::lookup();
    Interface<T> *templ = Interface<T>::createWithParent(name, parent, base);

    setupBindings<T>(templ);
    templ->seal();
    Engine::addInterface(templ);
    }
  };

template <typename T> void setupBindings(Interface<T> *templ)
  {
  (void)templ;
  }

template <> void setupBindings<QPointF>(Interface<QPointF> *templ)
  {
  static ClassDef<3,2,0> cls = {
    {
      templ->defaultConstructor(),
      templ->copyConstructor(),
      templ->constructor<QPointF*(float, float)>(),
    },
    {
      templ->property<qreal, &QPointF::x, &QPointF::setX>("x"),
      templ->property<qreal, &QPointF::y, &QPointF::setY>("y"),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QPoint>(Interface<QPoint> *templ)
  {
  static ClassDef<3,2,0> cls = {
    {
      templ->defaultConstructor(),
      templ->copyConstructor(),
      templ->constructor<QPoint*(int, int)>(),
    },
    {
      templ->property<int, &QPoint::x, &QPoint::setX>("x"),
      templ->property<int, &QPoint::y, &QPoint::setY>("y"),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QSizeF>(Interface<QSizeF> *templ)
  {
  static ClassDef<2,2,0> cls = {
    {
      templ->defaultConstructor(),
      templ->copyConstructor(),
    },
    {
      templ->property<qreal, &QSizeF::width, &QSizeF::setWidth>("width"),
      templ->property<qreal, &QSizeF::height, &QSizeF::setHeight>("height"),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QSize>(Interface<QSize> *templ)
  {
  static ClassDef<2,2,0> cls = {
    {
      templ->defaultConstructor(),
      templ->copyConstructor(),
    },
    {
      templ->property<int, &QSize::width, &QSize::setWidth>("width"),
      templ->property<int, &QSize::height, &QSize::setHeight>("height"),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QRectF>(Interface<QRectF> *templ)
  {
  static ClassDef<2,5,0> cls = {
    {
      templ->defaultConstructor(),
      templ->copyConstructor(),
    },
    {
      templ->property<qreal, &QRectF::left, &QRectF::setLeft>("left"),
      templ->property<qreal, &QRectF::right, &QRectF::setRight>("right"),
      templ->property<qreal, &QRectF::top, &QRectF::setTop>("top"),
      templ->property<qreal, &QRectF::bottom, &QRectF::setBottom>("bottom"),
      templ->property<QPointF, const QPointF &, &QRectF::topLeft, &QRectF::setTopLeft>("topLeft"),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QRect>(Interface<QRect> *templ)
  {
  static ClassDef<2,5,0> cls = {
    {
      templ->defaultConstructor(),
      templ->copyConstructor(),
    },
    {
      templ->property<int, &QRect::left, &QRect::setLeft>("left"),
      templ->property<int, &QRect::right, &QRect::setRight>("right"),
      templ->property<int, &QRect::top, &QRect::setTop>("top"),
      templ->property<int, &QRect::bottom, &QRect::setBottom>("bottom"),
      templ->property<QPoint, const QPoint &, &QRect::topLeft, &QRect::setTopLeft>("topLeft"),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QFile>(Interface<QFile> *templ)
  {
  static ClassDef<2,0,0> cls = {
    {
      templ->defaultConstructor(),
      templ->constructor<QFile*(const QString&)>("fromFilename"),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QIODevice>(Interface<QIODevice> *templ)
  {
  typedef MethodToInCa<QIODevice, qint64(const QByteArray &), &QIODevice::write> WriteType;

  static ClassDef<0,0,3> cls = {
    {
    templ->method<bool(QIODevice::OpenMode), &QIODevice::open>("open"),
    InterfaceBase::method<WriteType>("write"),
    templ->method<void(), &QIODevice::close>("close")
    }
  };

  templ->buildInterface(cls);
  }
*/

void initiate()
  {
#define BIND(t) Binder<Q##t>::init(#t)
#define BIND_WITH_PARENT(t, p, b) Binder<Q##t>::initWithParent<Q##p, Q##b>(#t)

  /*BIND(PointF);
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
  BIND_WITH_PARENT(File, IODevice, IODevice);*/
#undef BIND
#undef BIND_WITH_PARENT
  }


}
}
