#include "utils.h"
#include "textureformatinfo.h"

#include <TextureLib/Texture>

#include <QtGui/QOpenGLPixelTransferOptions>
#include <QtGui/QOpenGLTexture>

#include <QtCore/QDebug>

QOpenGLTexture::Target getTarget(const Texture &texture)
{
    if (texture.layers() > 1) {
        if (texture.faces() > 1)
            return QOpenGLTexture::Target::TargetCubeMapArray;
        if (texture.height() > 1)
            return QOpenGLTexture::Target::Target2DArray;

        return QOpenGLTexture::Target::Target1DArray;
    } else {
        if (texture.faces() > 1)
            return QOpenGLTexture::Target::TargetCubeMap;
        if (texture.depth() > 1)
            return QOpenGLTexture::Target::Target3D;
        if (texture.height() > 1)
            return QOpenGLTexture::Target::Target2D;

        return QOpenGLTexture::Target::Target1D;
    }
    Q_UNREACHABLE();
}

QOpenGLTexture::CubeMapFace getFace(Texture::Side side)
{
    switch (side) {
    case Texture::Side::PositiveX:
        return QOpenGLTexture::CubeMapPositiveX;
    case Texture::Side::NegativeX:
        return QOpenGLTexture::CubeMapNegativeX;
    case Texture::Side::PositiveY:
        return QOpenGLTexture::CubeMapPositiveY;
    case Texture::Side::NegativeY:
        return QOpenGLTexture::CubeMapNegativeY;
    case Texture::Side::PositiveZ:
        return QOpenGLTexture::CubeMapPositiveZ;
    case Texture::Side::NegativeZ:
        return QOpenGLTexture::CubeMapNegativeZ;
    }
    Q_UNREACHABLE();
}

std::unique_ptr<QOpenGLTexture> Utils::makeOpenGLTexture(const Texture &texture)
{
    if (texture.isNull()) {
        qWarning() << "Can't bind invalid texture";
        return nullptr;
    }

    const auto target = getTarget(texture);
    const auto &texelFormat = TextureFormatInfo::formatInfo(texture.format());
    const auto textureFormat = texelFormat.oglTextureFormat();
    const auto pixelFormat = texelFormat.oglPixelFormat();
    const auto pixelType = texelFormat.oglPixelType();
    const auto options = std::make_unique<QOpenGLPixelTransferOptions>();
    options->setAlignment(int(texture.alignment()));

    auto result = std::make_unique<QOpenGLTexture>(target);
    result->setFormat(textureFormat);
    result->setSize(texture.width(), texture.height(), texture.depth());
    result->setMipLevels(texture.levels());
    if (texture.layers() > 1)
        result->setLayers(texture.layers());
    result->allocateStorage();
    if (!texture.isCompressed()) {
        if (texture.faces() > 1) {
            for (int level = 0; level < texture.levels(); ++level) {
                for (int layer = 0; layer < texture.layers(); ++layer) {
                    for (int face = 0; face < 6; ++face) {
                        const auto image = texture.imageData({Texture::Side(face), level, layer});
                        result->setData(
                                    level,
                                    layer,
                                    getFace(Texture::Side(face)),
                                    pixelFormat,
                                    pixelType,
                                    image.data(),
                                    options.get());
                    }
                }
            }
        } else {
            for (int level = 0; level < texture.levels(); ++level) {
                for (int layer = 0; layer < texture.layers(); ++layer) {
                    const auto image = texture.imageData({level, layer});
                    result->setData(
                                level,
                                layer,
                                pixelFormat,
                                pixelType,
                                image.data(),
                                options.get());
                }
            }
        }
        if (result->mipLevels() == 1) {
            result->generateMipMaps();
        }
    } else {
        if (texture.faces() > 1) {
            for (int level = 0; level < texture.levels(); ++level) {
                for (int layer = 0; layer < texture.layers(); ++layer) {
                    for (int face = 0; face < 6; ++face) {
                        const auto data = texture.imageData({Texture::Side(face), level, layer});
                        result->setCompressedData(
                                    level,
                                    layer,
                                    getFace(Texture::Side(face)),
                                    int(data.size()),
                                    data.data());
                    }
                }
            }
        } else {
            for (int level = 0; level < texture.levels(); ++level) {
                for (int layer = 0; layer < texture.layers(); ++layer) {
                    const auto data = texture.imageData({level, layer});
                    result->setCompressedData(
                                level,
                                layer,
                                int(data.size()),
                                data.data());
                }
            }
        }
    }

    return result;
}
