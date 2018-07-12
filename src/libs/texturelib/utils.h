#ifndef UTILS_H
#define UTILS_H

#include <TextureLib/Texture>
#include <QDebug>
#include <QtGui/QOpenGLTexture>
#include <memory>

namespace Utils {

std::unique_ptr<QOpenGLTexture> makeOpenGLTexture(const Texture &texture);

} // namespace Utils

#endif // UTILS_H
