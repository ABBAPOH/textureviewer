#ifndef TEXTURE_P_H
#define TEXTURE_P_H

#include "texture.h"
#include "textureformatinfo.h"

#include <memory>

class TextureData
{
public:
    TextureData() = default;
    TextureData(const TextureData &other) = delete;
    TextureData(TextureData &&) = delete;
    ~TextureData() noexcept = default;

    TextureData &operator=(const TextureData &) = delete;
    TextureData &operator=(TextureData &&) = delete;

    static TextureData *create(TextureFormat format,
            int width,
            int height,
            int depth,
            bool isCubemap,
            int levels,
            int layers,
            Texture::Alignment align,
            Texture::Data data = Texture::Data(),
            Texture::DataDeleter deleter = Texture::DataDeleter());

    static qsizetype calculateBytesPerLine(
            const TextureFormatInfo &format,
            quint32 width,
            Texture::Alignment align = Texture::Alignment::Byte);
    static qsizetype calculateBytesPerSlice(
            const TextureFormatInfo &format,
            quint32 width,
            quint32 height,
            Texture::Alignment align = Texture::Alignment::Byte);

    int levelWidth(int level) const { return int(std::max(uint(width) >> uint(level), 1u)); }
    int levelHeight(int level) const { return int(std::max(uint(height) >> uint(level), 1u)); }
    int levelDepth(int level) const { return int(std::max(uint(depth) >> uint(level), 1u)); }

    qsizetype bytesPerLine(int level) const { return levelInfos[uint(level)].bytesPerLine; }
    qsizetype bytesPerSlice(int level) const { return levelInfos[uint(level)].bytesPerSlice; }
    qsizetype bytesPerImage(int level) const
    {
        return levelInfos[uint(level)].bytesPerSlice * levelDepth(level);
    }
    qsizetype levelOffset(int level) const { return levelInfos[uint(level)].offset; }
    qsizetype offset(int side, int level, int layer) const;

    static std::function<ColorVariant(Texture::ConstData)> getFormatReader(TextureFormat format);
    static std::function<void(Texture::Data, const ColorVariant &)> getFormatWriter(TextureFormat format);

    QAtomicInt ref {0};
    TextureFormat format {TextureFormat::Invalid};
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
    using DataPointer = std::unique_ptr<uchar[], Texture::DataDeleter>;
    DataPointer data;
};

#endif // TEXTURE_P_H
