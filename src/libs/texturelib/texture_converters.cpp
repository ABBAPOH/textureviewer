#include "texture.h"
#include "texture_p.h"

#include <HalfFloat>

#include <gsl/gsl_util>

namespace {

using ReaderFunc = ColorVariant(*)(Texture::ConstData);
using WriterFunc = void(*)(Texture::Data, const ColorVariant &);

struct TextureFormatConverter
{
    TextureFormat format {TextureFormat::Invalid};
    ReaderFunc reader {nullptr};
    WriterFunc writer {nullptr};
};

using TextureFormatConverters = gsl::span<const TextureFormatConverter>;

// generic functions

template<typename T>
typename Private::RgbaFromColorChannel_t<T> colorFunc(const ColorVariant &color)
{ return color.convert<typename Private::RgbaFromColorChannel_t<T>>(); }

template<typename Type, size_t components>
ColorVariant readRGBA(Texture::ConstData data)
{
    static_assert (components >= 1 && components <= 4, "Invalid components count");
    Q_ASSERT(data.size() == components * sizeof(Type));
    const auto d = reinterpret_cast<const Type *>(data.data());
    if (components == 1)
        return {Private::createRgba<Type>(d[0], Type(0), Type(0))};
    if (components == 2)
        return {Private::createRgba<Type>(d[0], d[1], Type(0))};
    if (components == 3)
        return {Private::createRgba<Type>(d[0], d[1], d[2])};
    if (components == 4)
        return {Private::createRgba<Type>(d[0], d[1], d[2], d[3])};
}

template<typename Type, size_t components>
void writeRGBA(Texture::Data data, const ColorVariant &color)
{
    static_assert (components >= 1 && components <= 4, "Invalid components count");
    Q_ASSERT(data.size() == components * sizeof(Type));
//    const auto rgba = color.value<typename Private::ColorChannelTraits<Type>::RgbaType>();
    const auto rgba = colorFunc<Type>(color);
    const auto d = reinterpret_cast<Type *>(data.data());
    if (components >= 1)
        d[0] = Type(getRed(rgba));
    if (components >= 2)
        d[1] = Type(getGreen(rgba));
    if (components >= 3)
        d[2] = Type(getBlue(rgba));
    if (components >= 4)
        d[3] = Type(getAlpha(rgba));
}

// specialized functions

ColorVariant readA8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 1);
    return {qRgba(0, 0, 0, *reinterpret_cast<const quint8 *>(data.data()))};
}

void writeA8_Unorm(Texture::Data data, const ColorVariant &color)
{
    Q_ASSERT(data.size() == 1);
    const auto rgba = color.convert<QRgb>();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = quint8(qAlpha(rgba));
}

ColorVariant readL8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 1);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgb(d[0], d[0], d[0])};
}

void writeL8_Unorm(Texture::Data data, const ColorVariant &color)
{
    Q_ASSERT(data.size() == 1);
    const auto rgba = color.convert<QRgb>();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = quint8((qRed(rgba) + qGreen(rgba) + qBlue(rgba)) / 3);
}

ColorVariant readLA8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 2);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[0], d[0], d[0], d[1])};
}

void writeLA8_Unorm(Texture::Data data, const ColorVariant &color)
{
    Q_ASSERT(data.size() == 2);
    const auto rgba = color.convert<QRgb>();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = quint8((qRed(rgba) + qGreen(rgba) + qBlue(rgba)) / 3);
    d[1] = quint8(qAlpha(rgba));
}

ColorVariant readBGR8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 3);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[2], d[1], d[0], 0xff)};
}

void writeBGR8_Unorm(Texture::Data data, const ColorVariant &color)
{
    Q_ASSERT(data.size() == 3);
    const auto rgba = color.convert<QRgb>();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[2] = quint8(qRed(rgba));
    d[1] = quint8(qGreen(rgba));
    d[0] = quint8(qBlue(rgba));
}

ColorVariant readBGRA8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 4);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[2], d[1], d[0], d[3])};
}

void writeBGRA8_Unorm(Texture::Data data, const ColorVariant &color)
{
    Q_ASSERT(data.size() == 4);
    const auto rgba = color.convert<QRgb>();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[2] = quint8(qRed(rgba));
    d[1] = quint8(qGreen(rgba));
    d[0] = quint8(qBlue(rgba));
    d[3] = quint8(qAlpha(rgba));
}

