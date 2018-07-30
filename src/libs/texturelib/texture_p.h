#ifndef TEXTURE_P_H
#define TEXTURE_P_H

#include "texture.h"
#include "texelformat.h"

class TextureData
{
public:
    TextureData() = default;
    TextureData(const TextureData &other) = delete;
    TextureData(TextureData &&) = delete;
    ~TextureData();
    TextureData &operator=(const TextureData &) = delete;
    TextureData &operator=(TextureData &&) = delete;

    static TextureData *create(
            Texture::Format format,
            int width,
            int height,
            int depth,
            bool isCubemap,
            int levels,
            int layers,
            Texture::Alignment align);

    static qsizetype calculateBytesPerLine(
            const TexelFormat &format,
            quint32 width,
            Texture::Alignment align = Texture::Alignment::Byte);
    static qsizetype calculateBytesPerSlice(
            const TexelFormat &format,
            quint32 width,
            quint32 height,
            Texture::Alignment align = Texture::Alignment::Byte);

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
    Texture::Format format {Texture::Format::Invalid};
    Texture::Alignment align {Texture::Alignment::Byte};
    bool compressed {false};
    int width {0};
    int height {0};
    int depth {0};
    int faces {0};
    int levels {0};
    int layers {0};

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

#endif // TEXTURE_P_H
