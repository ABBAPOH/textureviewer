#include "ktxhandler.h"
#include "ktxheader.h"

#include <TextureLib/TexelFormat>

static bool readPadding(KtxHandler::QIODevicePointer device, qint64 size)
{
    if (size == 0)
        return true;

    const auto buffer = std::make_unique<char[]>(size_t(size));
    const auto read = device->read(buffer.get(), size);
    if (read != size) {
        qCWarning(ktxhandler) << "Can't read padding of size" << size
                              << ":" << device->errorString();
        return false;
    }
    return true;
}

bool KtxHandler::read(Texture& texture)
{
    KtxHeader header;

    QDataStream s(device().get());
    s >> header;

    qCDebug(ktxhandler) << "header:" << header;

    if (s.status() != QDataStream::Ok) {
        qCWarning(ktxhandler) << "Can't read header: data stream status =" << s.status();
        return false;
    }

    if (!readPadding(device(), header.bytesOfKeyValueData))
        return false;

    readPadding(device(), 3 - ((device()->pos() + 3) % 4));

    TexelFormat texelFormat;
    if (header.glFormat == 0 && header.glType == 0) {
        texelFormat = TexelFormat::findOGLFormat(
                    QOpenGLTexture::TextureFormat(header.glInternalFormat));
    } else {
        texelFormat = TexelFormat::findOGLFormat(
                    QOpenGLTexture::TextureFormat(header.glInternalFormat),
                    QOpenGLTexture::PixelFormat(header.glFormat),
                    QOpenGLTexture::PixelType(header.glType));
    }

    if (texelFormat.format() == Texture::Format::Invalid) {
        qCWarning(ktxhandler) << "Can't find appropriate format";
        return false;
    }

    const auto size = Texture::Size(
                header.pixelWidth,
                std::max<int>(1, header.pixelHeight),
                std::max<int>(1, header.pixelDepth));
    const auto faces = std::max<int>(1, header.numberOfFaces);
    const auto levels = std::max<int>(1, header.numberOfMipmapLevels);
    const auto layers = std::max<int>(1, header.numberOfArrayElements);

    auto result = Texture::create(
                texelFormat.format(),
                size,
                levels,
                layers,
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

    qDebug() << device()->pos() << device()->size();

    texture = std::move(result);

    return true;
}

Q_LOGGING_CATEGORY(ktxhandler, "pluigns.textureformats.ktxhandler")
