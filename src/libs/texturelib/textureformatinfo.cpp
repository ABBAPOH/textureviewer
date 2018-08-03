#include "textureformatinfo.h"

#include <QtCore/QMetaEnum>

constexpr TextureFormatInfo formats[] = {
    {},
    // 8bit
    {TextureFormat::A8Unorm, 8, 0, QOpenGLTexture::AlphaFormat, QOpenGLTexture::Alpha, QOpenGLTexture::UInt8},
    {TextureFormat::L8Unorm, 8, 0, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, QOpenGLTexture::UInt8},
    // 16bit
    {TextureFormat::LA8Unorm, 16, 0, QOpenGLTexture::LuminanceAlphaFormat, QOpenGLTexture::LuminanceAlpha, QOpenGLTexture::UInt8},
    // 24bit
    {TextureFormat::RGB8Unorm, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGB, QOpenGLTexture::UInt8},
    {TextureFormat::BGR8Unorm, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGR, QOpenGLTexture::UInt8},
    // 32bit
    {TextureFormat::RGBA8Unorm, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8},
    {TextureFormat::BGRA8Unorm, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8},
    {TextureFormat::ABGR8Unorm, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGBA8},
    {TextureFormat::RGBX8Unorm, 32, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8},
    {TextureFormat::BGRX8Unorm, 32, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8},
    // 64bit
    {TextureFormat::RGBA16Unorm, 64, 0, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt16},
    {TextureFormat::RGBA16F, 64, 0, QOpenGLTexture::RGBA16F, QOpenGLTexture::RGBA, QOpenGLTexture::Float16},
    // packed formats
    // TODO: should use GL_RGB10_A2 instead of RGBA16_UNorm (aka GL_RGB10_A2UI), but Qt doesn't support it
    {TextureFormat::RGBA_10101002_Rev, 32, 0, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt32_RGB10A2_Rev},
    {TextureFormat::BGRA_10101002_Rev, 32, 0, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGB10A2_Rev},
    {TextureFormat::BGR565Unorm, 16, 0, QOpenGLTexture::R5G6B5, QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5},
    {TextureFormat::RGB565Unorm, 16, 0, QOpenGLTexture::R5G6B5, QOpenGLTexture::RGB, QOpenGLTexture::UInt16_R5G6B5_Rev},
    {TextureFormat::BGRA4Unorm, 16, 0, QOpenGLTexture::RGBA4, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev},
    {TextureFormat::BGRX4Unorm, 16, 0, QOpenGLTexture::RGBFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev},
    {TextureFormat::BGRA5551Unorm, 16, 0, QOpenGLTexture::RGBAFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev},
    {TextureFormat::BGRX5551Unorm, 16, 0, QOpenGLTexture::RGBFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev},
    {TextureFormat::RGB332Unorm, 8, 0, QOpenGLTexture::RG3B2, QOpenGLTexture::RGB, QOpenGLTexture::UInt8_RG3B2},
    // compressed
    {TextureFormat::Bc1RgbUnorm, 0, 8, QOpenGLTexture::QOpenGLTexture::RGB_DXT1},
    {TextureFormat::Bc1RgbSrgb, 0, 8, QOpenGLTexture::QOpenGLTexture::SRGB_DXT1},
    {TextureFormat::Bc1RgbaUnorm, 0, 8, QOpenGLTexture::QOpenGLTexture::RGBA_DXT1},
    {TextureFormat::Bc1RgbaSrgb, 0, 8, QOpenGLTexture::QOpenGLTexture::SRGB_Alpha_DXT1},
    {TextureFormat::Bc2Unorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT3},
    {TextureFormat::Bc2Srgb, 0, 16, QOpenGLTexture::QOpenGLTexture::SRGB_Alpha_DXT3},
    {TextureFormat::Bc3Unorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT5},
    {TextureFormat::Bc3Srgb, 0, 16, QOpenGLTexture::QOpenGLTexture::SRGB_Alpha_DXT5},
    {TextureFormat::Bc4Snorm, 0, 8, QOpenGLTexture::QOpenGLTexture::R_ATI1N_SNorm},
    {TextureFormat::Bc4Unorm, 0, 8, QOpenGLTexture::QOpenGLTexture::R_ATI1N_UNorm},
    {TextureFormat::Bc6HUF16, 0, 16, QOpenGLTexture::QOpenGLTexture::RGB_BP_UNSIGNED_FLOAT},
    {TextureFormat::Bc7Unorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RGB_BP_UNorm},
    {TextureFormat::Bc7Srgb, 0, 16, QOpenGLTexture::QOpenGLTexture::SRGB_BP_UNorm},
    {TextureFormat::RXGB, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA_DXT5},
    {TextureFormat::RG_ATI2N_UNorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RG_ATI2N_UNorm},
    {TextureFormat::RGB8_ETC1, 0, 8, QOpenGLTexture::QOpenGLTexture::RGB8_ETC1},
    {TextureFormat::RGB8_ETC2, 0, 8, QOpenGLTexture::QOpenGLTexture::RGB8_ETC2},
    {TextureFormat::RGBA8_ETC2_EAC, 0, 16, QOpenGLTexture::QOpenGLTexture::RGBA8_ETC2_EAC},
    {TextureFormat::RGB8_PunchThrough_Alpha1_ETC2, 0, 8, QOpenGLTexture::QOpenGLTexture::RGB8_PunchThrough_Alpha1_ETC2},
    {TextureFormat::R11_EAC_UNorm, 0, 8, QOpenGLTexture::QOpenGLTexture::R11_EAC_UNorm},
    {TextureFormat::RG11_EAC_UNorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RG11_EAC_UNorm},
    {TextureFormat::R11_EAC_SNorm, 0, 8, QOpenGLTexture::QOpenGLTexture::R11_EAC_SNorm},
    {TextureFormat::RG11_EAC_SNorm, 0, 16, QOpenGLTexture::QOpenGLTexture::RG11_EAC_SNorm},
};

