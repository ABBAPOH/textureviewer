#include "texture.h"
#include "texture_p.h"

#include <gsl/gsl_util>

namespace {

using ReaderFunc = AnyColor(*)(Texture::ConstData);
using WriterFunc = void(*)(Texture::Data, const AnyColor &);

struct TextureFormatConverter
{
    TextureFormat format {TextureFormat::Invalid};
    ReaderFunc reader {nullptr};
    WriterFunc writer {nullptr};
};

using TextureFormatConverters = gsl::span<const TextureFormatConverter>;

AnyColor readA8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 1);
    return {qRgba(0, 0, 0, *reinterpret_cast<const quint8 *>(data.data()))};
}

void writeA8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 1);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = qAlpha(rgba);
}

AnyColor readL8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 1);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgb(d[0], d[0], d[0])};
}

void writeL8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 1);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = (qRed(rgba) + qGreen(rgba) + qBlue(rgba)) / 3;;
}

AnyColor readR8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 1);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgb(d[0], 0, 0)};
}

void writeR8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 1);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = qRed(rgba);
}

AnyColor readLA8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 2);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[0], d[0], d[0], d[1])};
}

void writeLA8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 2);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = (qRed(rgba) + qGreen(rgba) + qBlue(rgba)) / 3;
    d[1] = qAlpha(rgba);
}

AnyColor readR16_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 2);
    const auto d = reinterpret_cast<const quint16 *>(data.data());
    return {qRgba64(d[0], 0, 0, 0xffff)};
}

void writeR16_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 2);
    const auto rgba = color.toRgba16_Unorm();
    const auto d = reinterpret_cast<quint16 *>(data.data());
    d[0] = qRed(rgba);
}

AnyColor readRG8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 2);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[0], d[1], 0, 0xff)};
}

void writeRG8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 2);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = qRed(rgba);
    d[1] = qGreen(rgba);
}

AnyColor readRGB8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 3);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[0], d[1], d[2], 0xff)};
}

void writeRGB8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 3);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = qRed(rgba);
    d[1] = qGreen(rgba);
    d[2] = qBlue(rgba);
}

AnyColor readBGR8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 3);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[2], d[1], d[0], 0xff)};
}

void writeBGR8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 3);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[2] = qRed(rgba);
    d[1] = qGreen(rgba);
    d[0] = qBlue(rgba);
}

AnyColor readRG16_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 4);
    const auto d = reinterpret_cast<const quint16 *>(data.data());
    return {qRgba64(d[0], d[1], 0, 0xffff)};
}

void writeRG16_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 4);
    const auto rgba = color.toRgba16_Unorm();
    const auto d = reinterpret_cast<quint16 *>(data.data());
    d[0] = qRed(rgba);
    d[1] = qGreen(rgba);
}

AnyColor readRGBA8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 4);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[0], d[1], d[2], d[3])};
}

void writeRGBA8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 4);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = qRed(rgba);
    d[1] = qGreen(rgba);
    d[2] = qBlue(rgba);
    d[3] = qAlpha(rgba);
}

AnyColor readBGRA8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 4);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[2], d[1], d[0], d[3])};
}

void writeBGRA8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 4);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[2] = qRed(rgba);
    d[1] = qGreen(rgba);
    d[0] = qBlue(rgba);
    d[3] = qAlpha(rgba);
}

AnyColor readABGR8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 4);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[3], d[2], d[1], d[0])};
}

void writeABGR8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 4);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[3] = qRed(rgba);
    d[2] = qGreen(rgba);
    d[1] = qBlue(rgba);
    d[0] = qAlpha(rgba);
}

AnyColor readRGBX8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 4);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[0], d[1], d[2], 0xff)};
}

void writeRGBX8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 4);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = qRed(rgba);
    d[1] = qGreen(rgba);
    d[2] = qBlue(rgba);
    d[3] = 0xff;
}

AnyColor readBGRX8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 4);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[2], d[1], d[0], 0xff)};
}

void writeBGRX8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 4);
    const auto rgba = color.toRgba8_Unorm();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[2] = qRed(rgba);
    d[1] = qGreen(rgba);
    d[0] = qBlue(rgba);
    d[3] = 0xff;
}

AnyColor readRGBA16_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 8);
    const auto d = reinterpret_cast<const quint16 *>(data.data());
    return {qRgba64(d[0], d[1], d[2], d[3])};
}

void writeRGBA16_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 8);
    const auto rgba = color.toRgba16_Unorm();
    const auto d = reinterpret_cast<quint16 *>(data.data());
    d[0] = qRed(rgba);
    d[1] = qGreen(rgba);
    d[2] = qBlue(rgba);
    d[3] = qAlpha(rgba);
}

