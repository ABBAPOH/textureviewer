#ifndef TEXTURE_P_H
#define TEXTURE_P_H

#include "texture.h"
#include "textureformatinfo.h"

#include <memory>

class TextureData
{
public:
    using size_type = Texture::size_type;
    using usize_type = Texture::usize_type;

    TextureData() = default;
    TextureData(const TextureData &other) = delete;
    TextureData(TextureData &&) = delete;
    ~TextureData() noexcept = default;

    TextureData &operator=(const TextureData &) = delete;
    TextureData &operator=(TextureData &&) = delete;

    static TextureData *create(TextureFormat format,
            size_type width,
            size_type height,
            size_type depth,
            bool isCubemap,
            size_type levels,
            size_type layers,
            Texture::Alignment align,
            Texture::Data data = Texture::Data(),
            Texture::DataDeleter deleter = Texture::DataDeleter());

    static std::size_t calculateBytesPerLine(const TextureFormatInfo &format,
            usize_type uwidth,
            Texture::Alignment align = Texture::Alignment::Byte);
    static std::size_t calculateBytesPerSlice(const TextureFormatInfo &format,
            usize_type width,
            usize_type height,
            Texture::Alignment align = Texture::Alignment::Byte);

    size_type levelWidth(size_type level) const
    { return size_type(std::max(usize_type(width) >> usize_type(level), usize_type(1))); }
    size_type levelHeight(size_type level) const
    { return size_type(std::max(usize_type(height) >> usize_type(level), usize_type(1))); }
    size_type levelDepth(size_type level) const
    { return size_type(std::max(usize_type(depth) >> usize_type(level), usize_type(1))); }

    qsizetype bytesPerLine(size_type level) const { return levelInfos[usize_type(level)].bytesPerLine; }
    qsizetype bytesPerSlice(size_type level) const { return levelInfos[usize_type(level)].bytesPerSlice; }
    qsizetype bytesPerImage(size_type level) const
    {
        return levelInfos[usize_type(level)].bytesPerSlice * levelDepth(level);
    }
    qsizetype levelOffset(size_type level) const { return levelInfos[uint(level)].offset; }
    qsizetype offset(size_type side, size_type level, size_type layer) const;

    static std::function<ColorVariant(Texture::ConstData)> getFormatReader(TextureFormat format);
    static std::function<void(Texture::Data, const ColorVariant &)> getFormatWriter(TextureFormat format);

    QAtomicInt ref {0};
    TextureFormat format {TextureFormat::Invalid};
    Texture::Alignment align {Texture::Alignment::Byte};
    bool compressed {false};
    size_type width {0};
    size_type height {0};
    size_type depth {0};
    size_type faces {0};
    size_type levels {0};
    size_type layers {0};

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