ColorVariant readABGR8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 4);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[3], d[2], d[1], d[0])};
}

void writeABGR8_Unorm(Texture::Data data, const ColorVariant &color)
{
    Q_ASSERT(data.size() == 4);
    const auto rgba = color.convert<QRgb>();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[3] = quint8(qRed(rgba));
    d[2] = quint8(qGreen(rgba));
    d[1] = quint8(qBlue(rgba));
    d[0] = quint8(qAlpha(rgba));
}

ColorVariant readRGBX8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 4);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[0], d[1], d[2], 0xff)};
}

void writeRGBX8_Unorm(Texture::Data data, const ColorVariant &color)
{
    Q_ASSERT(data.size() == 4);
    const auto rgba = color.convert<QRgb>();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = quint8(qRed(rgba));
    d[1] = quint8(qGreen(rgba));
    d[2] = quint8(qBlue(rgba));
    d[3] = 0xff;
}

ColorVariant readBGRX8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 4);
    const auto d = reinterpret_cast<const quint8 *>(data.data());
    return {qRgba(d[2], d[1], d[0], 0xff)};
}

void writeBGRX8_Unorm(Texture::Data data, const ColorVariant &color)
{
    Q_ASSERT(data.size() == 4);
    const auto rgba = color.convert<QRgb>();
    const auto d = reinterpret_cast<quint8 *>(data.data());
    d[2] = quint8(qRed(rgba));
    d[1] = quint8(qGreen(rgba));
    d[0] = quint8(qBlue(rgba));
    d[3] = 0xff;
}

