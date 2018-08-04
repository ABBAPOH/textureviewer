#include "ktxhandler.h"
#include "ktxheader.h"

#include <TextureLib/Texture>
#include <TextureLib/TextureFormatInfo>

namespace {

struct CompressedFormatInfo
{
    QOpenGLTexture::TextureFormat oglFormat {QOpenGLTexture::TextureFormat::NoFormat};
    TextureFormat format {TextureFormat::Invalid};
};

constexpr CompressedFormatInfo compressedInfos[] = {
    // BCn compressed
    { QOpenGLTexture::TextureFormat::RGB_DXT1,              TextureFormat::Bc1Rgb_Unorm  },
    { QOpenGLTexture::TextureFormat::SRGB_DXT1,             TextureFormat::Bc1Rgb_Srgb   },
    { QOpenGLTexture::TextureFormat::RGBA_DXT1,             TextureFormat::Bc1Rgba_Unorm },
    { QOpenGLTexture::TextureFormat::SRGB_Alpha_DXT1,       TextureFormat::Bc1Rgba_Srgb  },
    { QOpenGLTexture::TextureFormat::RGBA_DXT3,             TextureFormat::Bc2_Unorm     },
    { QOpenGLTexture::TextureFormat::SRGB_Alpha_DXT3,       TextureFormat::Bc2_Srgb      },
    { QOpenGLTexture::TextureFormat::RGBA_DXT5,             TextureFormat::Bc3_Unorm     },
    { QOpenGLTexture::TextureFormat::SRGB_Alpha_DXT5,       TextureFormat::Bc3_Srgb      },
    { QOpenGLTexture::TextureFormat::SRGB_Alpha_DXT5,       TextureFormat::Bc4_Snorm     },
    { QOpenGLTexture::TextureFormat::R_ATI1N_UNorm,         TextureFormat::Bc4_Unorm     },
    { QOpenGLTexture::TextureFormat::RG_ATI2N_UNorm,        TextureFormat::Bc5_Unorm     },
    { QOpenGLTexture::TextureFormat::RG_ATI2N_SNorm,        TextureFormat::Bc5_Snorm     },
    { QOpenGLTexture::TextureFormat::RGB_BP_UNSIGNED_FLOAT, TextureFormat::Bc6HUF16      },
    { QOpenGLTexture::TextureFormat::RGB_BP_SIGNED_FLOAT,   TextureFormat::Bc6HSF16      },
    { QOpenGLTexture::TextureFormat::RGB_BP_UNorm,          TextureFormat::Bc7_Unorm     },
    { QOpenGLTexture::TextureFormat::SRGB_BP_UNorm,         TextureFormat::Bc7_Srgb      },

    // other
    { QOpenGLTexture::TextureFormat::RGB8_ETC1,      TextureFormat::RGB8_ETC1      },
    { QOpenGLTexture::TextureFormat::RGB8_ETC2,      TextureFormat::RGB8_ETC2      },
    { QOpenGLTexture::TextureFormat::RGBA8_ETC2_EAC, TextureFormat::RGBA8_ETC2_EAC },
    { QOpenGLTexture::TextureFormat::R11_EAC_UNorm,  TextureFormat::R11_EAC_UNorm  },
    { QOpenGLTexture::TextureFormat::RG11_EAC_UNorm, TextureFormat::RG11_EAC_UNorm },
    { QOpenGLTexture::TextureFormat::R11_EAC_SNorm,  TextureFormat::R11_EAC_SNorm  },
    { QOpenGLTexture::TextureFormat::RG11_EAC_SNorm, TextureFormat::RG11_EAC_SNorm },
    { QOpenGLTexture::TextureFormat::RGB8_PunchThrough_Alpha1_ETC2, TextureFormat::RGB8_PunchThrough_Alpha1_ETC2 },
};

constexpr TextureFormat findCompressedFormat(QOpenGLTexture::TextureFormat format)
{
    const auto infos = gsl::span<const CompressedFormatInfo>(compressedInfos);
    for (const auto &info: infos) {
        if (info.oglFormat == format)
            return info.format;
    }
    return TextureFormat::Invalid;
}

} // namespace

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
    KtxHeader header = {};

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

    TextureFormat textureFormat;
    if (header.glFormat == 0 && header.glType == 0) {
        textureFormat = findCompressedFormat(QOpenGLTexture::TextureFormat(header.glInternalFormat));
    } else {
        qCWarning(ktxhandler) << "Uncompressed formats are no supported yet";
        return false;
    }

    if (textureFormat == TextureFormat::Invalid) {
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
                textureFormat,
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

    texture = std::move(result);

    return true;
}

Q_LOGGING_CATEGORY(ktxhandler, "plugins.textureformats.ktxhandler")
