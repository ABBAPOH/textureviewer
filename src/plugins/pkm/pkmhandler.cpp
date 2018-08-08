#include "pkmhandler.h"

#include <TextureLib/Texture>

#include <QtCore/QDataStream>

namespace {

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
    quint8 magic[4];
    quint8 version[2];
    quint16 textureType;
    quint16 paddedWidth;
    quint16 paddedHeight;
    quint16 width;
    quint16 height;
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

constexpr TextureFormat convertFormat(quint16 format)
{
    if (format >= V2Formats(v2Formats).size())
        return TextureFormat::Invalid;
    return gsl::at(v2Formats, format);
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

    if (QLatin1String(reinterpret_cast<const char *>(header.magic), 4) != "PKM ") {
        qCWarning(pkmhandler) << "Invalid magic number";
        return false;
    }

    TextureFormat format = TextureFormat::Invalid;

    if (header.version[0] == '1' && header.version[1] == '0') {
        format = TextureFormat::RGB8_ETC1;
    } else if (header.version[0] == '2' && header.version[1] == '0') {
        format = convertFormat(header.textureType);
    } else {
        qCWarning(pkmhandler) << "Unsupported version"
                              << QString("%1.%2").arg(header.version[0]).arg(header.version[1]);
        return false;
    }

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

Q_LOGGING_CATEGORY(pkmhandler, "pluigns.textureformats.pkmhandler")