constexpr TextureFormatConverter converters[] = {
    { TextureFormat::Invalid },

    // 8bit
    { TextureFormat::A8_Unorm, readA8_Unorm, writeA8_Unorm },
    { TextureFormat::L8_Unorm, readL8_Unorm, writeL8_Unorm },

    { TextureFormat::R8_Snorm, readRGBA<qint8,  1>, writeRGBA<qint8,  1> },
    { TextureFormat::R8_Unorm, readRGBA<quint8, 1>, writeRGBA<quint8, 1> },
    { TextureFormat::R8_Sint,  readRGBA<qint8,  1>, writeRGBA<qint8,  1> },
    { TextureFormat::R8_Uint,  readRGBA<quint8, 1>, writeRGBA<quint8, 1> },

    // 16 bit
    { TextureFormat::LA8_Unorm, readLA8_Unorm, writeLA8_Unorm },

    { TextureFormat::R16_Snorm, readRGBA<qint16,    1>, writeRGBA<qint16,    1> },
    { TextureFormat::R16_Unorm, readRGBA<quint16,   1>, writeRGBA<quint16,   1> },
    { TextureFormat::R16_Sint,  readRGBA<qint16,    1>, writeRGBA<qint16,    1> },
    { TextureFormat::R16_Uint,  readRGBA<quint16,   1>, writeRGBA<quint16,   1> },
    { TextureFormat::R16_Float, readRGBA<HalfFloat, 1>, writeRGBA<HalfFloat, 1> },

    { TextureFormat::RG8_Snorm, readRGBA<qint8,  2>, writeRGBA<qint8,  2> },
    { TextureFormat::RG8_Unorm, readRGBA<quint8, 2>, writeRGBA<quint8, 2> },
    { TextureFormat::RG8_Sint,  readRGBA<qint8,  2>, writeRGBA<qint8,  2> },
    { TextureFormat::RG8_Uint,  readRGBA<quint8, 2>, writeRGBA<quint8, 2> },

    // 24bit
    { TextureFormat::RGB8_Unorm, readRGBA<quint8, 3>, writeRGBA<quint8, 3> },
    { TextureFormat::BGR8_Unorm, readBGR8_Unorm, writeBGR8_Unorm },

    // 32bit
    { TextureFormat::R32_Sint,  readRGBA<qint32,  1>, writeRGBA<qint32,  1> },
    { TextureFormat::R32_Uint,  readRGBA<quint32, 1>, writeRGBA<quint32, 1> },
    { TextureFormat::R32_Float, readRGBA<float,   1>, writeRGBA<float,   1> },

    { TextureFormat::RG16_Snorm, readRGBA<qint16,    2>, writeRGBA<qint16,    2> },
    { TextureFormat::RG16_Unorm, readRGBA<quint16,   2>, writeRGBA<quint16,   2> },
    { TextureFormat::RG16_Sint,  readRGBA<qint16,    2>, writeRGBA<qint16,    2> },
    { TextureFormat::RG16_Uint,  readRGBA<quint16,   2>, writeRGBA<quint16,   2> },
    { TextureFormat::RG16_Float, readRGBA<HalfFloat, 2>, writeRGBA<HalfFloat, 2> },

    { TextureFormat::RGBA8_Snorm, readRGBA<qint8,  4>, writeRGBA<qint8,  4> },
    { TextureFormat::RGBA8_Unorm, readRGBA<quint8, 4>, writeRGBA<quint8, 4> },
    { TextureFormat::RGBA8_Sint,  readRGBA<qint8,  4>, writeRGBA<qint8,  4> },
    { TextureFormat::RGBA8_Uint,  readRGBA<quint8, 4>, writeRGBA<quint8, 4> },
    { TextureFormat::RGBA8_Srgb },

    { TextureFormat::BGRA8_Unorm, readBGRA8_Unorm, writeBGRA8_Unorm },
    { TextureFormat::BGRA8_Srgb },
    { TextureFormat::ABGR8_Unorm, readABGR8_Unorm, writeABGR8_Unorm },
    { TextureFormat::RGBX8_Unorm, readRGBX8_Unorm, writeRGBX8_Unorm },
    { TextureFormat::BGRX8_Unorm, readBGRX8_Unorm, writeBGRX8_Unorm },
    { TextureFormat::BGRX8_Srgb },

    // 64bit
    { TextureFormat::RGBA16_Snorm, readRGBA<qint16,    4>, writeRGBA<qint16,    4> },
    { TextureFormat::RGBA16_Unorm, readRGBA<quint16,   4>, writeRGBA<quint16,   4> },
    { TextureFormat::RGBA16_Sint,  readRGBA<qint16,    4>, writeRGBA<qint16,    4> },
    { TextureFormat::RGBA16_Uint,  readRGBA<quint16,   4>, writeRGBA<quint16,   4> },
    { TextureFormat::RGBA16_Float, readRGBA<HalfFloat, 4>, writeRGBA<HalfFloat, 4> },

    { TextureFormat::RG32_Sint,  readRGBA<qint32,  2>, writeRGBA<qint32,  2> },
    { TextureFormat::RG32_Uint,  readRGBA<quint32, 2>, writeRGBA<quint32, 2> },
    { TextureFormat::RG32_Float, readRGBA<float,   2>, writeRGBA<float,   2> },

    // 96bit
    { TextureFormat::RGB32_Sint,  readRGBA<qint32,  3>, writeRGBA<qint32,  3> },
    { TextureFormat::RGB32_Uint,  readRGBA<quint32, 3>, writeRGBA<quint32, 3> },
    { TextureFormat::RGB32_Float, readRGBA<float,   3>, writeRGBA<float,   3> },

    // 128bit
    { TextureFormat::RGBA32_Sint,  readRGBA<qint32,  4>, writeRGBA<qint32,  4> },
    { TextureFormat::RGBA32_Uint,  readRGBA<quint32, 4>, writeRGBA<quint32, 4> },
    { TextureFormat::RGBA32_Float, readRGBA<float,   4>, writeRGBA<float,   4> },

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

constexpr qsizetype convertionsCount()
{
    qsizetype result = 0;
    for (const auto &info: converters) {
        if (info.reader && info.writer)
            result++;
    }
    return result;
}

class SupportedConvertionsHolder
{
public:
    TextureFormat formats[convertionsCount()] = {};

    constexpr SupportedConvertionsHolder()
    {
        qsizetype pos = 0;
        for (const auto &info: converters) {
            if (info.reader && info.writer)
                gsl::at(formats, pos++) = info.format;
        }
    }
} supportedConvertionsHolder;

} // namespace

std::function<ColorVariant(Texture::ConstData)> TextureData::getFormatReader(TextureFormat format)
{
    return gsl::at(converters, qsizetype(format)).reader;
}

std::function<void(Texture::Data, const ColorVariant &color)> TextureData::getFormatWriter(TextureFormat format)
{
    return gsl::at(converters, qsizetype(format)).writer;
}

/*!
    \internal
    Returns the list of formats Texture can convert.
*/
gsl::span<const TextureFormat> Texture::supportedConvertions()
{
    return supportedConvertionsHolder.formats;
}

