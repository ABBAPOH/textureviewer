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
    inline Texture() noexcept = default;
    Texture(const Texture &other);
    explicit Texture(const QString &file);
    explicit Texture(QStringView file);
    explicit Texture(const QImage& image);
    inline Texture(Texture &&other) noexcept
    { qSwap(d, other.d); }

    Texture &operator=(const Texture &other);
    inline Texture &operator=(Texture &&other) noexcept
    { qSwap(d, other.d); return *this; }

    ~Texture();

    void detach();
    bool isDetached() const;

    enum class IsCubemap {
        No = 0,
        Yes
    };

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

    class Size;
    class Index;

    using Data = gsl::span<uchar>;
    using ConstData = gsl::span<const uchar>;

    static Texture create(TextureFormat format, Size size, IsCubemap isCubemap, int levels = 1, int layers = 1, Alignment align = Alignment::Byte);
    static Texture create(TextureFormat format, Size size, int levels = 1, int layers = 1, Alignment align = Alignment::Byte);

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
    int faces() const;
    int levels() const;
    int layers() const;

    qsizetype bytes() const;
    qsizetype bitsPerTexel() const;
    qsizetype bytesPerLine(int level = 0) const;
    qsizetype bytesPerSlice(int level = 0) const;
    qsizetype bytesPerImage(int level = 0) const;
    qsizetype offset(Index index) const;

    Data imageData(Index index);
    ConstData imageData(Index index) const;
    ConstData constImageData(Index index) const;

    inline Data data() { return {dataImpl(0, 0, 0), bytes()}; }
    inline ConstData data() const { return {dataImpl(0, 0, 0), bytes()}; }
    inline ConstData constData() const { return {dataImpl(0, 0, 0), bytes()}; }

    // TODO: do we need a texel API?
    // we can't return a pointer to the texels as they may not be aligned even to char size
    // (i.e. packed bitmap - an array of bits, 1 bit for each texel)
    // So, we should return a Color object
    // But QColor is too limited, we need a ColorVariant (AnyColor as proposed by Marc Mutz)

    // T texelData(const Position &p);
    // const T texelData(const Position &p) const;
    // const T constTexelData(const Position &p) const;

    // Ok, KTX really have different alignment (4) rather than other (dds, vtf) formats (1).
    // So, we can have a usage of a scanline API.
    // However, the same can be achieved via manual iteration over imageData, hovewer that can
    // lead to a bunch of a boilerplate code. A test need to be written (maybe convert(Alignment)?).
    // TODO: explore if we need this API or it's ok to use imageData instead
    // TODO: replace Position with z/y overloads?

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

class Texture::Size
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

class Texture::Index
{
public:
    using Side = Texture::Side;

    inline constexpr Index(int level = 0, int layer = 0) noexcept
        : m_level(level)
        , m_layer(layer)
    {}
    inline constexpr Index(Texture::Side side, int level = 0, int layer = 0) noexcept
        : m_side(side)
        , m_level(level)
        , m_layer(layer)
    {}

    inline constexpr Side side() const noexcept { return m_side; }
    inline constexpr void setSide(Side side) noexcept { m_side = side; }

    inline constexpr int face() const { return int(m_side); }

    inline constexpr int level() const noexcept { return m_level; }
    inline constexpr void setLevel(int level) noexcept { m_level = level; }

    inline constexpr int layer() const noexcept { return m_layer; }
    inline constexpr void setLayer(int layer) noexcept { m_layer = layer; }

private:
    Side m_side {Side(0)};
    int m_level {0};
    int m_layer {0};
};

QDebug operator<<(QDebug &d, const Texture::Index &index);

QString toQString(Texture::Side side);

#endif // TEXTURE_H
