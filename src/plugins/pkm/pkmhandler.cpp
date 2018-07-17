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

bool PkmHandler::read(Texture& texture)
{
    PkmHeader header;
    {
        QDataStream s(device().get());
        s.setByteOrder(QDataStream::BigEndian);
        s >> header;
    }

    if (QLatin1String(reinterpret_cast<const char *>(header.magic), 4) != "PKM ") {
        qCWarning(pkmhandler) << "Invalid magic number";
        return false;
    }

    Texture::Format format = Texture::Format::Invalid;

    if (header.version[0] == '1' && header.version[1] == '0') {
        format = Texture::Format::ETC1_RGB8;
    } else {
        qCWarning(pkmhandler) << "Unsupported version"
                              << QString("%1.%2").arg(header.version[0]).arg(header.version[1]);
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
