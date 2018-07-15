#include "texelformat.h"

static const TexelFormat formats[] = {
    {},
    {Texture::Format::RGBA_8888, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8},
    {Texture::Format::BGRA_8888, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8},
    {Texture::Format::ABGR_8888, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGBA8},
    {Texture::Format::RGBX_8888, 32, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8},
    {Texture::Format::BGRX_8888, 32, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8},
    {Texture::Format::RGB_888, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGB, QOpenGLTexture::UInt8},
    {Texture::Format::BGR_888, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGR, QOpenGLTexture::UInt8},
    {Texture::Format::DXT1, 0, 8, QOpenGLTexture::QOpenGLTexture::RGBA_DXT1},
    {Texture::Format::DXT3, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT1},
    {Texture::Format::DXT5, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT1},
};

static_assert (sizeof (formats) == sizeof(TexelFormat) * size_t(Texture::Format::FormatsCount),
               "Some Texture::Format eniumerations are not handled in an array");

const TexelFormat &TexelFormat::texelFormat(Texture::Format format) noexcept
{
    return formats[int(format)];
}
