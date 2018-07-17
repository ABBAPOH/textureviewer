#include "utils.h"
#include "texelformat.h"

QOpenGLTexture::Target convertType(const Texture &texture)
{
    switch (texture.type()) {
    case Texture::Type::None:
        Q_UNREACHABLE();
        break;
    case Texture::Type::Texture1D:
        return QOpenGLTexture::Target::Target1D;
    case Texture::Type::Texture1DArray:
        return QOpenGLTexture::Target::Target1DArray;
    case Texture::Type::Texture2D:
        return QOpenGLTexture::Target::Target2D;
    case Texture::Type::Texture2DArray:
        return QOpenGLTexture::Target::Target2DArray;
    case Texture::Type::Texture3D:
        return QOpenGLTexture::Target::Target3D;
    case Texture::Type::TextureCubeMap:
        return QOpenGLTexture::Target::TargetCubeMap;
    case Texture::Type::TextureCubeMapArray:
        return QOpenGLTexture::Target::TargetCubeMapArray;
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

    const auto target = convertType(texture);
    const auto &texelFormat = TexelFormat::texelFormat(texture.format());
    const auto textureFormat = texelFormat.oglTextureFormat();
    const auto pixelFormat = texelFormat.oglPixelFormat();
    const auto pixelType = texelFormat.oglPixelType();

    auto result = std::make_unique<QOpenGLTexture>(target);
    result->setFormat(textureFormat);
    result->setSize(texture.width(), texture.height(), texture.depth());
    result->setMipLevels(texture.levels());
    if (texture.layers() > 1)
        result->setLayers(texture.layers());
    result->allocateStorage();
    if (!texture.isCompressed()) {
        if (texture.type() == Texture::Type::TextureCubeMap
                || texture.type() == Texture::Type::TextureCubeMapArray) {
            for (int level = 0; level < texture.levels(); ++level) {
                for (int layer = 0; layer < texture.layers(); ++layer) {
                    for (int face = 0; face < 6; ++face) {
                        result->setData(
                                    level,
                                    layer,
                                    getFace(Texture::Side(face)),
                                    pixelFormat,
                                    pixelType,
                                    texture.imageData({Texture::Side(face), level, layer}).data());
                    }
                }
            }
        } else {
            for (int level = 0; level < texture.levels(); ++level) {
                for (int layer = 0; layer < texture.layers(); ++layer) {
                    result->setData(
                                level,
                                layer,
                                pixelFormat,
                                pixelType,
                                texture.imageData({level, layer}).data());
                }
            }
        }
    } else {
        if (texture.type() == Texture::Type::TextureCubeMap
                || texture.type() == Texture::Type::TextureCubeMapArray) {
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

    if (texture.levels() == 1) {
        result->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
        result->generateMipMaps();
    }

    return result;
}
