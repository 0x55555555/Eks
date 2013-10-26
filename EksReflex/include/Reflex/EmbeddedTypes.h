#pragma once

#include "Reflex/Global.h"
#include "Reflex/Type.h"

namespace Eks
{
namespace Reflex
{
namespace detail
{

enum EmbeddedType
  {
  embedded_void,
  embedded_bool,
  embedded_int,
  embedded_float,
  embedded_double,

  EmbeddedTypeCount
  };

REFLEX_EXPORT const Type *findEmbeddedType(EmbeddedType t);

#define EMBEDDED_TYPE(type) \
  template <> struct TypeResolver<type> { \
  static const Type *find() { return findEmbeddedType(embedded_##type); } };

EMBEDDED_TYPE(void)
EMBEDDED_TYPE(bool)
EMBEDDED_TYPE(int)
EMBEDDED_TYPE(float)
EMBEDDED_TYPE(double)

#undef EMBEDDED_TYPE

}
}
}
