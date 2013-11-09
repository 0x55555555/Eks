#ifndef WIDGETLIBRARYGLOBAL_H
#define WIDGETLIBRARYGLOBAL_H

#include "XGlobal"

#if defined(EKSGUI_STATIC)
# define EKSGUI_EXPORT extern
#else
# if defined(EKSGUI_BUILD)
#  define EKSGUI_EXPORT X_DECL_EXPORT
# else
#  define EKSGUI_EXPORT X_DECL_IMPORT
# endif
#endif

#endif // WIDGETLIBRARYGLOBAL_H
