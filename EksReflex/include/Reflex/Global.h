#pragma once

#include "XGlobal"

#ifdef EKSREFLEX_BUILD
# define REFLEX_EXPORT X_DECL_EXPORT
#else
# define REFLEX_EXPORT X_DECL_IMPORT
#endif
