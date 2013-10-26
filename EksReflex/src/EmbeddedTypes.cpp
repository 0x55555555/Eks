#include "Reflex/EmbeddedTypes.h"

namespace Eks
{

namespace Reflex
{

namespace detail
{

template <typename T> Type buildEmbeddedType(const char*)
  {
  return Type();
  }

const Type *findEmbeddedType(EmbeddedType t)
  {
#define TYPE(t) buildEmbeddedType<t>(#t)
  static const Type embedded[EmbeddedTypeCount] =
  {
    TYPE(void),
    TYPE(bool),
    TYPE(int),
    TYPE(float),
    TYPE(double),
  };

  return &(embedded[t]);
#undef TYPE
  }

}

}

}
