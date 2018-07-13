#include "vtfhandler.h"
#include "vtfenums.h"

template<typename T>
inline bool isPower2(T value)
{
    return (value - 1) ^ value;
}

static bool validateHeader(const VTFHeader &header)
{
    if (header.headerSize != 80) {
        qCWarning(vtfhandler) << "Invalid header.headerSize:" << header.headerSize;
        return false;
    }

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

static bool readPadding(VTFHandler::QIODevicePointer device, qint64 size)
{
    const auto buffer = std::make_unique<char[]>(size_t(size));
    const auto read = device->read(buffer.get(), size);
    if (read != size) {
        qCWarning(vtfhandler) << "Can't read padding of size" << size
                              << ":" << device->errorString();
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

    QDataStream s(device().get());
    s >> header;

    if (s.status() != QDataStream::Ok) {
        qCWarning(vtfhandler) << "Invalid data stream status:" << s.status();
        return false;
    }

    if (!validateHeader(header))
        return false;

    if (!readPadding(device(), header.headerSize - device()->pos()))
        return false;

    const auto highFormat = vtfFormat(header.highResImageFormat);

    auto format = Texture::Format::Invalid;
    switch (highFormat) {
    case VTFImageFormat::BGRA_8888: format = Texture::Format::BGRA_8888; break;
    case VTFImageFormat::RGB_888: format = Texture::Format::RGB_888; break;
    default: break;
    }

    if (format == Texture::Format::Invalid) {
        qCWarning(vtfhandler) << "format" << header.highResImageFormat << "is not supported";
        return false;
    }

    if (header.version[0] == 7 && header.version[1] == 2) {
        auto lowSize = header.lowResImageHeight * header.lowResImageHeight / 2;
        if (!readPadding(device(), lowSize))
            return false;
    }

    auto result = Texture::create(Texture::Type::Texture2D, format, header.width, header.height, 1);
    if (result.isNull()) {
        qCWarning(vtfhandler) << "Can't create resulting texture, file is too big or corrupted";
        return false;
    }

    for (int level = header.mipmapCount - 1; level >= 0; --level) {
        auto width = std::max<quint16>(1, header.width >> level);
        auto height = std::max<quint16>(1, header.height >> level);
        auto depth = std::max<quint16>(1, header.depth >> level);
        auto pitch = Texture::calculateBytesPerLine(format, width);

        for (int layer = 0; layer < header.frames; ++layer) {
            for (int face = 0; face < 1; ++face) { // TODO: where do we get faces??
                for (int z = 0; z < depth; ++z) {
                    for (int y = 0; y < height; ++y) {
                        const auto line = result.lineData(
                                    {0, y, z}, {(Texture::Side(face)), layer, level});
                        auto read = device()->read(reinterpret_cast<char *>(line.data()), pitch);
                        if (read != pitch) {
                            qCWarning(vtfhandler) << "Can't read from file:"
                                                  << device()->errorString();
                            return false;
                        }
                    }
                }
            }
        }
    }

    texture = std::move(result);

    qCDebug(vtfhandler) << "header:" << header;

    return true;
}

bool VTFHandler::canRead(QIODevicePointer device) const
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
