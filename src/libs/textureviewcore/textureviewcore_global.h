#pragma once

#include <QtCore/qglobal.h>

#if defined(TEXTUREVIEWCORE_LIBRARY)
#  define TEXTUREVIEWCORE_EXPORT Q_DECL_EXPORT
#else
#  define TEXTUREVIEWCORE_EXPORT Q_DECL_IMPORT
#endif
