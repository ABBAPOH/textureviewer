#include "vtfhandler.h"

bool VTFHandler::canRead() const
{
    return canRead(device());
}

bool VTFHandler::read(Texture &texture)
{
    VTFHeader header;

    QDataStream s(device());
    s >> header;

    if (s.status() != QDataStream::Ok)
        return false;

    qCDebug(vtfhandler) << "header:" << header;

    return true;
}

bool VTFHandler::canRead(QIODevice *device)
{
    if (!device) {
        qCWarning(vtfhandler) << "DDSHandler: canRead() called with no device";
        return false;
    }

    if (device->isSequential()) {
        qCWarning(vtfhandler) << "DDSHandler: Sequential devices are not supported";
        return false;
    }

    return device->peek(4) == QByteArrayLiteral("VTF\0");
}

QByteArray VTFHandlerPlugin::name() const
{
    return QByteArrayLiteral("vtf");
}

std::unique_ptr<TextureIOHandler> VTFHandlerPlugin::create(QIODevice *device, const QMimeType &mimeType)
{
    Q_UNUSED(device);
    Q_UNUSED(mimeType);
    return std::make_unique<VTFHandler>();
}

TextureIOHandlerPlugin::Capabilities VTFHandlerPlugin::capabilities(QIODevice *device, const QMimeType &mimeType) const
{
    Q_UNUSED(device);
    Q_UNUSED(mimeType);
    return Capabilities(VTFHandlerPlugin::CanRead);
}

Q_LOGGING_CATEGORY(vtfhandler, "pluigns.textureformats.vtfhandler")
