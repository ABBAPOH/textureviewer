#pragma once

#include <QtCore/qglobal.h>

#if defined(TEXTUREVIEWWIDGETS_LIBRARY)
#  define TEXTUREVIEWWIDGETS_EXPORT Q_DECL_EXPORT
#else
#  define TEXTUREVIEWWIDGETS_EXPORT Q_DECL_IMPORT
#endif
