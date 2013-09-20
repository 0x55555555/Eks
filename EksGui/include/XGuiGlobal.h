#ifndef WIDGETLIBRARYGLOBAL_H
#define WIDGETLIBRARYGLOBAL_H

#include "XGlobal"

#if WIN32
#   if defined(EKSGUI_STATIC)
#       define EKSGUI_EXPORT extern
#   else
#       if defined(EKSGUI_BUILD)
#           define EKSGUI_EXPORT __declspec(dllexport)
#       else
#           define EKSGUI_EXPORT __declspec(dllimport)
#       endif
#   endif
#else
#   define EKSGUI_EXPORT
#endif

#endif // WIDGETLIBRARYGLOBAL_H
