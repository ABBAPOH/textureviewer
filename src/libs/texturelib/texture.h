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
    static Texture create1DTexture(Format format, int width);
    static Texture create1DTextureArray(Format format, int width, int layers);
    static Texture create2DTexture(Format format, int width, int height);
    static Texture create2DTextureArray(Format format, int width, int height, int layers);
    static Texture create3DTexture(Format format, int width, int height, int depth);
    static Texture createCubeMapTexture(Format format, int size);
    static Texture createCubeMapTextureArray(Format format, int size, int layers);

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

    inline uchar *data(int level = 0) { return dataImpl(0, 0, level); }
    inline uchar *data(Side side, int level = 0) { return dataImpl(int(side), 0, level); }
    inline uchar *layerData(int layer, int level = 0) { return dataImpl(0, layer, level); }
    inline uchar *layerData(int layer, Side side, int level = 0)
    { return dataImpl(int(side), layer, level); }

    inline uchar *data(int level = 0) const { return dataImpl(0, 0, level); }
    inline uchar *data(Side side, int level = 0) const { return dataImpl(int(side), 0, level); }
    inline uchar *layerData(int layer, int level = 0) const { return dataImpl(0, layer, level); }
    inline uchar *layerData(int layer, Side side, int level = 0) const
    { return dataImpl(int(side), layer, level); }

    inline uchar *constData(int level = 0) const { return dataImpl(0, 0, level); }
    inline uchar *constData(Side side, int level = 0) const
    { return dataImpl(int(side), 0, level); }
    inline uchar *constLayerData(int layer, int level = 0) const
    { return dataImpl(0, layer, level); }
    inline uchar *constLayerData(int layer, Side side, int level = 0) const
    { return dataImpl(int(side), layer, level); }

    uchar *scanLine(int y, int z = 0);
    const uchar *scanLine(int y, int z = 0) const;
    inline const uchar *constScanLine(int y, int z = 0) const
    { return scanLine(y, z); }

    Texture copy() const;

private:
    explicit Texture(TextureData *dd);

    uchar *dataImpl(int side, int layer, int level);
    uchar *dataImpl(int side, int layer, int level) const;

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

#endif // TEXTURE_H
