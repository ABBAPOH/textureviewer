#include "texelformat.h"

constexpr TexelFormat formats[] = {
    {},
    // 8bit
    {Texture::Format::A8Unorm, 8, 0, QOpenGLTexture::AlphaFormat, QOpenGLTexture::Alpha, QOpenGLTexture::UInt8},
    {Texture::Format::L8Unorm, 8, 0, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, QOpenGLTexture::UInt8},
    // 16bit
    {Texture::Format::LA8Unorm, 16, 0, QOpenGLTexture::LuminanceAlphaFormat, QOpenGLTexture::LuminanceAlpha, QOpenGLTexture::UInt8},
    // 24bit
    {Texture::Format::RGB8Unorm, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGB, QOpenGLTexture::UInt8},
    {Texture::Format::BGR8Unorm, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGR, QOpenGLTexture::UInt8},
    // 32bit
    {Texture::Format::RGBA8Unorm, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8},
    {Texture::Format::BGRA8Unorm, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8},
    {Texture::Format::ABGR8Unorm, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGBA8},
    {Texture::Format::RGBX8Unorm, 32, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8},
    {Texture::Format::BGRX8Unorm, 32, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8},
    // 64bit
    {Texture::Format::RGBA16Unorm, 64, 0, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt16},
    {Texture::Format::RGBA16F, 64, 0, QOpenGLTexture::RGBA16F, QOpenGLTexture::RGBA, QOpenGLTexture::Float16},
    // packed formats
    // TODO: should use GL_RGB10_A2 instead of RGBA16_UNorm (aka GL_RGB10_A2UI), but Qt doesn't support it
    {Texture::Format::RGBA_10101002_Rev, 32, 0, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt32_RGB10A2_Rev},
    {Texture::Format::BGRA_10101002_Rev, 32, 0, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGB10A2_Rev},
    {Texture::Format::BGR565Unorm, 16, 0, QOpenGLTexture::R5G6B5, QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5},
    {Texture::Format::RGB565Unorm, 16, 0, QOpenGLTexture::R5G6B5, QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5_Rev},
    {Texture::Format::BGRA4Unorm, 16, 0, QOpenGLTexture::RGBA4, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev},
    {Texture::Format::BGRX4Unorm, 16, 0, QOpenGLTexture::RGBFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev},
    {Texture::Format::BGRA5551Unorm, 16, 0, QOpenGLTexture::RGBAFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev},
    {Texture::Format::BGRX5551Unorm, 16, 0, QOpenGLTexture::RGBFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev},
    {Texture::Format::RGB332Unorm, 8, 0, QOpenGLTexture::RG3B2, QOpenGLTexture::RGB, QOpenGLTexture::UInt8_RG3B2},
    // compressed
    {Texture::Format::Bc1RgbUnorm, 0, 8, QOpenGLTexture::QOpenGLTexture::RGB_DXT1},
    {Texture::Format::Bc1RgbSrgb, 0, 8, QOpenGLTexture::QOpenGLTexture::SRGB_DXT1},
    {Texture::Format::Bc1RgbaUnorm, 0, 8, QOpenGLTexture::QOpenGLTexture::RGBA_DXT1},
    {Texture::Format::Bc1RgbaSrgb, 0, 8, QOpenGLTexture::QOpenGLTexture::SRGB_Alpha_DXT1},
    {Texture::Format::Bc2Unorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT3},
    {Texture::Format::Bc2Srgb, 0, 16, QOpenGLTexture::QOpenGLTexture::SRGB_Alpha_DXT3},
    {Texture::Format::Bc3Unorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT5},
    {Texture::Format::Bc3Srgb, 0, 16, QOpenGLTexture::QOpenGLTexture::SRGB_Alpha_DXT5},
    {Texture::Format::Bc4Snorm, 0, 8, QOpenGLTexture::QOpenGLTexture::R_ATI1N_SNorm},
    {Texture::Format::Bc4Unorm, 0, 8, QOpenGLTexture::QOpenGLTexture::R_ATI1N_UNorm},
    {Texture::Format::Bc6HUF16, 0, 16, QOpenGLTexture::QOpenGLTexture::RGB_BP_UNSIGNED_FLOAT},
    {Texture::Format::Bc7Unorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RGB_BP_UNorm},
    {Texture::Format::Bc7Srgb, 0, 16, QOpenGLTexture::QOpenGLTexture::SRGB_BP_UNorm},
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
};

