#ifndef TEXTURE_P_H
#define TEXTURE_P_H

#include "texture.h"

class TextureData
{
public:
    TextureData() = default;
    TextureData(const TextureData &other) = delete;
    TextureData(TextureData &&) = delete;
    ~TextureData();
    TextureData &operator=(const TextureData &) = delete;
    TextureData &operator=(TextureData &&) = delete;

    static TextureData *create(Texture::Type type,
            Texture::Format format, int width,
            int height,
            int depth, int layers,
            int levels);

    static qsizetype calculateBytesPerLine(Texture::Format format, qsizetype bitsPerTexel, quint32 width, Texture::Alignment align);
    static qsizetype calculateBytesPerSlice(Texture::Format format, qsizetype bytesPerLine, quint32 height);

    inline int levelWidth(int level) const { return std::max(width >> level, 1); }
    inline int levelHeight(int level) const { return std::max(height >> level, 1); }
    inline int levelDepth(int level) const { return std::max(depth >> level, 1); }

    inline qsizetype bytesPerLine(int level) const { return levelInfos[level].bytesPerLine; }
    inline qsizetype bytesPerSlice(int level) const { return levelInfos[level].bytesPerSlice; }
    inline qsizetype bytesPerImage(int level) const
    {
        return levelInfos[level].bytesPerSlice * levelDepth(level);
    }
    inline qsizetype levelOffset(int level) const { return levelInfos[level].offset; }
    qsizetype offset(int side, int level, int layer) const;

    QAtomicInt ref {0};
    Texture::Type type {Texture::Type::None};
    Texture::Format format {Texture::Format::Invalid};
    int width {0};
    int height {0};
    int depth {0};
    int faces {0};
    int levels {0};
    int layers {0};
    int bitsPerTexel {0};

    struct LevelInfo
    {
        qsizetype offset {0};
        qsizetype bytesPerLine {0};
        qsizetype bytesPerSlice {0};
        qsizetype bytes {0};
    };

    std::vector<LevelInfo> levelInfos;

    qsizetype nbytes {0};
    uchar *data {nullptr};
};

inline int bbpForFormat(Texture::Format format)
{
    switch(format) {
    case Texture::Format::Invalid:
    case Texture::Format::FormatsCount:
        Q_UNREACHABLE();
    case Texture::Format::ARGB32:
    case Texture::Format::BGRA_8888:
        return 32;
    case Texture::Format::RGB_888:
        return 24;
    case Texture::Format::DXT1:
        return 8; // block sizein bytes, not a texel size
    case Texture::Format::DXT3:
    case Texture::Format::DXT5:
        return 16; // block size in bytes, not a texel size
    }
    return 0;
}

#endif // TEXTURE_P_H
