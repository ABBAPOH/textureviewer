#ifndef UTILS_H
#define UTILS_H

#include <TextureLib/Texture>
#include <QOpenGLFunctions>

namespace Utils {

template<class OpenGLFunctions>
std::pair<GLenum, GLuint> bindTexture(OpenGLFunctions *funcs, const Texture &texture)
{
    GLuint result = 0;
    funcs->glGenTextures(1, &result);
    //    funcs->glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    switch (texture.type()) {
    case Texture::Type::Texture1D:
        funcs->glBindTexture(GL_TEXTURE_1D, result);
        for (int level = 0; level < texture.levels(); ++level) {
            funcs->glTexImage1D(GL_TEXTURE_1D, level, GL_RGB, texture.width(level), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data(level));
        }
        funcs->glBindTexture(GL_TEXTURE_1D, 0);
        return {GL_TEXTURE_1D, result};
    case Texture::Type::Texture1DArray:
        funcs->glBindTexture(GL_TEXTURE_1D_ARRAY, result);
        for (int level = 0; level < texture.levels(); ++level) {
            funcs->glTexImage2D(GL_TEXTURE_1D_ARRAY, level, GL_RGB, texture.width(level), texture.layers(), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data(level));
        }
        funcs->glBindTexture(GL_TEXTURE_1D_ARRAY, 0);
        return {GL_TEXTURE_1D_ARRAY, result};
    case Texture::Type::Texture2D:
        funcs->glBindTexture(GL_TEXTURE_2D, result);
        for (int level = 0; level < texture.levels(); ++level) {
            funcs->glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, texture.width(level), texture.height(level), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data(level));
        }
        if (texture.levels() == 1)
            funcs->glGenerateMipmap(GL_TEXTURE_2D);
        funcs->glBindTexture(GL_TEXTURE_2D, 0);
        return {GL_TEXTURE_2D, result};
    case Texture::Type::Texture2DArray:
        funcs->glBindTexture(GL_TEXTURE_2D_ARRAY, result);
        for (int level = 0; level < texture.levels(); ++level) {
            funcs->glTexImage3D(GL_TEXTURE_2D_ARRAY, level, GL_RGB, texture.width(level), texture.height(level), texture.layers(), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data(level));
        }
        funcs->glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        return {GL_TEXTURE_2D_ARRAY, result};
    case Texture::Type::Texture3D:
        funcs->glBindTexture(GL_TEXTURE_3D, result);
        for (int level = 0; level < texture.levels(); ++level) {
            funcs->glTexImage3D(GL_TEXTURE_3D, level, GL_RGB, texture.width(level), texture.height(level), texture.depth(level), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data(level));
        }
        funcs->glBindTexture(GL_TEXTURE_3D, 0);
        return {GL_TEXTURE_3D, result};
    default:
        break;
    }
}

} // namespace Utils

#endif // UTILS_H
