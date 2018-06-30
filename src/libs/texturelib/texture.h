#ifndef TEXTURE_H
#define TEXTURE_H

#include "texturelib_global.h"

#include <TextureLib/TextureIOResult>

#include <QtCore/QLoggingCategory>
#include <QtCore/QDataStream>
#include <QtGui/QImage>

#include <gsl/span>

#include <tuple>

class TextureData;

class TEXTURELIB_EXPORT Texture
{
public:
    Texture() noexcept;
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

    enum class Type {
        None = 0,
        Texture1D,
        Texture1DArray,
        Texture2D,
        Texture2DArray,
        Texture3D,
        TextureCubeMap,
        TextureCubeMapArray
    };

    enum class Format {
        Invalid = 0,
        ARGB32,
        RGB_888,
        BGRA_8888,
        FormatsCount // should be the last
    };

    enum class Side {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5,
    };

    class Position;

    using DataSpan = gsl::span<uchar>;
    using ConstDataSpan = gsl::span<const uchar>;

    static Texture create1DTexture(Format format, int width, int levels = 1, int layers = -1);
    static Texture create2DTexture(Format format, int width, int height, int levels = 1, int layers = -1);
    static Texture create3DTexture(Format format, int width, int height, int depth, int levels = 1);
    static Texture createCubeMapTexture(Format format, int size, int levels = 1, int layers = -1);
    static Texture create(Type type, Format format, int width, int height, int depth, int levels = 1, int layers = 1);

    bool isNull() const;
    Type type() const;
    Format format() const;

    // ok, opengl does not support really huge textures, so int will be enough
    // (although they use uint32 for sizes)
    int width() const;
    int width(int level) const;
    int height() const;
    int height(int level) const;
    int depth() const;
    int depth(int level) const;
    int levels() const;
    int layers() const;

    qsizetype bytes() const;
    qsizetype bitsPerTexel() const;
    qsizetype bytesPerLine(int level = 0) const;
    qsizetype bytesPerImage(int level = 0) const;
    qsizetype Q_DECL_DEPRECATED levelOffset(int level = 0) const;
    qsizetype offset(int level = 0, int layer = 0) const;

    inline uchar *data(int level = 0, int layer = 0) { return dataImpl(0, level, layer); }
    inline uchar *data(Side side, int level = 0, int layer = 0) { return dataImpl(int(side), level, layer); }

    inline const uchar *data(int level = 0, int layer = 0) const { return dataImpl(0, level, layer); }
    inline const uchar *data(Side side, int level = 0, int layer = 0) const { return dataImpl(int(side), level, layer); }

    inline const uchar *constData(int level = 0, int layer = 0) const { return dataImpl(0, level, layer); }
    inline const uchar *constData(Side side, int level = 0, int layer = 0) const { return dataImpl(int(side), level, layer); }

    // TODO: do we need a texel API?
    // we can't return a pointer to the texels as they may not be aligned even to char size
    // (i.e. packed bitmap - an array of bits, 1 bit for each texel)
    // So, we should return a Color object
    // But QColor is too limited, we need a ColorVariant (AnyColor as proposed by Marc Mutz)

    // T texelData(const Position &p);
    // const T texelData(const Position &p) const;
    // const T constTexelData(const Position &p) const;

    // The only reason for the scanLine API (aka lineData) is that textures *possibly* can
    // have different alignments (in OGL, it is 1, 4, 8).
    // TODO: need testing
    // Ok, here's another usecase: converting to QImage, QImage has an alignment of 4,
    // so we forced to fill it line by line even if format is the same
    DataSpan lineData(const Position &p);
    ConstDataSpan lineData(const Position &p) const;
    ConstDataSpan constLineData(const Position &p) const;

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

Q_DECLARE_METATYPE(Texture::Type)
Q_DECLARE_METATYPE(Texture::Format)

Q_DECLARE_LOGGING_CATEGORY(texture)

class Texture::Position
{
public:
    using Side = Texture::Side;

    inline constexpr Position() = default;
    inline constexpr Position(int x, int y, int z) : m_x(x), m_y(y), m_z(z) {}
    inline constexpr Position(Side side, int x, int y) : m_x(x), m_y(y), m_side(side) {}

    inline constexpr int x() const { return m_x; }
    inline constexpr Position &x(int x) { m_x = x; return *this; }
    inline constexpr void setX(int x) { m_x = x; }

    inline constexpr int y() const { return m_y; }
    inline constexpr Position &y(int y) { m_y = y; return *this; }
    inline constexpr void setY(int y) { m_y = y; }

    inline constexpr int z() const { return m_z; }
    inline constexpr Position &z(int z) { m_x = z; return *this; }
    inline constexpr void setZ(int z) { m_z = z; }

    inline constexpr Side side() const { return m_side; }
    inline constexpr Position &side(Side side) { m_side = side; return *this; }
    inline constexpr void setZ(Side side) { m_side = side; }

    inline constexpr int face() const { return int(m_side); }

    inline constexpr int level() const { return m_level; }
    inline constexpr Position &level(int level) { m_level = level; return *this; }
    inline constexpr void setLevel(int level) { m_level = level; }

    inline constexpr int layer() const { return m_layer; }
    inline constexpr Position &layer(int layer) { m_layer = layer; return *this; }
    inline constexpr void setLayer(int layer) { m_layer = layer; }

private:
    int m_x {0};
    int m_y {0};
    int m_z {0};
    Side m_side {Side(0)};
    int m_level {0};
    int m_layer {0};
};

#endif // TEXTURE_H
