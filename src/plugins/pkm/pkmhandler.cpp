#include "pkmhandler.h"

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

static Texture::Format convertFormat(quint16 format)
{
    switch (format) {
    case 0: return Texture::Format::RGB8_ETC1;
    case 1: return Texture::Format::RGB8_ETC2;
    case 3: return Texture::Format::RGBA8_ETC2_EAC;
    case 4: return Texture::Format::RGB8_PunchThrough_Alpha1_ETC2;
    case 5: return Texture::Format::R11_EAC_UNorm;
    case 6: return Texture::Format::RG11_EAC_UNorm;
    case 7: return Texture::Format::R11_EAC_SNorm;
    case 8: return Texture::Format::RG11_EAC_SNorm;
    }
    return Texture::Format::Invalid;
}

bool PkmHandler::read(Texture& texture)
{
    PkmHeader header;
    {
        QDataStream s(device().get());
        s.setByteOrder(QDataStream::BigEndian);
        s >> header;
        qCDebug(pkmhandler) << "header:" << header;
    }

    if (QLatin1String(reinterpret_cast<const char *>(header.magic), 4) != "PKM ") {
        qCWarning(pkmhandler) << "Invalid magic number";
        return false;
    }

    Texture::Format format = Texture::Format::Invalid;

    if (header.version[0] == '1' && header.version[1] == '0') {
        format = Texture::Format::RGB8_ETC1;
    } else if (header.version[0] == '2' && header.version[1] == '0') {
        format = convertFormat(header.textureType);
    } else {
        qCWarning(pkmhandler) << "Unsupported version"
                              << QString("%1.%2").arg(header.version[0]).arg(header.version[1]);
        return false;
    }

    if (format == Texture::Format::Invalid) {
        qCWarning(pkmhandler) << "Unsupported format" << header.textureType;
        return false;
    }

    auto result = Texture::create(Texture::Type::Texture2D, format, {header.width, header.height});
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

std::unique_ptr<TextureIOHandler> PkmHandlerPlugin::create(const QMimeType& mimeType)
{
    Q_UNUSED(mimeType);
    return std::make_unique<PkmHandler>();
}

TextureIOHandlerPlugin::Capabilities PkmHandlerPlugin::capabilities(const QMimeType& mimeType) const
{
    Q_UNUSED(mimeType);
    return CanRead;
}

Q_LOGGING_CATEGORY(pkmhandler, "pluigns.textureformats.pkmhandler")
