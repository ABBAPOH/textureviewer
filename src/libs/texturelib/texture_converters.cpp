#include "texture.h"
#include "texture_p.h"

#include <gsl/gsl_util>

namespace {

using ReaderFunc = AnyColor(*)(Texture::ConstData);
using WriterFunc = void(*)(Texture::Data, const AnyColor &);

struct TextureFormatConverters
{
    TextureFormat format {TextureFormat::Invalid};
    ReaderFunc reader {nullptr};
    WriterFunc writer {nullptr};
};

AnyColor readA8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 1);
    return {qRgba(0, 0, 0, *reinterpret_cast<const quint8 *>(data.data()))};
}

void writeA8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 1);
    const auto rgba = color.toRgba8_Unorm();
    *reinterpret_cast<quint8 *>(data.data()) = qAlpha(rgba);
}

AnyColor readL8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 1);
    const auto l = *reinterpret_cast<const quint8 *>(data.data());
    return {qRgb(l, l, l)};
}

void writeL8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 1);
    const auto rgba = color.toRgba8_Unorm();
    const auto l = (qRed(rgba) + qGreen(rgba) + qBlue(rgba)) / 3;
    *reinterpret_cast<quint8 *>(data.data()) = l;
}

AnyColor readR8_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 1);
    const auto red = *reinterpret_cast<const quint8 *>(data.data());
    return {qRgb(red, 0, 0)};
}

void writeR8_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 1);
    const auto rgba = color.toRgba8_Unorm();
    *reinterpret_cast<quint8 *>(data.data()) = qRed(rgba);
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
    const auto l = (qRed(rgba) + qGreen(rgba) + qBlue(rgba)) / 3;
    auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = l;
    d[1] = qAlpha(rgba);
}

AnyColor readR16_Unorm(Texture::ConstData data)
{
    Q_ASSERT(data.size() == 2);
    const auto red = *reinterpret_cast<const quint16 *>(data.data());
    return {qRgba64(red, 0, 0, 0xffff)};
}

void writeR16_Unorm(Texture::Data data, const AnyColor &color)
{
    Q_ASSERT(data.size() == 2);
    const auto rgba = color.toRgba16_Unorm();
    *reinterpret_cast<quint16 *>(data.data()) = qRed(rgba);
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
    auto d = reinterpret_cast<quint8 *>(data.data());
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
    auto d = reinterpret_cast<quint8 *>(data.data());
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
    auto d = reinterpret_cast<quint8 *>(data.data());
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
    auto d = reinterpret_cast<quint16 *>(data.data());
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
    Q_ASSERT(data.size() == 3);
    const auto rgba = color.toRgba8_Unorm();
    auto d = reinterpret_cast<quint8 *>(data.data());
    d[0] = qRed(rgba);
    d[1] = qGreen(rgba);
    d[2] = qBlue(rgba);
    d[3] = qAlpha(rgba);
}

TextureFormatConverters converters[] = {
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
};

} // namespace

std::function<AnyColor(Texture::ConstData)> TextureData::getFormatReader(TextureFormat format)
{
    return gsl::at(converters, size_t(format)).reader;
}

std::function<void(Texture::Data, const AnyColor &color)> TextureData::getFormatWriter(TextureFormat format)
{
    return gsl::at(converters, size_t(format)).writer;
}
