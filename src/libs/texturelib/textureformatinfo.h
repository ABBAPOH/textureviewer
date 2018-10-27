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
    Q_ENUM(Type)

    constexpr inline TextureFormatInfo() noexcept = default;
    constexpr inline TextureFormatInfo(
            TextureFormat format,
            Type type,
            int size,
            QOpenGLTexture::TextureFormat oglTextureFormat,
            QOpenGLTexture::PixelFormat oglPixelFormat = QOpenGLTexture::PixelFormat::NoSourceFormat,
            QOpenGLTexture::PixelType oglPixelType = QOpenGLTexture::PixelType::NoPixelType) noexcept
        : m_format(format)
        , m_type(type)
        , m_size(size)
        , m_oglTextureFormat(oglTextureFormat)
        , m_oglPixelFormat(oglPixelFormat)
        , m_oglPixelType(oglPixelType)
    {}

    constexpr inline TextureFormat format() const noexcept { return m_format; }
    constexpr inline Type type() const noexcept { return m_type; }
    constexpr inline int size() const noexcept { return m_size; }

    constexpr inline int bytesPerTexel() const noexcept
    {
        return m_type != Type::Compressed ? m_size : 0;
    }

    constexpr inline int blockSize() const noexcept
    {
        return m_type == Type::Compressed ? m_size : 0;
    }

    constexpr inline QOpenGLTexture::TextureFormat oglTextureFormat() const noexcept {
        return m_oglTextureFormat;
    }

    constexpr inline QOpenGLTexture::PixelFormat oglPixelFormat() const noexcept {
        return m_oglPixelFormat;
    }

    constexpr inline QOpenGLTexture::PixelType oglPixelType() const noexcept {
        return m_oglPixelType;
    }

    constexpr inline bool isCompressed() const noexcept { return m_type == Type::Compressed; }

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
    int m_size {0};
    QOpenGLTexture::TextureFormat m_oglTextureFormat {QOpenGLTexture::TextureFormat::NoFormat};
    QOpenGLTexture::PixelFormat m_oglPixelFormat {QOpenGLTexture::PixelFormat::NoSourceFormat};
    QOpenGLTexture::PixelType m_oglPixelType {QOpenGLTexture::PixelType::NoPixelType};
};

constexpr inline bool operator==(const TextureFormatInfo &lhs, const TextureFormatInfo &rhs)
{
    return lhs.format() == rhs.format()
            && lhs.type() == rhs.type()
            && lhs.size() == rhs.size()
            && lhs.oglTextureFormat() == rhs.oglTextureFormat()
            && lhs.oglPixelFormat() == rhs.oglPixelFormat()
            && lhs.oglPixelType() == rhs.oglPixelType();
}

constexpr inline bool operator!=(const TextureFormatInfo &lhs, const TextureFormatInfo &rhs)
{
    return !(lhs == rhs);
}

QString TEXTURELIB_EXPORT toQString(TextureFormatInfo::Type type);

Q_DECLARE_METATYPE(TextureFormatInfo::Type)

#endif // TEXELFORMAT_H
