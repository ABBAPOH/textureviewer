#ifndef TEXELFORMAT_H
#define TEXELFORMAT_H

#include <TextureLib/Texture>

class TexelFormat
{
public:
    constexpr inline TexelFormat() = default;
    constexpr inline TexelFormat(
            Texture::Format format,
            int bitsPerTexel,
            int blockSize)
        : m_format(format)
        , m_bitsPerTexel(bitsPerTexel)
        , m_blockSize(blockSize)
    {}

    constexpr inline Texture::Format format() const { return m_format; }
    constexpr inline int bitsPerTexel() const { return m_bitsPerTexel; }
    constexpr inline int blockSize() const { return m_blockSize; }

    static const TexelFormat &texelFormat(Texture::Format format);

private:
    Texture::Format m_format {Texture::Format::Invalid};
    int m_bitsPerTexel {0};
    int m_blockSize {0};
};

#endif // TEXELFORMAT_H
