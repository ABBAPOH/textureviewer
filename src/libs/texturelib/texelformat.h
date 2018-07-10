#ifndef TEXELFORMAT_H
#define TEXELFORMAT_H

#include <TextureLib/Texture>

#include <QtGui/QOpenGLTexture>

class TexelFormat
{
public:
    constexpr inline TexelFormat() = default;
    constexpr inline TexelFormat(
            Texture::Format format,
            int bitsPerTexel,
            int blockSize,
            QOpenGLTexture::TextureFormat oglTextureFormat,
            QOpenGLTexture::PixelFormat oglPixelFormat = QOpenGLTexture::PixelFormat::NoSourceFormat,
            QOpenGLTexture::PixelType oglPixelType = QOpenGLTexture::PixelType::NoPixelType)
        : m_format(format)
        , m_bitsPerTexel(bitsPerTexel)
        , m_blockSize(blockSize)
        , m_oglTextureFormat(oglTextureFormat)
        , m_oglPixelFormat(oglPixelFormat)
        , m_oglPixelType(oglPixelType)
    {}

    constexpr inline Texture::Format format() const { return m_format; }
    constexpr inline int bitsPerTexel() const { return m_bitsPerTexel; }
    constexpr inline int blockSize() const { return m_blockSize; }

    constexpr inline QOpenGLTexture::TextureFormat oglTextureFormat() const {
        return m_oglTextureFormat;
    }

    constexpr inline QOpenGLTexture::PixelFormat oglPixelFormat() const {
        return m_oglPixelFormat;
    }

    constexpr inline QOpenGLTexture::PixelType oglPixelType() const {
        return m_oglPixelType;
    }

    static const TexelFormat &texelFormat(Texture::Format format);

private:
    Texture::Format m_format {Texture::Format::Invalid};
    int m_bitsPerTexel {0};
    int m_blockSize {0};
    QOpenGLTexture::TextureFormat m_oglTextureFormat {QOpenGLTexture::TextureFormat::NoFormat};
    QOpenGLTexture::PixelFormat m_oglPixelFormat {QOpenGLTexture::PixelFormat::NoSourceFormat};
    QOpenGLTexture::PixelType m_oglPixelType {QOpenGLTexture::PixelType::NoPixelType};
};

#endif // TEXELFORMAT_H
