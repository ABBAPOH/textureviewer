#ifndef TEXTURE_H
#define TEXTURE_H

#include "texturelib_global.h"

#include <TextureLib/TextureFormat>
#include <TextureLib/TextureIOResult>

#include <QtCore/QDataStream>
#include <QtCore/QLoggingCategory>

#include <QtGui/QImage>

#include <gsl/span>

class TextureData;

class TEXTURELIB_EXPORT Texture
{
    Q_GADGET
public:
    enum class Side {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5,
    };
    Q_ENUM(Side)

    enum class Alignment {
        Byte = 1, // 1-byte alignment
        Word = 4, // 4-bytes alignment
    };

    enum class IsCubemap {
        No = 0,
        Yes
    };

    class Size
    {
    public:
        inline constexpr Size() noexcept = default;
        inline constexpr Size(int width) noexcept
            : m_width(width), m_height(1), m_depth(1) {}
        inline constexpr Size(int width, int height) noexcept
            : m_width(width), m_height(height), m_depth(1) {}
        inline constexpr Size(QSize size) noexcept
            : m_width(size.width()), m_height(size.height()), m_depth(1) {}
        inline constexpr Size(int width, int height, int depth) noexcept
            : m_width(width), m_height(height), m_depth(depth) {}

        constexpr bool isNull() const noexcept { return !m_width && !m_height && !m_depth; }
        constexpr bool isValid() const noexcept { return m_width > 0 && m_height > 0 && m_depth > 0; }

        inline constexpr int width() const noexcept { return m_width; }
        inline constexpr void setWidth(int w) noexcept { m_width = w; }

        inline constexpr int height() const { return m_height; }
        inline constexpr void setHeight(int h) noexcept { m_height = h; }

        inline constexpr int depth() const { return m_depth; }
        inline constexpr void setDepth(int d) noexcept { m_depth = d; }

    private:
        int m_width {0};
        int m_height {0};
        int m_depth {0};
    };

    class ArraySize
    {
    public:
        inline constexpr ArraySize() noexcept {}
        inline constexpr ArraySize(int levels, int layers = 1) noexcept
            : m_faces(1)
            , m_levels(levels)
            , m_layers(layers)
        {}
        inline constexpr ArraySize(IsCubemap isCumemap, int levels = 1, int layers = 1) noexcept
            : m_faces(isCumemap == IsCubemap::Yes ? 6 : 1)
            , m_levels(levels)
            , m_layers(layers)
        {}

        constexpr bool isNull() const noexcept { return !m_faces && !m_levels && !m_layers; }
        constexpr bool isValid() const noexcept { return m_faces > 0 && m_levels > 0 && m_layers > 0; }

        inline constexpr int faces() const noexcept { return m_faces; }

        inline constexpr bool isCubemap() const noexcept { return m_faces == 6; }
        inline constexpr void setIsCubemap(bool isCumemap) noexcept { m_faces = isCumemap ? 6 : 1; }
        inline constexpr void setIsCubemap(IsCubemap isCumemap) noexcept
        {
            m_faces = isCumemap == IsCubemap::Yes ? 6 : 1;
        }

        inline constexpr int levels() const noexcept { return m_levels; }
        inline constexpr void setLevels(int levels) noexcept { m_levels = levels; }

        inline constexpr int layers() const noexcept { return m_layers; }
        inline constexpr void setLayers(int layers) noexcept { m_layers = layers; }

    private:
        int m_faces {0};
        int m_levels {0};
        int m_layers {0};
    };

    class ArrayIndex
    {
    public:
        using Side = Texture::Side;

        inline constexpr ArrayIndex(int level = 0, int layer = 0) noexcept
            : m_level(level)
            , m_layer(layer)
        {}
        inline constexpr ArrayIndex(Side side, int level = 0, int layer = 0) noexcept
            : m_face(int(side))
            , m_level(level)
            , m_layer(layer)
        {}

        constexpr bool isNull() const noexcept { return !m_face && !m_level && !m_layer; }
        constexpr bool isValid() const noexcept { return m_face >= 0 && m_level >= 0 && m_layer >= 0; }

        inline constexpr Side side() const noexcept { return Side(m_face); }
        inline constexpr void setSide(Side side) noexcept { m_face = int(side); }

        inline constexpr int face() const { return m_face; }

        inline constexpr int level() const noexcept { return m_level; }
        inline constexpr void setLevel(int level) noexcept { m_level = level; }

        inline constexpr int layer() const noexcept { return m_layer; }
        inline constexpr void setLayer(int layer) noexcept { m_layer = layer; }

    private:
        int m_face {0};
        int m_level {0};
        int m_layer {0};
    };

    using Data = gsl::span<uchar>;
    using ConstData = gsl::span<const uchar>;

    using DataDeleter = std::function<void(uchar[])>;

    inline Texture() noexcept {}
    Texture(const Texture &other);
    inline Texture(Texture &&other) noexcept
    { qSwap(d, other.d); }

    explicit Texture(const QString &file);
    explicit Texture(QStringView file);
    explicit Texture(const QImage& image);

