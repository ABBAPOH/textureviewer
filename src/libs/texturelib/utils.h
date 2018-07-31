#ifndef UTILS_H
#define UTILS_H

#include "texturelib_global.h"

#include <qglobal.h>

#include <memory>

QT_BEGIN_NAMESPACE
class QOpenGLTexture;
QT_END_NAMESPACE

class Texture;

namespace Utils {

std::unique_ptr<QOpenGLTexture> TEXTURELIB_EXPORT makeOpenGLTexture(const Texture &texture);

} // namespace Utils

#endif // UTILS_H