static_assert (sizeof (formats) == sizeof(TextureFormatInfo) * size_t(TextureFormat::FormatsCount),
               "Some TextureFormat eniumerations are not handled in an array");

/*!
  \internal
    Formats in the table should be on the right position
*/
constexpr bool checkFormatPositions()
{
    int position = 0;
    // Use a variable to compile with msvc. It can't build because rvalue is not constexpr
    const auto array = TextureFormatInfo::TextureFormatInfos(formats);
    for (const auto &format: array) {
        if (format.format() != TextureFormat(position++))
            return false;
    }
    return true;
}

static_assert (checkFormatPositions(), "Incorrect format position in formats array");

const TextureFormatInfo &TextureFormatInfo::texelFormat(TextureFormat format) noexcept
{
    return TextureFormatInfos(formats).at(size_t(format));
}

TextureFormatInfo::TextureFormatInfos TextureFormatInfo::texelFormats() noexcept
{
    return {formats};
}

#if defined(Q_OS_LINUX)
struct OGLMapping
{
    using Format = TextureFormat;

    static constexpr auto maxSize = size_t(TextureFormat::FormatsCount) * 2;

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
        return formats[size_t(TextureFormat::BC3_Unorm)];

    const auto texelFormat = TexelFormat{
            TextureFormat::Invalid, 0, 0, textureFormat, pixelFormat, pixelType};
    auto index = OGLMapping::firstIndex(texelFormat);
    while (oGLMapping.data[index] != TextureFormat::Invalid) {
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

TextureFormatInfo TextureFormatInfo::findOGLFormatLinear(
        QOpenGLTexture::TextureFormat textureFormat,
        QOpenGLTexture::PixelFormat pixelFormat,
        QOpenGLTexture::PixelType pixelType) noexcept
{
    const auto compareFormats = [=](const TextureFormatInfo &other)
    {
        return other.oglTextureFormat() == textureFormat
                && other.oglPixelFormat() == pixelFormat
                && other.oglPixelType() == pixelType;
    };

    const auto formats = TextureFormatInfos(::formats);
    const auto it = std::find_if(formats.begin(), formats.end(), compareFormats);
    if (it != formats.end())
        return *it;
    return {};
}

QString toQString(TextureFormat format)
{
    const auto &mo = TextureFormatInfo::staticMetaObject;
    const auto index = mo.indexOfEnumerator("TextureFormat");
    Q_ASSERT(index != -1);
    const auto en = mo.enumerator(index);
    return en.valueToKey(int(format));
}
