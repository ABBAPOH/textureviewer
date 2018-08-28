#include "ktxhandler.h"
#include "ktxheader.h"

#include <TextureLib/Texture>
#include <TextureLib/TextureFormatInfo>

namespace {

constexpr auto maxInt = std::numeric_limits<int>::max();

struct FormatInfo
{
    quint32 internalFormat {QOpenGLTexture::TextureFormat::NoFormat};
    quint32 pixelFormat {QOpenGLTexture::PixelFormat::NoSourceFormat};
    quint32 pixelType {QOpenGLTexture::PixelType::NoPixelType};
    TextureFormat textureFormat {TextureFormat::Invalid};
};

using FormatInfos = gsl::span<const FormatInfo>;

constexpr FormatInfo extraInfos[] = {
    { 0x8040 /*GL_LUMINANCE8*/, QOpenGLTexture::Luminance, QOpenGLTexture::UInt8, TextureFormat::L8_Unorm },
};

constexpr TextureFormat findFormat(quint32 textureFormat, quint32 pixelFormat, quint32 pixelType)
{
    for (const auto &info: FormatInfos(extraInfos)) {
        if (textureFormat == info.internalFormat
                && pixelFormat == info.pixelFormat
                && pixelType == info.pixelType) {
            return info.textureFormat;
        }
    }
    return TextureFormat::Invalid;
}

bool readPadding(KtxHandler::QIODevicePointer device, qint64 size)
{
    if (size == 0)
        return true;

    const auto buffer = std::make_unique<char[]>(size_t(size));
    const auto read = device->read(buffer.get(), size);
    if (read != size) {
        qCWarning(ktxhandler) << "Can't read padding of size" << size
                              << ":" << device->errorString();
    }
    return read == size;
}

bool verifyHeader(const KtxHeader &header)
{
    if (header.endianness != 0x01020304 && header.endianness != 0x04030201) {
        qCWarning(ktxhandler) << "Invalid endianess value:" << header.endianness;
        return false;
    }

    if (header.pixelWidth > maxInt) {
        qCWarning(ktxhandler) << "Pixel width is too big:" << header.pixelWidth;
        return false;
    }

    if (header.pixelHeight > maxInt) {
        qCWarning(ktxhandler) << "Pixel height is too big:" << header.pixelHeight;
        return false;
    }

    if (header.pixelDepth > maxInt) {
        qCWarning(ktxhandler) << "Pixel depth is too big:" << header.pixelDepth;
        return false;
    }

    if (header.numberOfArrayElements > maxInt) {
        qCWarning(ktxhandler) << "Number of array elements is too big:"
                              << header.numberOfArrayElements;
        return false;
    }

    if (header.numberOfFaces != 1 && header.numberOfFaces != 6) {
        qCWarning(ktxhandler) << "Number of faces is invalid:" << header.numberOfFaces;
        return false;
    }

    const auto levels = header.numberOfMipmapLevels;
    const auto size = std::max(std::max(header.pixelWidth, header.pixelHeight), header.pixelDepth);
    if (header.numberOfMipmapLevels > maxInt || (levels && (size >> (levels - 1)) < 1)) {
        qCWarning(ktxhandler) << "Number of mipmap levels is too big:" << levels;
        return false;
    }

    return true;
}

} // namespace

bool KtxHandler::read(Texture& texture)
{
    KtxHeader header = {};

    QDataStream s(device().get());
    s >> header;

    qCDebug(ktxhandler) << "header:" << header;

    if (s.status() != QDataStream::Ok) {
        qCWarning(ktxhandler) << "Can't read header: data stream status =" << s.status();
        return false;
    }

    if (!verifyHeader(header))
        return false;

    if (!readPadding(device(), header.bytesOfKeyValueData))
        return false;

    readPadding(device(), 3 - ((device()->pos() + 3) % 4));

    TextureFormat textureFormat = TextureFormat::Invalid;
    if (header.glFormat == 0 && header.glType == 0) {
        textureFormat = TextureFormatInfo::findOGLFormat(
                    QOpenGLTexture::TextureFormat(header.glInternalFormat)).format();
    } else {
        textureFormat = TextureFormatInfo::findOGLFormat(
                    QOpenGLTexture::TextureFormat(header.glInternalFormat),
                    QOpenGLTexture::PixelFormat(header.glFormat),
                    QOpenGLTexture::PixelType(header.glType)).format();

            // Ok, can't find in our formats, try to use extended table
        if (textureFormat == TextureFormat::Invalid) {
            textureFormat = findFormat(header.glInternalFormat, header.glFormat, header.glType);
        }
    }

    if (textureFormat == TextureFormat::Invalid) {
        qCWarning(ktxhandler) << "Can't find appropriate format";
        return false;
    }

    const auto size = TextureSize(
                header.pixelWidth,
                std::max<int>(1, header.pixelHeight),
                std::max<int>(1, header.pixelDepth));
    const auto faces = std::max<int>(1, header.numberOfFaces);
    const auto levels = std::max<int>(1, header.numberOfMipmapLevels);
    const auto layers = std::max<int>(1, header.numberOfArrayElements);

    auto result = Texture(
                textureFormat,
                size,
                {levels, layers},
                Texture::Alignment::Word);
    if (result.isNull()) {
        qCWarning(ktxhandler) << "Can't create texture";
        return false;
    }

    for (int level = 0; level < levels; ++level) {
        quint32 imageSize = 0;
        s >> imageSize;
        if (s.status() != QDataStream::Ok) {
            qCWarning(ktxhandler) << "Can't read image size: data stream status =" << s.status();
            return false;
        }

        for (int layer = 0; layer < layers; ++layer) {
            for (int face = 0; face < faces; ++face) {
                const auto data = result.imageData({Texture::Side(face), level, layer});
                const auto read = device()->read(reinterpret_cast<char *>(data.data()), data.size());
                if (read != data.size()) {
                    qCWarning(ktxhandler) << "Can't read from device:"
                                          << device()->errorString();
                    return false;
                }
                readPadding(device(), 3 - ((device()->pos() + 3) % 4));
            }
        }
        readPadding(device(), 3 - ((device()->pos() + 3) % 4));
    }

    texture = std::move(result);

    return true;
}

Q_LOGGING_CATEGORY(ktxhandler, "plugins.textureformats.ktxhandler")