constexpr TextureFormatConverter converters[] = {
    { TextureFormat::Invalid },

    // 8bit
    { TextureFormat::A8_Unorm, readA8_Unorm, writeA8_Unorm },
    { TextureFormat::L8_Unorm, readL8_Unorm, writeL8_Unorm },

    { TextureFormat::R8_Snorm },
    { TextureFormat::R8_Unorm, readR8_Unorm, writeR8_Unorm },
    { TextureFormat::R8_Sint },
    { TextureFormat::R8_Uint },

    // 16 bit
    { TextureFormat::LA8_Unorm, readLA8_Unorm, writeLA8_Unorm },

    { TextureFormat::R16_Snorm },
    { TextureFormat::R16_Unorm, readR16_Unorm, writeR16_Unorm },
    { TextureFormat::R16_Sint },
    { TextureFormat::R16_Uint },
    { TextureFormat::R16_Float },

    { TextureFormat::RG8_Snorm },
    { TextureFormat::RG8_Unorm, readRG8_Unorm, writeRG8_Unorm },
    { TextureFormat::RG8_Sint },
    { TextureFormat::RG8_Uint },

    // 24bit
    { TextureFormat::RGB8_Unorm, readRGB8_Unorm, writeRGB8_Unorm },
    { TextureFormat::BGR8_Unorm, readBGR8_Unorm, writeBGR8_Unorm },

    // 32bit
    { TextureFormat::R32_Sint },
    { TextureFormat::R32_Uint },
    { TextureFormat::R32_Float },

    { TextureFormat::RG16_Snorm },
    { TextureFormat::RG16_Unorm, readRG16_Unorm, writeRG16_Unorm },
    { TextureFormat::RG16_Sint },
    { TextureFormat::RG16_Uint },
    { TextureFormat::RG16_Float },

    { TextureFormat::RGBA8_Snorm },
    { TextureFormat::RGBA8_Unorm, readRGBA8_Unorm, writeRGBA8_Unorm },
    { TextureFormat::RGBA8_Sint },
    { TextureFormat::RGBA8_Uint },
    { TextureFormat::RGBA8_Srgb },

    { TextureFormat::BGRA8_Unorm, readBGRA8_Unorm, writeBGRA8_Unorm },
    { TextureFormat::BGRA8_Srgb },
    { TextureFormat::ABGR8_Unorm, readABGR8_Unorm, writeABGR8_Unorm },
    { TextureFormat::RGBX8_Unorm, readRGBX8_Unorm, writeRGBX8_Unorm },
    { TextureFormat::BGRX8_Unorm, readBGRX8_Unorm, writeBGRX8_Unorm },
    { TextureFormat::BGRX8_Srgb },

    // 64bit
    { TextureFormat::RGBA16_Snorm },
    { TextureFormat::RGBA16_Unorm, readRGBA16_Unorm, writeRGBA16_Unorm },
    { TextureFormat::RGBA16_Sint },
    { TextureFormat::RGBA16_Uint },
    { TextureFormat::RGBA16_Float },

    { TextureFormat::RG32_Sint },
    { TextureFormat::RG32_Uint },
    { TextureFormat::RG32_Float },

    // 96bit
    { TextureFormat::RGB32_Sint },
    { TextureFormat::RGB32_Uint },
    { TextureFormat::RGB32_Float },

    // 128bit
    { TextureFormat::RGBA32_Sint },
    { TextureFormat::RGBA32_Uint },
    { TextureFormat::RGBA32_Float },

    // packed formats
    { TextureFormat::BGR565_Unorm },
    { TextureFormat::RGB565_Unorm },
    // { TextureFormat::RGBA4Unorm },
    { TextureFormat::BGRA4_Unorm },
    // { TextureFormat::RGBX4Unorm },
    { TextureFormat::BGRX4_Unorm },
    { TextureFormat::BGRA5551_Unorm },
    { TextureFormat::BGRX5551_Unorm },
    { TextureFormat::RGB332_Unorm },

    // compressed
    { TextureFormat::Bc1Rgb_Unorm },
    { TextureFormat::Bc1Rgb_Srgb },
    { TextureFormat::Bc1Rgba_Unorm },
    { TextureFormat::Bc1Rgba_Srgb },
    { TextureFormat::Bc2_Unorm },
    { TextureFormat::Bc2_Srgb },
    { TextureFormat::Bc3_Unorm },
    { TextureFormat::Bc3_Srgb },
    { TextureFormat::Bc4_Snorm },
    { TextureFormat::Bc4_Unorm },
    { TextureFormat::Bc5_Unorm },
    { TextureFormat::Bc5_Snorm },
    { TextureFormat::Bc6HUF16 },
    { TextureFormat::Bc6HSF16 },
    { TextureFormat::Bc7_Unorm },
    { TextureFormat::Bc7_Srgb },

    { TextureFormat::RXGB },
    { TextureFormat::RG_ATI2N_UNorm },
    { TextureFormat::RGB8_ETC1 },
    { TextureFormat::RGB8_ETC2 },
    { TextureFormat::RGBA8_ETC2_EAC },
    { TextureFormat::RGB8_PunchThrough_Alpha1_ETC2 },
    { TextureFormat::R11_EAC_UNorm },
    { TextureFormat::RG11_EAC_UNorm },
    { TextureFormat::R11_EAC_SNorm },
    { TextureFormat::RG11_EAC_SNorm }
};

static_assert(TextureFormatConverters(converters).size() == size_t(TextureFormat::FormatsCount),
              "Incorrect size of the converters array");

/*!
  \internal
    Formats in the table should be on the right position
*/
constexpr bool checkConvertersPositions()
{
    int position = 0;
    // Use a variable to compile with msvc. It can't build because rvalue is not constexpr
    const auto array = TextureFormatConverters(converters);
    for (const auto &format: array) {
        if (format.format != TextureFormat(position++))
            return false;
    }
    return true;
}

static_assert(checkConvertersPositions(), "Incorrect format position in converters array");

} // namespace

std::function<AnyColor(Texture::ConstData)> TextureData::getFormatReader(TextureFormat format)
{
    return gsl::at(converters, size_t(format)).reader;
}

std::function<void(Texture::Data, const AnyColor &color)> TextureData::getFormatWriter(TextureFormat format)
{
    return gsl::at(converters, size_t(format)).writer;
}
