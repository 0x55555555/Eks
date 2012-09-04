#include "XQtWrappers.h"
#include "XScriptEngine.h"
#include "XInterface.h"
#include "XScriptConstructors.h"

namespace XScript
{
namespace Convert
{
namespace internal
{
template <> struct JSToNative<QIODevice::OpenMode>
  {
  typedef QIODevice::OpenMode ResultType;

  ResultType operator()(Value const &h) const
    {
    QString n = Convert::from<QString>(h);
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

namespace QtWrappers
{

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
  typedef QPointF CLASS;

  static ClassDef<3,2,0> cls = {
    {
      XScriptDefaultConstructor,
      XScriptCopyConstructor,
      XScriptConstructor("", float, float),
    },
    {
      XScriptPropertyDef(qreal, "x", x, setX),
      XScriptPropertyDef(qreal, "y", y, setY),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QPoint>(Interface<QPoint> *templ)
  {
  typedef QPoint CLASS;

  static ClassDef<3,2,0> cls = {
    {
      XScriptDefaultConstructor,
      XScriptCopyConstructor,
      XScriptConstructor("", int, int),
    },
    {
      XScriptPropertyDef(int, "x", x, setX),
      XScriptPropertyDef(int, "y", y, setY),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QSizeF>(Interface<QSizeF> *templ)
  {
  typedef QSizeF CLASS;

  static ClassDef<2,2,0> cls = {
    {
      XScriptDefaultConstructor,
      XScriptCopyConstructor,
    },
    {
      XScriptPropertyDef(qreal, "width", width, setWidth),
      XScriptPropertyDef(qreal, "height", height, setHeight),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QSize>(Interface<QSize> *templ)
  {
  typedef QSize CLASS;

  static ClassDef<2,2,0> cls = {
    {
      XScriptDefaultConstructor,
      XScriptCopyConstructor,
    },
    {
      XScriptPropertyDef(int, "width", width, setWidth),
      XScriptPropertyDef(int, "height", height, setHeight),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QRectF>(Interface<QRectF> *templ)
  {
  typedef QRectF CLASS;

  static ClassDef<2,5,0> cls = {
    {
      XScriptDefaultConstructor,
      XScriptCopyConstructor,
    },
    {
      XScriptPropertyDef(qreal, "width", left, setLeft),
      XScriptPropertyDef(qreal, "right", right, setRight),
      XScriptPropertyDef(qreal, "top", top, setTop),
      XScriptPropertyDef(qreal, "bottom", bottom, setBottom),
      XScriptPropertyDefExplicit(const QPointF &, QPointF, "topLeft", topLeft, setTopLeft),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QRect>(Interface<QRect> *templ)
  {
  typedef QRect CLASS;

  static ClassDef<2,5,0> cls = {
    {
      XScriptDefaultConstructor,
      XScriptCopyConstructor,
    },
    {
      XScriptPropertyDef(int, "width", left, setLeft),
      XScriptPropertyDef(int, "right", right, setRight),
      XScriptPropertyDef(int, "top", top, setTop),
      XScriptPropertyDef(int, "bottom", bottom, setBottom),
      XScriptPropertyDefExplicit(const QPoint &, QPoint, "topLeft", topLeft, setTopLeft),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QFile>(Interface<QFile> *templ)
  {
  typedef QFile CLASS;

  static ClassDef<3,0,0> cls = {
    {
      XScriptDefaultConstructor,
      XScriptConstructor("fromFilename", const QString &),
    }
  };

  templ->buildInterface(cls);
  }

template <> void setupBindings<QIODevice>(Interface<QIODevice> *templ)
  {
  typedef QIODevice CLASS;

  static ClassDef<0,0,3> cls = {
    {
      XScriptMethod("open", bool(QIODevice::OpenMode), open),
      XScriptMethod("write", qint64(const QByteArray &), write),
      XScriptMethod("close", void(), close)
    }
  };

  templ->buildInterface(cls);
  }

void initiate()
  {
#define BIND(t) Binder<Q##t>::init(#t)
#define BIND_WITH_PARENT(t, p, b) Binder<Q##t>::initWithParent<Q##p, Q##b>(#t)

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
}
