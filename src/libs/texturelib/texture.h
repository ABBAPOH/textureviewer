#ifndef TEXTURE_H
#define TEXTURE_H

#include "texturelib_global.h"

#include <QtCore/QLoggingCategory>
#include <QtCore/QDataStream>
#include <QtGui/QImage>

#include <tuple>

class TextureData;

class TEXTURELIB_EXPORT Texture
{
public:
    Texture();
    Texture(const Texture &other);
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

    using Point3D = std::tuple<int, int, int>;

    static Texture create1DTexture(Format format, int width, int levels = 1, int layers = -1);
    static Texture create2DTexture(Format format, int width, int height, int levels = 1, int layers = -1);
    static Texture create3DTexture(Format format, int width, int height, int depth, int levels = 1);
    static Texture createCubeMapTexture(Format format, int size, int levels = 1, int layers = -1);

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
    int levels() const; // for now, levels are not supported, requires a bit more math
    int layers() const;

    qsizetype bytes() const;
    qsizetype bytesPerTexel() const;
    qsizetype bytesPerLine(int level = 0) const;
    qsizetype bytesPerLevel(int level = 0) const;
    qsizetype levelOffset(int level = 0) const;

    inline uchar *data(int level = 0, int layer = 0) { return dataImpl(0, level, layer); }
    inline uchar *data(Side side, int level = 0, int layer = 0) { return dataImpl(int(side), level, layer); }

    inline uchar *data(int level = 0, int layer = 0) const { return dataImpl(0, level, layer); }
    inline uchar *data(Side side, int level = 0, int layer = 0) const { return dataImpl(int(side), level, layer); }

    inline uchar *constData(int level = 0, int layer = 0) const { return dataImpl(0, level, layer); }
    inline uchar *constData(Side side, int level = 0, int layer = 0) const { return dataImpl(int(side), level, layer); }

    inline uchar *texelData(int x, int level = 0, int layer = 0)
    { return texelDataImpl(0, x, 0, 0, level, layer); }
    inline uchar *texelData(QPoint p, int level = 0, int layer = 0)
    { return texelDataImpl(0, p.x(), p.y(), 0, level, layer); }
    inline uchar *texelData(Point3D p, int level = 0, int layer = 0)
    { return texelDataImpl(0, std::get<0>(p), std::get<1>(p), std::get<2>(p), level, layer); }
    inline uchar *texelData(Side side, QPoint p, int level = 0, int layer = 0)
    { return texelDataImpl(int(side), p.x(), p.y(), 0, level, layer); }

    inline uchar *texelData(int x, int level = 0, int layer = 0) const
    { return texelDataImpl(0, x, 0, 0, level, layer); }
    inline uchar *texelData(QPoint p, int level = 0, int layer = 0) const
    { return texelDataImpl(0, p.x(), p.y(), 0, level, layer); }
    inline uchar *texelData(Point3D p, int level = 0, int layer = 0) const
    { return texelDataImpl(0, std::get<0>(p), std::get<1>(p), std::get<2>(p), level, layer); }
    inline uchar *texelData(Side side, QPoint p, int level = 0, int layer = 0) const
    { return texelDataImpl(int(side), p.x(), p.y(), 0, level, layer); }

    inline uchar *constTexelData(int x, int level = 0, int layer = 0) const
    { return texelDataImpl(0, x, 0, 0, level, layer); }
    inline uchar *constTexelData(QPoint p, int level = 0, int layer = 0) const
    { return texelDataImpl(0, p.x(), p.y(), 0, level, layer); }
    inline uchar *constTexelData(Point3D p, int level = 0, int layer = 0) const
    { return texelDataImpl(0, std::get<0>(p), std::get<1>(p), std::get<2>(p), level, layer); }
    inline uchar *constTexelData(Side side, QPoint p, int level = 0, int layer = 0) const
    { return texelDataImpl(int(side), p.x(), p.y(), 0, level, layer); }

    Texture copy() const;

private:
    explicit Texture(TextureData *dd);

    uchar *dataImpl(int side, int layer, int level);
    uchar *dataImpl(int side, int level, int layer) const;

    uchar *texelDataImpl(int side, int x, int y, int z, int level, int layer);
    uchar *texelDataImpl(int side, int x, int y, int z, int level, int layer) const;

    TextureData *d {nullptr};

    friend bool operator==(const Texture &lhs, const Texture &rhs);
    friend bool operator!=(const Texture &lhs, const Texture &rhs);

    friend QDataStream &operator <<(QDataStream &stream, const Texture &texture);
    friend QDataStream &operator >>(QDataStream &stream, Texture &texture);
};

bool operator==(const Texture &lhs, const Texture &rhs);
bool operator!=(const Texture &lhs, const Texture &rhs);

QDataStream &operator<<(QDataStream &stream, const Texture &texture);
QDataStream &operator>>(QDataStream &stream, Texture &texture);

Q_DECLARE_METATYPE(Texture::Type)
Q_DECLARE_METATYPE(Texture::Format)

Q_DECLARE_LOGGING_CATEGORY(texture)

#endif // TEXTURE_H
