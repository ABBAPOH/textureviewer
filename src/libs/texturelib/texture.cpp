#include "texture_p.h"

static inline bool isCubeMap(Texture::Type type)
{
    return type == Texture::Type::TextureCubeMap || type == Texture::Type::TextureCubeMapArray;
}

TextureData::~TextureData()
{
    free(data);
}

TextureData *TextureData::create(
        int width,
        int height,
        int depth,
        int layers,
        int levels,
        Texture::Type type,
        Texture::Format format)
{
    std::unique_ptr<TextureData> data;

    if (width <= 0 || height <= 0 || depth <= 0 || layers <= 0 || format == Texture::Format::Invalid)
        return nullptr; // invalid parameter(s)

    auto uwidth = uint(width);
    auto uheight = uint(height);
    auto udepth = uint(depth);
    auto ufaces = ::isCubeMap(type) ? uint(6) : uint(1);
    auto ulayers = uint(layers);
    auto ubbp = uint(bbpForFormat(format));

    const auto bytesPerLine = ((uwidth * ubbp + 31) >> 5) << 2; // bytes per scanline (must be multiple of 4)

    // sanity check for potential overflows
    if (std::numeric_limits<int>::max() / ubbp / uwidth < 1
        || bytesPerLine <= 0
        || std::numeric_limits<qsizetype>::max() / uint(bytesPerLine) / uheight / udepth / ufaces / ulayers < 1
        || std::numeric_limits<qsizetype>::max() / sizeof(uchar *) / uwidth / uheight / udepth / ufaces / ulayers < 1)
        return nullptr;

    data.reset(new TextureData);

    data->ref.ref();
    data->width = width;
    data->height = height;
    data->depth = depth;
    data->faces = int(ufaces);
    data->levels = levels;
    data->layers = layers;
    data->bytesPerTexel = int(ubbp);
    data->format = format;
    data->type = type;

    data->bytesPerLine = qsizetype(bytesPerLine);

    data->nbytes = qsizetype(bytesPerLine * uheight * udepth * ufaces * ulayers);
    data->data = static_cast<uchar *>(malloc(size_t(data->nbytes)));

    if (!data->data)
        return nullptr;

    return data.release();
}

Texture::Texture()
    : d(nullptr)
{

}

Texture::Texture(const Texture &other)
    : d(other.d)
{
    if (d)
        d->ref.ref();
}

Texture &Texture::operator=(const Texture &other)
{
    if (this != &other) {
        if (other.d)
            other.d->ref.ref();
        if (d && !d->ref.deref())
            delete d;
        d = other.d;
    }
    return *this;
}

Texture::~Texture()
{
    if (d && !d->ref.deref())
        delete d;
}

void Texture::detach()
{
    if (d) {
        if (d->ref.load() != 1 /*|| d->ro_data*/)
            *this = copy();
    }
}

bool Texture::isDetached() const
{
    return d && d->ref.load() == 1;
}

Texture Texture::create1DTexture(int width, Format format)
{
    return Texture(TextureData::create(width, 1, 1, 1, 1, Type::Texture1D, format));
}

Texture Texture::create1DTextureArray(int width, int layers, Format format)
{
    return Texture(TextureData::create(width, 1, 1, layers, 1, Type::Texture1DArray, format));
}

Texture Texture::create2DTexture(int width, int height, Format format)
{
    return Texture(TextureData::create(width, height, 1, 1, 1, Type::Texture2D, format));
}

Texture Texture::create2DTextureArray(
        int width, int height, int layers, Format format)
{
    return Texture(TextureData::create(width, height, 1, layers, 1, Type::Texture2DArray, format));
}

Texture Texture::create3DTexture(int width, int height, int depth, Format format)
{
    return Texture(TextureData::create(width, height, depth, 1, 1, Type::Texture3D, format));
}

Texture Texture::createCubeMapTexture(int size, Texture::Format format)
{
    return Texture(TextureData::create(size, size, 1, 1, 1, Type::TextureCubeMap, format));
}

Texture Texture::createCubeMapTextureArray(int size, int layers, Texture::Format format)
{
    return Texture(TextureData::create(size, size, 1, 1, layers, Type::TextureCubeMapArray, format));
}

bool Texture::isNull() const
{
    return !d;
}

Texture::Type Texture::type() const
{
    if (!d)
        return Type::None;
    return d->type;
}

Texture::Format Texture::format() const
{
    return d ? d->format : Format::Invalid;
}

int Texture::width() const
{
    return d ? d->width : 0;
}

int Texture::height() const
{
    return d ? d->height : 0;
}

int Texture::depth() const
{
    return d ? d->depth : 0;
}

int Texture::levels() const
{
    return d ? 1 : 0;
}

int Texture::layers() const
{
    return d ? d->layers : 0;
}

qsizetype Texture::bytes() const
{
    return d ? d->nbytes : 0;
}

qsizetype Texture::bytesPerTexel() const
{
    return d ? d->bytesPerTexel : 0;
}

qsizetype Texture::bytesPerLine() const
{
    return d ? d->bytesPerLine : 0;
}

qsizetype Texture::bytesPerImage() const
{
    return d ? d->bytesPerLine * d->height * d->depth * d->faces : 0;
}

uchar *Texture::data(int layer)
{
    if (!d)
        return nullptr;
    detach();

    // In case detach ran out of memory...
    if (!d)
        return nullptr;

    return d ? d->data + bytesPerImage() * layer : nullptr;
}

const uchar *Texture::data(int layer) const
{
    return d ? d->data + bytesPerImage() * layer : nullptr;
}

uchar *Texture::scanLine(int y, int z)
{
    if (!d)
        return nullptr;

    detach();

    // In case detach() ran out of memory
    if (!d)
        return nullptr;

    Q_ASSERT(y >= 0 && y < height());
    Q_ASSERT(z >= 0 && (z < depth() || z < layers()));
    return d->data + y * d->bytesPerLine;
}

const uchar *Texture::scanLine(int y, int z) const
{
    if (!d)
        return nullptr;

    Q_ASSERT(y >= 0 && y < height());
    Q_ASSERT(z >= 0 && (z < depth() || z < layers()));
    return d->data + z * y * d->bytesPerLine;
}

/*!
    Performs a deep-copying of this image
*/
Texture Texture::copy() const
{
    Texture result(TextureData::create(
                       d->width, d->height, d->depth, d->layers, d->levels, d->type, d->format));
    if (result.isNull())
        return result;

    if (result.d->nbytes != d->nbytes) {
        const auto bpl = qMin(bytesPerLine(), result.bytesPerLine());
        for (int i = 0; i < height(); i++)
            memcpy(result.scanLine(i), scanLine(i), size_t(bpl));
    } else
        memcpy(result.data(), data(), size_t(d->nbytes));

    return result;
}

Texture::Texture(TextureData *dd):
    d(dd)
{
}

bool operator==(const Texture &lhs, const Texture &rhs)
{
    // same object, or shared?
    if (lhs.d == rhs.d)
        return true;

    if (!lhs.d || !rhs.d)
        return false;

    // TODO: check contents

    return false;
}

bool operator!=(const Texture &lhs, const Texture &rhs)
{
    return !(lhs == rhs);
}
