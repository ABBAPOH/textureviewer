#include "pkmhandler.h"

#include <TextureLib/Texture>

#include <OptionalType>

#include <QtCore/QDataStream>

namespace {

constexpr auto maxSize = std::numeric_limits<quint16>::max();

constexpr TextureFormat v2Formats[] = {
    TextureFormat::RGB8_ETC1,
    TextureFormat::RGB8_ETC2,
    TextureFormat::Invalid,
    TextureFormat::RGBA8_ETC2_EAC,
    TextureFormat::RGB8_PunchThrough_Alpha1_ETC2,
    TextureFormat::R11_EAC_UNorm,
    TextureFormat::RG11_EAC_UNorm,
    TextureFormat::R11_EAC_SNorm,
    TextureFormat::RG11_EAC_SNorm,
};

using V2Formats = gsl::span<const TextureFormat>;

struct PkmHeader
{
    quint8 magic[4] { 'P', 'K', 'M', ' '};
    quint8 version[2] { '2', '0' };
    quint16 textureType {};
    quint16 paddedWidth {};
    quint16 paddedHeight {};
    quint16 width {};
    quint16 height {};
};

QDataStream &operator>>(QDataStream &s, PkmHeader &header)
{
    s >> header.magic[0] >> header.magic[1] >> header.magic[2] >> header.magic[3];
    s >> header.version[0] >> header.version[1];
    s >> header.textureType;
    s >> header.paddedWidth;
    s >> header.paddedHeight;
    s >> header.width;
    s >> header.height;

    return s;
}

QDataStream &operator<<(QDataStream &s, const PkmHeader &header)
{
    s << header.magic[0] << header.magic[1] << header.magic[2] << header.magic[3];
    s << header.version[0] << header.version[1];
    s << header.textureType;
    s << header.paddedWidth;
    s << header.paddedHeight;
    s << header.width;
    s << header.height;

    return s;
}

QDebug &operator<<(QDebug &d, const PkmHeader &header)
{
    d << "PkmHeader {"
      << "magic:" << QLatin1String(reinterpret_cast<const char *>(header.magic), 4) << ","
      << "version:" << (QString("%1.%2")
                        .arg(QLatin1Char(header.version[0]))
                        .arg(QLatin1Char(header.version[1]))) << ","
      << "textureType:" << header.textureType << ","
      << "paddedWidth:" << header.paddedWidth << ","
      << "paddedHeight:" << header.paddedHeight << ","
      << "width:" << header.paddedHeight << ","
      << "height:" << header.paddedHeight
      << "}";

    return d;
}

template<typename T>
inline constexpr bool isPower2(T value) noexcept
{
    return (value - 1) ^ value;
}

constexpr TextureFormat convertFormat(quint16 format)
{
    if (format >= V2Formats(v2Formats).size())
        return TextureFormat::Invalid;
    return gsl::at(v2Formats, format);
}

constexpr Optional<quint16> convertFormat(TextureFormat format)
{
    quint16 position = 0;
    for (const auto fmt : V2Formats(v2Formats)) {
        if (fmt == format)
            return position;
        position++;
    }
    return nullOptional();
}

constexpr TextureFormat getFormat(const PkmHeader &header)
{
    if (header.version[0] == '1') {
        return TextureFormat::RGB8_ETC1;
    } else if (header.version[0] == '2') {
        return convertFormat(header.textureType);
    }

    return TextureFormat::Invalid;
}

bool verifyHeader(const PkmHeader &header)
{
    if (QLatin1String(reinterpret_cast<const char *>(header.magic), 4) != "PKM ") {
        qCWarning(pkmhandler) << "Invalid magic number";
        return false;
    }

    if (!((header.version[0] == '1' && header.version[1] == '0')
          || (header.version[0] == '2' && header.version[1] == '0'))) {
        qCWarning(pkmhandler) << "Unsupported version"
                              << QString("%1.%2").arg(header.version[0]).arg(header.version[1]);
        return false;
    }

    return true;
}

bool verifyTexture(const Texture &texture)
{
    if (!texture.isCompressed()) {
        qCWarning(pkmhandler) << "Writing uncomressed textures is not supported";
        return false;
    }

    if (!isPower2(texture.width())) {
        qCWarning(pkmhandler) << "Width should be a power of two";
        return false;
    }

    if (!isPower2(texture.height())) {
        qCWarning(pkmhandler) << "Height should be a power of two";
        return false;
    }

    if (texture.width() > maxSize) {
        qCWarning(pkmhandler) << "Width is too big:" << texture.width();
        return false;
    }

    if (texture.height() > maxSize) {
        qCWarning(pkmhandler) << "Height is too big:" << texture.height();
        return false;
    }

    if (texture.depth() > 1) {
        qCWarning(pkmhandler) << "Writing volumemaps is not supported";
        return false;
    }

    if (texture.faces() > 1) {
        qCWarning(pkmhandler) << "Writing cubemaps is not supported";
        return false;
    }

    if (texture.layers() > 1) {
        qCWarning(pkmhandler) << "Writing texture arrays is not supported";
        return false;
    }

    return true;
}

} // namespace

bool PkmHandler::read(Texture& texture)
{
    PkmHeader header;

    {
        QDataStream s(device().get());
        s.setByteOrder(QDataStream::BigEndian);
        s >> header;

        qCDebug(pkmhandler) << "header:" << header;

        if (s.status() != QDataStream::Ok) {
            qCWarning(pkmhandler) << "Invalid data stream status:" << s.status();
            return false;
        }
    }

    if (!verifyHeader(header))
        return false;

    const auto format = getFormat(header);

    if (format == TextureFormat::Invalid) {
        qCWarning(pkmhandler) << "Unsupported format" << header.textureType;
        return false;
    }

    auto result = Texture::create(format, {header.width, header.height});
    if (result.isNull()) {
        qCWarning(pkmhandler) << "Can't create texture";
        return false;
    }

    const auto data = result.imageData({});
    const auto read = device()->read(reinterpret_cast<char *>(data.data()), data.size());
    if (read != data.size()) {
        qCWarning(pkmhandler) << "Can't read from device:" << device()->errorString();
        return false;
    }

    texture = std::move(result);

    return true;
}

bool PkmHandler::write(const Texture& texture)
{
    if (!verifyTexture(texture))
        return false;

    const auto format = convertFormat(texture.format());
    if (!format) {
        qCWarning(pkmhandler) << "Unsupported format" << texture.format();
        return false;
    }

    PkmHeader header;
    header.textureType = *format;
    header.width = quint16(texture.width());
    header.height = quint16(texture.width());
    header.paddedWidth = header.width;
    header.paddedHeight = header.height;

    {
        QDataStream s(device().get());
        s.setByteOrder(QDataStream::BigEndian);
        s << header;

        if (s.status() != QDataStream::Ok) {
            qCWarning(pkmhandler) << "Invalid data stream status:" << s.status();
            return false;
        }
    }

    const auto data = texture.imageData({});
    const auto read = device()->write(reinterpret_cast<const char *>(data.data()), data.size());
    if (read != data.size()) {
        qCWarning(pkmhandler) << "Can't write to device:" << device()->errorString();
        return false;
    }

    return true;
}

Q_LOGGING_CATEGORY(pkmhandler, "pluigns.textureformats.pkmhandler")
