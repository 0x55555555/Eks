#ifndef XABSTRACTSHADER_H
#define XABSTRACTSHADER_H

#include "X3DGlobal.h"
#include "XPrivateImpl"

namespace Eks
{

class Shader;
class Renderer;
class Resource;

class ShaderVertexLayoutDescription
  {
public:
  enum Format
    {
    FormatFloat1,
    FormatFloat2,
    FormatFloat3,
    FormatFloat4,

    FormatCount
    };

  struct Slot
    {
  public:
    enum Type
      {
      PerVertex,
      PerInstance
      };
    Slot() : index(0), type(PerVertex), instanceDataStepRate(0)
      {
      }

    xuint8 index;
    Type type;
    xsize instanceDataStepRate;
    };

  enum OffsetPack
    {
    OffsetPackTight = X_UINT32_SENTINEL
    };

  enum Semantic
    {
    Position,
    Colour,
    TextureCoordinate,
    Normal,

    SemanticCount
    };

  ShaderVertexLayoutDescription(Semantic s, Format fmt, xsize off=OffsetPackTight, Slot sl=Slot())
    : semantic(s), format(fmt), offset(off), slot(sl)
    {
    }

  Semantic semantic;
  Format format;
  xsize offset;
  Slot slot;
  };

class EKS3D_EXPORT ShaderVertexLayout : public PrivateImpl<sizeof(void*)>
  {
public:
  typedef ShaderVertexLayoutDescription Description;
  ShaderVertexLayout() : _renderer(0) { }
  ~ShaderVertexLayout();

private:
  X_DISABLE_COPY(ShaderVertexLayout);
  friend class ShaderVertexComponent;

  Renderer *_renderer;
  };

class EKS3D_EXPORT ShaderConstantData : public PrivateImpl<sizeof(void*)>
  {
public:
  ShaderConstantData(Renderer *r=0, xsize sizeOfData=0, void *data=0);
  ~ShaderConstantData();

  static bool delayedCreate(ShaderConstantData &ths,
                            Renderer *r,
                            xsize size,
                            void *data = 0);

  void update(void *data);

private:
  X_DISABLE_COPY(ShaderConstantData);

  Renderer *_renderer;
  };

class EKS3D_EXPORT ShaderVertexComponent : public PrivateImpl<sizeof(void*)>
  {
public:
  typedef ShaderVertexLayout VertexLayout;

  ShaderVertexComponent(Renderer *r=0,
                         const char *source=0,
                         xsize length=0,
                         const VertexLayout::Description *vertexDescriptions=0,
                         xsize vertexItemCount=0,
                         VertexLayout *layout=0);
  ~ShaderVertexComponent();

  static bool delayedCreate(ShaderVertexComponent &ths,
                            Renderer *r,
                            const char *source,
                            xsize length,
                            const VertexLayout::Description *vertexDescription=0,
                            xsize vertexItemCount=0,
                            VertexLayout *layout=0);


private:
  X_DISABLE_COPY(ShaderVertexComponent);

  Renderer *_renderer;
  };

class EKS3D_EXPORT ShaderFragmentComponent : public PrivateImpl<sizeof(void*)>
  {
public:
  ShaderFragmentComponent(Renderer *r=0,
                           const char *source=0,
                           xsize length=0);
  ~ShaderFragmentComponent();

  static bool delayedCreate(ShaderFragmentComponent &ths,
                            Renderer *r,
                            const char *source,
                            xsize length);

private:
  X_DISABLE_COPY(ShaderFragmentComponent);

  Renderer *_renderer;
  };

class EKS3D_EXPORT Shader : public PrivateImpl<sizeof(void*)*2>
  {
public:
  typedef ShaderConstantData ConstantData;

  Shader(Renderer *r=0,
          ShaderVertexComponent *v=0,
          ShaderFragmentComponent *f=0);
  ~Shader();

  static bool delayedCreate(Shader &ths,
              Renderer *r,
              ShaderVertexComponent *v,
              ShaderFragmentComponent *f);


  void setShaderConstantData(xsize first, const ConstantData *data);
  void setShaderConstantDatas(xsize first, xsize num, const ConstantData *const* data);

  void setShaderResource(xsize first, const Resource *data);
  void setShaderResources(xsize first, xsize num, const Resource *const* data);

private:
  X_DISABLE_COPY(Shader);

  Renderer *_renderer;
  };

}

#include "XRenderer.h"

namespace Eks
{

inline void ShaderConstantData::update(void *data)
  {
  xAssert(_renderer);
  _renderer->functions().set.shaderConstantData(_renderer, this, data);
  }

inline void Shader::setShaderConstantData(xsize first, const ConstantData *data)
  {
  xAssert(_renderer);
  _renderer->functions().set.shaderConstantBuffer(_renderer, this, first, 1, &data);
  }

inline void Shader::setShaderConstantDatas(xsize first, xsize num, const ConstantData *const* data)
  {
  xAssert(_renderer);
  _renderer->functions().set.shaderConstantBuffer(_renderer, this, first, num, data);
  }

inline void Shader::setShaderResource(xsize first, const Resource *data)
  {
  xAssert(_renderer);
  _renderer->functions().set.shaderResource(_renderer, this, first, 1, &data);
  }

inline void Shader::setShaderResources(xsize first, xsize num, const Resource *const* data)
  {
  xAssert(_renderer);
  _renderer->functions().set.shaderResource(_renderer, this, first, num, data);
  }

}

#endif // XABSTRACTSHADER_H
