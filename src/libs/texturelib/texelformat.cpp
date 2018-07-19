#include "texelformat.h"

static constexpr const TexelFormat formats[] = {
    {},
    // unsigned 64bit
    {Texture::Format::RGBA_16161616, 64, 0, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt16},
    // unsigned 32bit
    {Texture::Format::RGBA_8888, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8},
    {Texture::Format::BGRA_8888, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8},
    {Texture::Format::ABGR_8888, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGBA8},
    {Texture::Format::RGBX_8888, 32, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8},
    {Texture::Format::BGRX_8888, 32, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8},
    // unsigned 24bit
    {Texture::Format::RGB_888, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGB, QOpenGLTexture::UInt8},
    {Texture::Format::BGR_888, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGR, QOpenGLTexture::UInt8},
    // packed formats
    // TODO: should use GL_RGB10_A2 instead of RGBA16_UNorm (aka GL_RGB10_A2UI), but Qt doesn't support it
    {Texture::Format::RGBA_10101002_Rev, 32, 0, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt32_RGB10A2_Rev},
    {Texture::Format::BGRA_10101002_Rev, 32, 0, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGB10A2_Rev},
    {Texture::Format::BGR_565, 16, 0, QOpenGLTexture::R5G6B5, QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5},
    {Texture::Format::RGB_565, 16, 0, QOpenGLTexture::R5G6B5, QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5_Rev},
    {Texture::Format::BGRA_4444, 16, 0, QOpenGLTexture::RGBA4, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev},
    {Texture::Format::BGRX_4444, 16, 0, QOpenGLTexture::RGBFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev},
    {Texture::Format::BGRA_5551, 16, 0, QOpenGLTexture::RGBAFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev},
    {Texture::Format::BGRX_5551, 16, 0, QOpenGLTexture::RGBFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev},
    {Texture::Format::RGB_332, 8, 0, QOpenGLTexture::RG3B2, QOpenGLTexture::RGB, QOpenGLTexture::UInt8_RG3B2},
    // compressed
    {Texture::Format::DXT1, 0, 8, QOpenGLTexture::QOpenGLTexture::RGB_DXT1},
    {Texture::Format::DXT1a, 0, 8, QOpenGLTexture::QOpenGLTexture::RGBA_DXT1},
    {Texture::Format::DXT3, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT3},
    {Texture::Format::DXT5, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT5},
    {Texture::Format::RXGB, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT5},
    {Texture::Format::RG_ATI2N_UNorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RG_ATI2N_UNorm},
    {Texture::Format::RGB8_ETC1, 0, 8, QOpenGLTexture::QOpenGLTexture::RGB8_ETC1},
    {Texture::Format::RGB8_ETC2, 0, 8, QOpenGLTexture::QOpenGLTexture::RGB8_ETC2},
    {Texture::Format::RGBA8_ETC2_EAC, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA8_ETC2_EAC},
    {Texture::Format::RGB8_PunchThrough_Alpha1_ETC2, 0, 8, QOpenGLTexture::QOpenGLTexture::RGB8_PunchThrough_Alpha1_ETC2},
    {Texture::Format::R11_EAC_UNorm, 0, 8, QOpenGLTexture::QOpenGLTexture::R11_EAC_UNorm},
    {Texture::Format::RG11_EAC_UNorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RG11_EAC_UNorm},
    {Texture::Format::R11_EAC_SNorm, 0, 8, QOpenGLTexture::QOpenGLTexture::R11_EAC_SNorm},
    {Texture::Format::RG11_EAC_SNorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RG11_EAC_SNorm},
    // floating point
    {Texture::Format::RGBA_16161616F, 64, 0, QOpenGLTexture::RGBA16F, QOpenGLTexture::RGBA, QOpenGLTexture::Float16},
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

TexelFormat::TexelFormats TexelFormat::texelFormats() noexcept
{
    return {formats};
}
