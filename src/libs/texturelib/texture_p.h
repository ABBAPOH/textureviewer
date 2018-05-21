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

    inline int levelWidth(int level) const { return std::max(width >> level, 1); }
    inline int levelHeight(int level) const { return std::max(height >> level, 1); }
    inline int levelDepth(int level) const { return std::max(depth >> level, 1); }

    inline qsizetype bytesPerLine(int level) const { return levelInfos[level].bytesPerLine; }
    inline qsizetype bytesPerImage(int level) const
    {
        return levelInfos[level].bytesPerLine * levelHeight(level) * levelDepth(level);
    }
    inline qsizetype levelOffset(int level) const { return levelInfos[level].offset; }

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
        return 32;
    }
    return 0;
}

#endif // TEXTURE_P_H
