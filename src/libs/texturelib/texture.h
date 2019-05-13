#ifndef TEXTURE_H
#define TEXTURE_H

#include "texturelib_global.h"

#include <TextureLib/ColorVariant>
#include <TextureLib/TextureFormat>
#include <TextureLib/TextureFormatInfo>
#include <TextureLib/TextureIOResult>

#include <QtCore/QDataStream>
#include <QtCore/QLoggingCategory>

#include <QtGui/QImage>

#include <gsl/span>

#include <Expected>

class TextureData;

class TEXTURELIB_EXPORT Texture
{
    Q_GADGET
public:
    using size_type = qsizetype;
    using usize_type = QIntegerForSizeof<size_type>::Unsigned;

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

    struct Size
    {
    public:
        using size_type = Texture::size_type;

        constexpr Size() noexcept = default;
        constexpr Size(QSize size) noexcept
            : width(size_type(size.width())), height(size_type(size.height())), depth(1) {}
        constexpr Size(size_type width, size_type height = 1, size_type depth = 1) noexcept
            : width(width), height(height), depth(depth) {}

        constexpr bool isNull() const noexcept { return !width && !height && !depth; }
        constexpr bool isValid() const noexcept { return width > 0 && height > 0 && depth > 0; }

        size_type width {0};
        size_type height {0};
        size_type depth {0};
    };

    struct Position
    {
        using size_type = Texture::size_type;

        constexpr Position(size_type x = 0, size_type y = 0, size_type z = 0) noexcept
            : x(x), y(y), z(z) {}
        constexpr Position(QPoint p, size_type z = 0) noexcept
            : x(p.x()), y(p.y()), z(z) {}

        constexpr bool isValid() const noexcept { return x >= 0 && y >= 0 && z >= 0; }

        size_type x {0};
        size_type y {0};
        size_type z {0};
    };

    class ArraySize
    {
    public:
        using size_type = Texture::size_type;

        constexpr ArraySize() noexcept {}
        constexpr ArraySize(size_type levels, size_type layers = 1) noexcept
            : m_faces(1)
            , m_levels(levels)
            , m_layers(layers)
        {}
        constexpr ArraySize(IsCubemap isCumemap, size_type levels = 1, size_type layers = 1) noexcept
            : m_faces(isCumemap == IsCubemap::Yes ? 6 : 1)
            , m_levels(levels)
            , m_layers(layers)
        {}

        constexpr bool isNull() const noexcept { return !m_faces && !m_levels && !m_layers; }
        constexpr bool isValid() const noexcept { return m_faces > 0 && m_levels > 0 && m_layers > 0; }

        constexpr size_type faces() const noexcept { return m_faces; }

        constexpr bool isCubemap() const noexcept { return m_faces == 6; }
        constexpr void setIsCubemap(bool isCumemap) noexcept { m_faces = isCumemap ? 6 : 1; }
        constexpr void setIsCubemap(IsCubemap isCumemap) noexcept
        {
            m_faces = isCumemap == IsCubemap::Yes ? 6 : 1;
        }

        constexpr size_type levels() const noexcept { return m_levels; }
        constexpr void setLevels(size_type levels) noexcept { m_levels = levels; }

        constexpr size_type layers() const noexcept { return m_layers; }
        constexpr void setLayers(size_type layers) noexcept { m_layers = layers; }

    private:
        size_type m_faces {0};
        size_type m_levels {0};
        size_type m_layers {0};
    };

    class ArrayIndex
    {
    public:
        using size_type = Texture::size_type;
        using Side = Texture::Side;

        constexpr ArrayIndex(size_type level = 0, size_type layer = 0) noexcept
            : m_level(level)
            , m_layer(layer)
        {}
        constexpr ArrayIndex(Side side, size_type level = 0, size_type layer = 0) noexcept
            : m_face(size_type(side))
            , m_level(level)
            , m_layer(layer)
        {}

        constexpr bool isNull() const noexcept { return !m_face && !m_level && !m_layer; }
        constexpr bool isValid() const noexcept { return m_face >= 0 && m_level >= 0 && m_layer >= 0; }

        constexpr Side side() const noexcept { return Side(m_face); }
        constexpr void setSide(Side side) noexcept { m_face = size_type(side); }

        constexpr size_type face() const { return m_face; }

        constexpr size_type level() const noexcept { return m_level; }
        constexpr void setLevel(size_type level) noexcept { m_level = level; }

        constexpr size_type layer() const noexcept { return m_layer; }
        constexpr void setLayer(size_type layer) noexcept { m_layer = layer; }

    private:
        size_type m_face {0};
        size_type m_level {0};
        size_type m_layer {0};
    };

    using Data = gsl::span<uchar>;
    using ConstData = gsl::span<const uchar>;

    using DataDeleter = std::function<void(uchar[])>;

    Texture() noexcept {}
    Texture(const Texture &other);
    Texture(Texture &&other) noexcept
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
    Texture &operator=(Texture &&other) noexcept
    { qSwap(d, other.d); return *this; }

    static qsizetype calculateBytesPerLine(TextureFormat format, size_type width, Alignment align = Alignment::Byte);
    static qsizetype calculateBytesPerSlice(TextureFormat format, size_type width, size_type height, Alignment align = Alignment::Byte);

    bool isNull() const;
    TextureFormat format() const;
    const TextureFormatInfo &formatInfo() const;

    Alignment alignment() const;

    bool isCompressed() const;

    size_type width(size_type level = 0) const;
    size_type height(size_type level = 0) const;
    size_type depth(size_type level = 0) const;
    Size size(size_type level = 0) const;
    size_type faces() const;
    size_type levels() const;
    size_type layers() const;
    ArraySize arraySize() const;

    qsizetype bytes() const;
    qsizetype bytesPerTexel() const;
    qsizetype bytesPerLine(size_type level = 0) const;
    qsizetype bytesPerSlice(size_type level = 0) const;
    qsizetype bytesPerImage(size_type level = 0) const;

    Data imageData(ArrayIndex index);
    ConstData imageData(ArrayIndex index) const;
    ConstData constImageData(ArrayIndex index) const;

    Data data() { return {dataImpl(0, 0, 0), bytes()}; }
    ConstData data() const { return {dataImpl(0, 0, 0), bytes()}; }
    ConstData constData() const { return {dataImpl(0, 0, 0), bytes()}; }

    ColorVariant texelColor(Position p, ArrayIndex index) const;
    void setTexelColor(const Position &p, const ColorVariant &color)
    { setTexelColor(p, {}, color); }
    void setTexelColor(Position p, ArrayIndex index, const ColorVariant &color);

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
    static gsl::span<const TextureFormat> supportedConvertions();

    Texture copy() const;

    QImage toImage() const;

    TextureIOResult save(const QString &file);
    TextureIOResult save(QStringView file) { return save(file.toString()); }
    using ReadResult = Expected<Texture, TextureIOError>;
    static ReadResult load(const QString &file);
    static ReadResult load(QStringView file) { return load(file.toString()); }

private:
    explicit Texture(TextureData *dd);

    void detach();
    bool isDetached() const;

    uchar *dataImpl(size_type side, size_type level, size_type layer);
    const uchar *dataImpl(size_type side, size_type level, size_type layer) const;

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
