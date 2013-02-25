#include "X3DCanvas.h"

#if X_QT_INTEROP

#include "XOptional"
#include "XGLRenderer.h"
#include "XD3DRenderer.h"
#include "XFramebuffer.h"

#define ALLOC Eks::GlobalAllocator::instance()

namespace Eks
{

#if X_ENABLE_GL_RENDERER

GL3DCanvas::GL3DCanvas(QWidget *parent) : QGLWidget(parent)
  {
  _buffer = ALLOC->create<ScreenFrameBuffer>();
  _renderer = GLESRenderer::createGLRenderer(_buffer, ALLOC);
  }

GL3DCanvas::~GL3DCanvas()
  {
  Eks::GLESRenderer::destroyGLRenderer(_renderer, _buffer, ALLOC);
  ALLOC->destroy(_buffer);
  _buffer = 0;
  }

void GL3DCanvas::paintGL()
  {
  paint3D(_renderer, _buffer);
  }


#endif

#if X_ENABLE_DX_RENDERER


D3D3DCanvas::D3D3DCanvas(QWidget* parent, Renderer **r)
    : QWidget(parent)
  {
  setAttribute(Qt::WA_PaintOnScreen, true);
  setAttribute(Qt::WA_NativeWindow, true);

  WId handle = winId();

  Eks::Optional<Renderer *> renderer(r);

  _buffer = ALLOC->create<ScreenFrameBuffer>();
  _renderer = renderer = Eks::D3DRenderer::createD3DRenderer((void*)handle, _buffer, ALLOC);


  }

D3D3DCanvas::~D3D3DCanvas()
  {
  Eks::D3DRenderer::destroyD3DRenderer(_renderer, _buffer, Eks::GlobalAllocator::instance());
  ALLOC->destroy(_buffer); 
  _buffer = 0;
  }

void D3D3DCanvas::update3D()
  {
  update();
  }

void D3D3DCanvas::resizeEvent(QResizeEvent* evt)
  {
  _buffer->resize(evt->size().width(), evt->size().height(), ScreenFrameBuffer::RotateNone);
  resize3D(_renderer, evt->size().width(), evt->size().height());
  }

void D3D3DCanvas::paintEvent(QPaintEvent *)
  {
  paint3D(_renderer, _buffer);

  bool deviceLost = false;
  _buffer->present(&deviceLost);
  xAssert(!deviceLost);
  }

#endif

QWidget* Canvas3D::createBest(QWidget* parent, Renderer **r)
  {
  Eks::Optional<Renderer*> ren(r);

#if X_ENABLE_DX_RENDERER
  if(QSysInfo::WindowsVersion >= QSysInfo::WV_WINDOWS8)
    {
    D3D3DCanvas *can = new D3D3DCanvas(parent, &ren);
    can->initialise3D(ren);
    return can;
    }
#endif

#if X_ENABLE_GL_RENDERER
  GL3DCanvas *can = new GL3DCanvas(parent, &ren);
  can->initialise3D(ren);
  return can;
#else
  return 0;
#endif
  }
}

#endif
