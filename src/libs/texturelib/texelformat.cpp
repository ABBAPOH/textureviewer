#include "texelformat.h"

static constexpr const TexelFormat formats[] = {
    {},
    // unsigned 32bit
    {Texture::Format::RGBA_8888, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8},
    {Texture::Format::BGRA_8888, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8},
    {Texture::Format::ABGR_8888, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGBA8},
    {Texture::Format::RGBX_8888, 32, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8},
    {Texture::Format::BGRX_8888, 32, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8},
    // unsigned 24bit
    {Texture::Format::RGB_888, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGB, QOpenGLTexture::UInt8},
    {Texture::Format::BGR_888, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGR, QOpenGLTexture::UInt8},
    // unsigned 16bit
    {Texture::Format::RGB_565, 16, 0, QOpenGLTexture::R5G6B5, QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5},
    {Texture::Format::BGRX_4444, 16, 0, QOpenGLTexture::RGBFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev},
    {Texture::Format::BGRX_5551, 16, 0, QOpenGLTexture::RGBFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev},
    // compressed
    {Texture::Format::DXT1, 0, 8, QOpenGLTexture::QOpenGLTexture::RGBA_DXT1},
    {Texture::Format::DXT3, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT3},
    {Texture::Format::DXT5, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT5},
};

static_assert (sizeof (formats) == sizeof(TexelFormat) * size_t(Texture::Format::FormatsCount),
               "Some Texture::Format eniumerations are not handled in an array");

constexpr bool checkFormats()
{
    for (int i = 0; i < int(Texture::Format::FormatsCount); ++i) {
        if (formats[i].format() != Texture::Format(i))
            return false;
    }
    return true;
}

static_assert (checkFormats(), "Corrupted formats array");

const TexelFormat &TexelFormat::texelFormat(Texture::Format format) noexcept
{
    return formats[int(format)];
}
