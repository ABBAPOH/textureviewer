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

    int getWidth(int level) const;
    int getHeight(int level) const;
    int getDepth(int level) const;
    qsizetype bytesPerLine(int level) const;
    qsizetype bytesPerImage(int level) const;
    qsizetype levelOffset(int level) const;

    QAtomicInt ref {0};
    Texture::Type type {Texture::Type::None};
    Texture::Format format {Texture::Format::Invalid};
    int width {0};
    int height {0};
    int depth {0};
    int faces {0};
    int levels {0};
    int layers {0};
    int bytesPerTexel {0};

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
