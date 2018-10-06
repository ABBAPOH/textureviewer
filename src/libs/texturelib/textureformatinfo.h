#ifndef TEXELFORMAT_H
#define TEXELFORMAT_H

#include "texturelib_global.h"

#include <TextureLib/TextureFormat>

#include <QtGui/QOpenGLTexture>

#include <gsl/span>

class TEXTURELIB_EXPORT TextureFormatInfo
{
    Q_GADGET
public:
    enum class Type {
        None,
        Float,
        SignedNormalized,
        UnsignedNormalized,
        SignedInteger,
        UnsignedInteger,
        Srgb,
        Compressed
    };
    Q_ENUMS(Type);

    constexpr inline TextureFormatInfo() noexcept = default;
    constexpr inline TextureFormatInfo(
            TextureFormat format,
            Type type,
            int bytesPerTexel,
            int blockSize,
            QOpenGLTexture::TextureFormat oglTextureFormat,
            QOpenGLTexture::PixelFormat oglPixelFormat = QOpenGLTexture::PixelFormat::NoSourceFormat,
            QOpenGLTexture::PixelType oglPixelType = QOpenGLTexture::PixelType::NoPixelType) noexcept
        : m_format(format)
        , m_type(type)
        , m_bytesPerTexel(bytesPerTexel)
        , m_blockSize(blockSize)
        , m_oglTextureFormat(oglTextureFormat)
        , m_oglPixelFormat(oglPixelFormat)
        , m_oglPixelType(oglPixelType)
    {}

    constexpr inline TextureFormat format() const noexcept { return m_format; }
    constexpr inline Type type() const noexcept { return m_type; }
    constexpr inline int bytesPerTexel() const noexcept { return m_bytesPerTexel; }
    constexpr inline int blockSize() const noexcept { return m_blockSize; }

    constexpr inline QOpenGLTexture::TextureFormat oglTextureFormat() const noexcept {
        return m_oglTextureFormat;
    }

    constexpr inline QOpenGLTexture::PixelFormat oglPixelFormat() const noexcept {
        return m_oglPixelFormat;
    }

    constexpr inline QOpenGLTexture::PixelType oglPixelType() const noexcept {
        return m_oglPixelType;
    }

    constexpr inline bool isCompressed() const noexcept { return m_blockSize != 0; }

    using TextureFormatInfos = gsl::span<const TextureFormatInfo>;

    static const TextureFormatInfo & formatInfo(TextureFormat format) noexcept;
    static TextureFormatInfos allFormatInfos() noexcept;

    static const TextureFormatInfo &findOGLFormat(
            QOpenGLTexture::TextureFormat textureFormat,
            QOpenGLTexture::PixelFormat pixelFormat = QOpenGLTexture::PixelFormat::NoSourceFormat,
            QOpenGLTexture::PixelType pixelType = QOpenGLTexture::PixelType::NoPixelType) noexcept;

private:
    TextureFormat m_format {TextureFormat::Invalid};
    Type m_type {Type::None};
    int m_bytesPerTexel {0};
    int m_blockSize {0};
    QOpenGLTexture::TextureFormat m_oglTextureFormat {QOpenGLTexture::TextureFormat::NoFormat};
    QOpenGLTexture::PixelFormat m_oglPixelFormat {QOpenGLTexture::PixelFormat::NoSourceFormat};
    QOpenGLTexture::PixelType m_oglPixelType {QOpenGLTexture::PixelType::NoPixelType};
};

constexpr inline bool operator==(const TextureFormatInfo &lhs, const TextureFormatInfo &rhs)
{
    return lhs.format() == rhs.format()
            && lhs.bytesPerTexel() == rhs.bytesPerTexel()
            && lhs.blockSize() == rhs.blockSize()
            && lhs.oglTextureFormat() == rhs.oglTextureFormat()
            && lhs.oglPixelFormat() == rhs.oglPixelFormat()
            && lhs.oglPixelType() == rhs.oglPixelType();
}

constexpr inline bool operator!=(const TextureFormatInfo &lhs, const TextureFormatInfo &rhs)
{
    return !(lhs == rhs);
}

Q_DECLARE_METATYPE(TextureFormatInfo::Type)

#endif // TEXELFORMAT_H
