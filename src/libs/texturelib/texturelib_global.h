#pragma once

#include <QtCore/qglobal.h>

#if defined(TEXTURELIB_LIBRARY)
#  define TEXTURELIB_EXPORT Q_DECL_EXPORT
#else
#  define TEXTURELIB_EXPORT Q_DECL_IMPORT
#endif
