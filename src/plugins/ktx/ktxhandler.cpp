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

    TexelFormat texelFormat;
    if (header.glFormat == 0) {
        texelFormat = {Texture::Format::Invalid, 0, 0, QOpenGLTexture::TextureFormat(header.glInternalFormat)};
    }

    const auto texelFormats = TexelFormat::texelFormats();

    auto compareFormats = [texelFormat](const TexelFormat &other)
    {
        return other.oglTextureFormat() == texelFormat.oglTextureFormat();
    };
    const auto it = std::find_if(texelFormats.begin(), texelFormats.end(), compareFormats);
    if (it == texelFormats.end()) {
        qCWarning(ktxhandler) << "Can't find appropriate format";
        return false;
    }
    texelFormat = *it;

    const auto size = Texture::Size(
                header.pixelWidth,
                std::max<int>(1, header.pixelHeight),
                std::max<int>(1, header.pixelDepth));
    const auto faces = std::max<int>(1, header.numberOfFaces);
    const auto levels = std::max<int>(1, header.numberOfMipmapLevels);
    const auto layers = std::max<int>(1, header.numberOfArrayElements);

    auto result = Texture::create(Texture::Type::Texture2D, texelFormat.format(), size, levels, layers);
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
            }
        }
    }

    texture = std::move(result);

    return true;
}

Q_LOGGING_CATEGORY(ktxhandler, "pluigns.textureformats.ktxhandler")