static_assert (sizeof (formats) == sizeof(TexelFormat) * size_t(Texture::Format::FormatsCount),
               "Some Texture::Format eniumerations are not handled in an array");

/*!
  \internal
    Formats in the table should be on the right position
*/
constexpr bool checkFormatPositions()
{
    int position = 0;
    // Use a variable to compile with msvc. It can't build because rvalue is not constexpr
    const auto array = TexelFormat::TexelFormats(formats);
    for (const auto &format: array) {
        if (format.format() != Texture::Format(position++))
            return false;
    }
    return true;
}

static_assert (checkFormatPositions(), "Incorrect format position in formats array");

const TexelFormat &TexelFormat::texelFormat(Texture::Format format) noexcept
{
    return TexelFormats(formats).at(size_t(format));
}

TexelFormat::TexelFormats TexelFormat::texelFormats() noexcept
{
    return {formats};
}

#if defined(Q_OS_LINUX)
struct OGLMapping
{
    using Format = Texture::Format;

    static constexpr auto maxSize = size_t(Texture::Format::FormatsCount) * 2;

    static constexpr uint getHash(const TexelFormat &format)
    {
        return qHash(format.oglTextureFormat())
                ^ qHash(format.oglPixelFormat())
                ^ qHash(format.oglPixelType());
    }

    static constexpr size_t firstIndex(const TexelFormat &format)
    {
        return getHash(format) % maxSize;
    }

    static constexpr size_t nextIndex(size_t index) { return (index + 3) % maxSize; }

    constexpr OGLMapping()
        : data{}
    {
        for (auto &item: gsl::span<Format>(data))
            item = Format::Invalid;

        for (const auto &format: gsl::span<const TexelFormat>(formats)) {
            auto index = firstIndex(format);
            while (data[index] != Format::Invalid) {
                index = nextIndex(index);
            }
            data[index] = format.format();
        }
    }

    Format data[maxSize];
};

constexpr const OGLMapping oGLMapping;

/*!
    \internal

    Searches for the requested type using stupid constexpr hash.
    However, according to the benchmark this is almost 2 times faster than the linear search
    even for 35 formats.
*/
TexelFormat TexelFormat::findOGLFormatConst(
        QOpenGLTexture::TextureFormat textureFormat,
        QOpenGLTexture::PixelFormat pixelFormat,
        QOpenGLTexture::PixelType pixelType) noexcept
{
    if (textureFormat == QOpenGLTexture::RGBA_DXT5) // special check for DXT5/RXGB formats
        return formats[size_t(Texture::Format::BC3_Unorm)];

    const auto texelFormat = TexelFormat{
            Texture::Format::Invalid, 0, 0, textureFormat, pixelFormat, pixelType};
    auto index = OGLMapping::firstIndex(texelFormat);
    while (oGLMapping.data[index] != Texture::Format::Invalid) {
        const auto &format = formats[size_t(oGLMapping.data[index])];
        if (format.oglTextureFormat() == textureFormat
                && format.oglPixelFormat() == pixelFormat
                && format.oglPixelType() == pixelType) {
            return format;
        }
        index = OGLMapping::nextIndex(index);
    }
    return TexelFormat();
}
#endif // Q_OS_LINUX

TexelFormat TexelFormat::findOGLFormatLinear(
        QOpenGLTexture::TextureFormat textureFormat,
        QOpenGLTexture::PixelFormat pixelFormat,
        QOpenGLTexture::PixelType pixelType) noexcept
{
    const auto compareFormats = [=](const TexelFormat &other)
    {
        return other.oglTextureFormat() == textureFormat
                && other.oglPixelFormat() == pixelFormat
                && other.oglPixelType() == pixelType;
    };

    const auto formats = TexelFormats(::formats);
    const auto it = std::find_if(formats.begin(), formats.end(), compareFormats);
    if (it != formats.end())
        return *it;
    return {};
}