    Texture(TextureFormat format,
            Size size,
            ArraySize dimensions = {1, 1},
            Alignment align = Alignment::Byte);

    Texture(Data data,
            TextureFormat format,
            Size size,
            ArraySize dimensions = {1, 1},
            Alignment align = Alignment::Byte);

    Texture(Data data,
            DataDeleter deleter,
            TextureFormat format,
            Size size,
            ArraySize dimensions = {1, 1},
            Alignment align = Alignment::Byte);

    ~Texture();

    Texture &operator=(const Texture &other);
    inline Texture &operator=(Texture &&other) noexcept
    { qSwap(d, other.d); return *this; }

    static qsizetype calculateBytesPerLine(TextureFormat format, int width, Alignment align = Alignment::Byte);
    static qsizetype calculateBytesPerSlice(TextureFormat format, int width, int height, Alignment align = Alignment::Byte);

    bool isNull() const;
    TextureFormat format() const;

    Alignment alignment() const;

    bool isCompressed() const;

    // ok, opengl does not support really huge textures, so int will be enough
    // (although they use uint32 for sizes)
    int width() const;
    int width(int level) const;
    int height() const;
    int height(int level) const;
    int depth() const;
    int depth(int level) const;
    Size size() const;
    Size size(int level) const;
    int faces() const;
    int levels() const;
    int layers() const;
    ArraySize arraySize() const;

    qsizetype bytes() const;
    qsizetype bitsPerTexel() const;
    qsizetype bytesPerLine(int level = 0) const;
    qsizetype bytesPerSlice(int level = 0) const;
    qsizetype bytesPerImage(int level = 0) const;
    qsizetype offset(ArrayIndex index) const;

    Data imageData(ArrayIndex index);
    ConstData imageData(ArrayIndex index) const;
    ConstData constImageData(ArrayIndex index) const;

    inline Data data() { return {dataImpl(0, 0, 0), bytes()}; }
    inline ConstData data() const { return {dataImpl(0, 0, 0), bytes()}; }
    inline ConstData constData() const { return {dataImpl(0, 0, 0), bytes()}; }

    // TODO: do we need a texel API?
    // we can't return a pointer to the texels as they may not be aligned even to char size
    // (i.e. packed bitmap - an array of bits, 1 bit for each texel)
    // So, we should return a Color object
    // But QColor is too limited, we need a ColorVariant (AnyColor as proposed by Marc Mutz)

    // AnyColor texelColor(const Position &p, const Index &index);
    // AnyColor texelColor(const Position &p, const Index &index) const;
    // AnyColor constTexelColor(const Position &p, const Index &index) const;
    // void setTexelColor(const Position &p, const AnyColor &color);
    // void setTexelColor(const Position &p, const Index &index, const AnyColor &color);

    // Ok, KTX really have different alignment (4) rather than other (dds, vtf) formats (1).
    // So, we can have a usage of a scanline API.
    // However, the same can be achieved via manual iteration over imageData, hovewer that can
    // lead to a bunch of a boilerplate code - see convert(TextureFormat, Alignment) function.

    // Data lineData(const Position &p, const Index &index);
    // ConstData lineData(const Position &p, const Index &index) const;
    // ConstData constLineData(const Position &p, const Index &index) const;

    Texture convert(Alignment align) const;
    Texture convert(TextureFormat format) const;
    Texture convert(TextureFormat format, Alignment align) const;
    Texture copy() const;

    QImage toImage() const;

    TextureIOResult save(const QString &file);
    inline TextureIOResult save(QStringView file) { return save(file.toString()); }
    TextureIOResult load(const QString &file);
    inline TextureIOResult load(QStringView file) { return load(file.toString()); }

private:
    explicit Texture(TextureData *dd);

    void detach();
    bool isDetached() const;

    uchar *dataImpl(int side, int level, int layer);
    const uchar *dataImpl(int side, int level, int layer) const;

    TextureData *d {nullptr};

    friend bool TEXTURELIB_EXPORT operator==(const Texture &lhs, const Texture &rhs);
    friend bool TEXTURELIB_EXPORT operator!=(const Texture &lhs, const Texture &rhs);

    friend QDataStream TEXTURELIB_EXPORT &operator <<(QDataStream &stream, const Texture &texture);
    friend QDataStream TEXTURELIB_EXPORT &operator >>(QDataStream &stream, Texture &texture);
};

bool TEXTURELIB_EXPORT operator==(const Texture &lhs, const Texture &rhs);
bool TEXTURELIB_EXPORT operator!=(const Texture &lhs, const Texture &rhs);

QDataStream TEXTURELIB_EXPORT &operator<<(QDataStream &stream, const Texture &texture);
QDataStream TEXTURELIB_EXPORT &operator>>(QDataStream &stream, Texture &texture);

Q_DECLARE_LOGGING_CATEGORY(texture)

QDebug operator<<(QDebug &d, const Texture::ArrayIndex &index);

QString toQString(Texture::Side side);

#endif // TEXTURE_H
