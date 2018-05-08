#ifndef TEXTURE_H
#define TEXTURE_H

#include "texturelib_global.h"

#include <QtCore/QDataStream>
#include <QtGui/QImage>

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

    // TODO: add overloads with levels before format? Upd: no levels yet:)
    static Texture create1DTexture(int width, Format format);
    static Texture create1DTextureArray(int width, int layers, Format format);
    static Texture create2DTexture(int width, int height, Format format);
    static Texture create2DTextureArray(int width, int height, int layers, Format format);
    static Texture create3DTexture(int width, int height, int depth, Format format);
    static Texture createCubeMapTexture(int size, Format format);
    static Texture createCubeMapTextureArray(int size, int layers, Format format);

    bool isNull() const;
    Type type() const;
    Format format() const;

    // ok, opengl does not support really huge textures, so int will be enough
    // (although they use uint32 for sizes)
    int width() const;
    int height() const;
    int depth() const;
    int levels() const; // for now, levels are not supported, requires a bit more math
    int layers() const;

    qsizetype bytes() const;
    qsizetype bytesPerTexel() const;
    qsizetype bytesPerLine() const;
    qsizetype bytesPerImage() const;

    uchar *data(int layer = 0);
    const uchar *data(int layer = 0) const;
    inline const uchar *constData(int layer = 0) const
    { return data(layer); }

    uchar *scanLine(int y, int z = 0);
    const uchar *scanLine(int y, int z = 0) const;
    inline const uchar *constScanLine(int y, int z = 0) const
    { return scanLine(y, z); }

    Texture copy() const;

private:
    explicit Texture(TextureData *dd);

    TextureData *d {nullptr};

    friend bool operator==(const Texture &lhs, const Texture &rhs);
    friend bool operator!=(const Texture &lhs, const Texture &rhs);

    friend QDataStream &operator <<(QDataStream &stream, const Texture &texture);
    friend QDataStream &operator >>(QDataStream &stream, Texture &texture);
};

bool operator==(const Texture &lhs, const Texture &rhs);
bool operator!=(const Texture &lhs, const Texture &rhs);

QDataStream &operator <<(QDataStream &stream, const Texture &texture);
QDataStream &operator >>(QDataStream &stream, Texture &texture);

Q_DECLARE_METATYPE(Texture::Type)
Q_DECLARE_METATYPE(Texture::Format)

#endif // TEXTURE_H
