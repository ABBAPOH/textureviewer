#ifndef TEXELFORMAT_H
#define TEXELFORMAT_H

#include "texturelib_global.h"

#include <TextureLib/TextureFormat>

#include <QtGui/QOpenGLTexture>

#include <gsl/span>

class TextureFormatInfo
{
    Q_GADGET
public:
    Q_ENUM(TextureFormat)

    constexpr inline TextureFormatInfo() noexcept = default;
    constexpr inline TextureFormatInfo(
            TextureFormat format,
            int bitsPerTexel,
            int blockSize,
            QOpenGLTexture::TextureFormat oglTextureFormat,
            QOpenGLTexture::PixelFormat oglPixelFormat = QOpenGLTexture::PixelFormat::NoSourceFormat,
            QOpenGLTexture::PixelType oglPixelType = QOpenGLTexture::PixelType::NoPixelType) noexcept
        : m_format(format)
        , m_bitsPerTexel(bitsPerTexel)
        , m_blockSize(blockSize)
        , m_oglTextureFormat(oglTextureFormat)
        , m_oglPixelFormat(oglPixelFormat)
        , m_oglPixelType(oglPixelType)
    {}

    constexpr inline TextureFormat format() const noexcept { return m_format; }
    constexpr inline int bitsPerTexel() const noexcept { return m_bitsPerTexel; }
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

    static const TextureFormatInfo TEXTURELIB_EXPORT & texelFormat(TextureFormat format) noexcept;
    static TextureFormatInfos TEXTURELIB_EXPORT texelFormats() noexcept;

#if defined(Q_OS_LINUX)
    // private method
    static TexelFormat findOGLFormatConst(
            QOpenGLTexture::TextureFormat textureFormat,
            QOpenGLTexture::PixelFormat pixelFormat,
            QOpenGLTexture::PixelType pixelType) noexcept;
#endif // Q_OS_LINUX

    // private method
    static TextureFormatInfo TEXTURELIB_EXPORT findOGLFormatLinear(
            QOpenGLTexture::TextureFormat textureFormat,
            QOpenGLTexture::PixelFormat pixelFormat,
            QOpenGLTexture::PixelType pixelType) noexcept;

    static inline TextureFormatInfo findOGLFormat(
            QOpenGLTexture::TextureFormat textureFormat,
            QOpenGLTexture::PixelFormat pixelFormat = QOpenGLTexture::PixelFormat::NoSourceFormat,
            QOpenGLTexture::PixelType pixelType = QOpenGLTexture::PixelType::NoPixelType) noexcept
    {
#if defined(Q_OS_LINUX)
        return findOGLFormatConst(textureFormat, pixelFormat, pixelType);
#else
        return findOGLFormatLinear(textureFormat, pixelFormat, pixelType);
#endif // Q_OS_LINUX
    }

private:
    TextureFormat m_format {TextureFormat::Invalid};
    int m_bitsPerTexel {0};
    int m_blockSize {0};
    QOpenGLTexture::TextureFormat m_oglTextureFormat {QOpenGLTexture::TextureFormat::NoFormat};
    QOpenGLTexture::PixelFormat m_oglPixelFormat {QOpenGLTexture::PixelFormat::NoSourceFormat};
    QOpenGLTexture::PixelType m_oglPixelType {QOpenGLTexture::PixelType::NoPixelType};
};

constexpr inline bool operator==(const TextureFormatInfo &lhs, const TextureFormatInfo &rhs)
{
    return lhs.format() == rhs.format()
            && lhs.bitsPerTexel() == rhs.bitsPerTexel()
            && lhs.blockSize() == rhs.blockSize()
            && lhs.oglTextureFormat() == rhs.oglTextureFormat()
            && lhs.oglPixelFormat() == rhs.oglPixelFormat()
            && lhs.oglPixelType() == rhs.oglPixelType();
}

constexpr inline bool operator!=(const TextureFormatInfo &lhs, const TextureFormatInfo &rhs)
{
    return !(lhs == rhs);
}

#endif // TEXELFORMAT_H
