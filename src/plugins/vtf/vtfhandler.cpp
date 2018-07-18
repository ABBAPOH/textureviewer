#include "vtfhandler.h"
#include "vtfenums.h"

template<typename T>
inline constexpr bool isPower2(T value) noexcept
{
    return (value - 1) ^ value;
}

static bool validateHeader(const VTFHeader &header)
{
    if (header.signature != 0x00465456) {
        qCWarning(vtfhandler) << "Invalid header.signature:" << header.signature;
        return false;
    }

    if (header.headerSize % 16 != 0) {
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

    if (header.numResources > maxResourcesCount) {
        qCWarning(vtfhandler) << "invalid numResources:" << header.numResources;
        return false;
    }

    if (!(header.version[0] >= 7)) {
        qCWarning(vtfhandler) << "Versions before 7.0 are not supported";
        return false;
    }

    return true;
}

static bool readPadding(VTFHandler::QIODevicePointer device, qint64 size)
{
    if (size == 0)
        return true;

    const auto buffer = std::make_unique<char[]>(size_t(size));
    const auto read = device->read(buffer.get(), size);
    if (read != size) {
        qCWarning(vtfhandler) << "Can't read padding of size" << size
                              << ":" << device->errorString();
        return false;
    }
    return true;
}

static Texture::Format convertFormat(VTFImageFormat format)
{
    switch (format) {
    case VTFImageFormat::RGBA_16161616: return Texture::Format::RGBA_16161616;
    case VTFImageFormat::RGBA_8888: return Texture::Format::RGBA_8888;
    case VTFImageFormat::BGRA_8888: return Texture::Format::BGRA_8888;
    case VTFImageFormat::ABGR_8888: return Texture::Format::ABGR_8888;
    case VTFImageFormat::BGRX_8888: return Texture::Format::BGRX_8888;
    case VTFImageFormat::BGRA_4444: return Texture::Format::BGRA_4444;
    case VTFImageFormat::BGRX_5551: return Texture::Format::BGRX_5551;
    case VTFImageFormat::BGRA_5551: return Texture::Format::BGRA_5551;
    case VTFImageFormat::RGB_565: return Texture::Format::RGB_565;
    case VTFImageFormat::BGR_565: return Texture::Format::BGR_565;
    case VTFImageFormat::RGB_888: return Texture::Format::RGB_888;
    case VTFImageFormat::BGR_888: return Texture::Format::BGR_888;
    case VTFImageFormat::DXT1: return Texture::Format::DXT1;
    case VTFImageFormat::DXT1_ONEBITALPHA: return Texture::Format::DXT1a;
    case VTFImageFormat::DXT3: return Texture::Format::DXT3;
    case VTFImageFormat::DXT5: return Texture::Format::DXT5;
    case VTFImageFormat::RGBA_16161616F: return Texture::Format::RGBA_16161616F;
    default: return Texture::Format::Invalid;
    }
}

static bool readTexture(
        VTFHandler::QIODevicePointer device,
        const VTFHeader &header,
        Texture &texture)
{
    const auto highFormat = vtfFormat(header.highResImageFormat);
    const auto format = convertFormat(highFormat);
    if (format == Texture::Format::Invalid) {
        qCWarning(vtfhandler) << "format" << header.highResImageFormat << "is not supported";
        return false;
    }

    const auto depth = std::max<quint16>(1, header.depth);
    auto result = Texture::create(
                format,
                {header.width, header.height, depth},
                header.mipmapCount,
                header.frames);
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
                    if (result.isCompressed()) {
                        qsizetype size = pitch * std::max<quint32>(1, (height + 3) / 4);
                        if (size != result.bytesPerImage(level)) {
                            qCWarning(vtfhandler) << "Image size != texture size:"
                                                  << size << "!=" << result.bytesPerImage(level);
                            return false;
                        }
                        const auto data = result.imageData({Texture::Side(face), level, layer});
                        const auto read = device->read(reinterpret_cast<char *>(data.data()), size);
                        if (read != size) {
                            qCWarning(vtfhandler) << "Can't read from device:"
                                                  << device->errorString();
                            return false;
                        }
                    } else {
                        for (int y = 0; y < height; ++y) {
                            const auto line = result.lineData(
                            {0, y, z}, {(Texture::Side(face)), level, layer});
                            auto read = device->read(reinterpret_cast<char *>(line.data()), pitch);
                            if (read != pitch) {
                                qCWarning(vtfhandler) << "Can't read from device:"
                                                      << device->errorString();
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    texture = std::move(result);
    return true;
}

bool VTFHandler::read(Texture &texture)
{
    VTFHeader header;

    QDataStream s(device().get());
    s >> header;

    qCDebug(vtfhandler) << "header:" << header;

    if (s.status() != QDataStream::Ok) {
        qCWarning(vtfhandler) << "Invalid data stream status:" << s.status();
        return false;
    }

    if (!validateHeader(header))
        return false;

    // read padding after header before resources entries
    if (!readPadding(device(), 15 - (device()->pos() + 15) % 16))
        return false;

    if (header.version[0] == 7) {
        if (header.version[1] == 0
                || header.version[1] == 1
                || header.version[1] == 2) {
            const auto lowSize = header.lowResImageHeight * header.lowResImageHeight / 2;
            if (!readPadding(device(), lowSize))
                return false;
            return readTexture(device(), header, texture);
        }

        if (header.version[1] == 3
                || header.version[1] == 4
                || header.version[1] == 5) {

            std::vector<VTFResourceEntry> resources;
            resources.reserve(header.numResources);
            for (quint32 i = 0; i < header.numResources; ++i) {
                resources.emplace_back();
                s >> resources.back();
            }

            // Sort entries by offset in a file so we can simply skip padding instead of seeking
            // (in case we need any other resources except Image)
            const auto lessThan = [](const VTFResourceEntry &lhs, const VTFResourceEntry &rhs)
            {
                return lhs.data < rhs.data;
            };
            std::sort(resources.begin(), resources.end(), lessThan);

            for (const auto &entry: resources) {
                if (entry.type == quint32(VTFResourceType::LegacyImage)) {
                    if (!readPadding(device(), entry.data - device()->pos()))
                        return false;
                    return readTexture(device(), header, texture);
                }
            }

            qCWarning(vtfhandler) << "Can't find any image resource";
            return false;

        }
    }

    qCWarning(vtfhandler) << "Unsupported version"
                          << QString("%1.%2").arg(header.version[0]).arg(header.version[1]);
    return false;
}

Q_LOGGING_CATEGORY(vtfhandler, "pluigns.textureformats.vtfhandler")
