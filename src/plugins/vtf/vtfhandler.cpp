#include "vtfhandler.h"
#include "vtfenums.h"

bool VTFHandler::canRead() const
{
    return canRead(device());
}

bool VTFHandler::read(Texture &texture)
{
    if (!canRead(device()))
        return false;

    VTFHeader header;

    QDataStream s(device());
    s >> header;

    if (s.status() != QDataStream::Ok)
        return false;

    const auto lowFormat = vtfFormat(header.lowResImageFormat);

    if (lowFormat != VTFImageFormat::DXT1) {
        qCWarning(vtfhandler) << "lowResImageFormat is not DXT1";
        return false;
    }

    const auto highFormat = vtfFormat(header.highResImageFormat);
    if (highFormat != VTFImageFormat::BGRA_8888) {
        qCWarning(vtfhandler) << "format" << header.highResImageFormat << "is not supported";
        return false;
    }

    if (header.lowResImageHeight > 16 ||  header.lowResImageHeight > 16) {
        qCWarning(vtfhandler) << "invalid low resolution size:"
                              << QSize(header.lowResImageHeight, header.lowResImageHeight);
        return false;
    }

    if (header.version[0] == 7 && header.version[1] == 2) {
        auto lowSize = header.lowResImageHeight * header.lowResImageHeight / 16;
        std::unique_ptr<char[]> buffer(new char[size_t(lowSize)]);
        auto read = device()->read(buffer.get(), lowSize);
        if (read != lowSize) {
            qCWarning(vtfhandler) << "Can't read low resolution image" << device()->errorString();
            return false;
        }
    }

    texture = Texture::create(Texture::Type::Texture2D, Texture::Format::BGRA_8888, header.width, header.height, 1);
    if (texture.isNull()) {
        qCWarning(vtfhandler) << "Can't create resulting texture, file is too big or corrupted";
        return false;
    }

    qCDebug(vtfhandler) << "header:" << header;

    return true;
}

bool VTFHandler::canRead(QIODevice *device) const
{
    if (!device) {
        qCWarning(vtfhandler) << "canRead() called with no device";
        return false;
    }

    if (device->isSequential()) {
        qCWarning(vtfhandler) << "Sequential devices are not supported";
        return false;
    }

    return device->peek(4) == QByteArrayLiteral("VTF\0");
}

QByteArray VTFHandlerPlugin::name() const
{
    return QByteArrayLiteral("vtf");
}

std::unique_ptr<TextureIOHandler> VTFHandlerPlugin::create(const QMimeType &mimeType)
{
    if (mimeType.name() == u"image/x-vtf")
        return std::make_unique<VTFHandler>();
    return nullptr;
}

TextureIOHandlerPlugin::Capabilities VTFHandlerPlugin::capabilities(const QMimeType &mimeType) const
{
    if (mimeType.name() == u"image/x-vtf")
        return Capabilities(VTFHandlerPlugin::CanRead);
    return Capabilities();
}

Q_LOGGING_CATEGORY(vtfhandler, "pluigns.textureformats.vtfhandler")
