#include "vtfhandler.h"
#include "vtfenums.h"

template<typename T>
inline bool isPower2(T value)
{
    return (value - 1) ^ value;
}

static bool validateHeader(const VTFHeader &header)
{
    if (!header.width || !isPower2(header.width)) {
        qCWarning(vtfhandler) << "Invalid header.width value: "
                              << header.width << "Should be greater then zero and be power of 2";
        return false;
    }

    if (!header.height || !isPower2(header.height)) {
        qCWarning(vtfhandler) << "Invalid header.height value: "
                              << header.height << "Should be greater then zero and be power of 2";
        return false;
    }

    if (!header.frames) {
        qCWarning(vtfhandler) << "header.frames should be greater then zero";
        return false;
    }

    if (!header.mipmapCount) {
        qCWarning(vtfhandler) << "header.mipmapCount should be greater then zero";
        return false;
    }

    const auto lowFormat = vtfFormat(header.lowResImageFormat);
    if (lowFormat != VTFImageFormat::DXT1) {
        qCWarning(vtfhandler) << "lowResImageFormat is not DXT1";
        return false;
    }

    if (header.lowResImageHeight > 16 ||  header.lowResImageHeight > 16) {
        qCWarning(vtfhandler) << "invalid low resolution size:"
                              << QSize(header.lowResImageHeight, header.lowResImageHeight);
        return false;
    }

    if (!(header.version[0] == 7 && header.version[1] == 2)) {
        qCWarning(vtfhandler) << "Only version 7.2 is supported";
        return false;
    }

    return true;
}

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

    if (!validateHeader(header))
        return false;

    const auto highFormat = vtfFormat(header.highResImageFormat);
    if (highFormat != VTFImageFormat::BGRA_8888) {
        qCWarning(vtfhandler) << "format" << header.highResImageFormat << "is not supported";
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
