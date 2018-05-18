#include "texture_p.h"

#include <QtCore/QDebug>
#include <memory>

static inline bool isCubeMap(Texture::Type type)
{
    return type == Texture::Type::TextureCubeMap || type == Texture::Type::TextureCubeMapArray;
}

TextureData::~TextureData()
{
    free(data);
}

TextureData *TextureData::create(
        Texture::Type type,
        Texture::Format format,
        int width,
        int height,
        int depth,
        int layers,
        int levels)
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

int TextureData::getWidth(int level) const
{
    if (level < 0 || level >= levels) {
        qWarning() << Q_FUNC_INFO << "was called with invalid level" << level;
        return 0;
    }
    return std::max(width >> level, 1);
}

int TextureData::getHeight(int level) const
{
    if (level < 0 || level >= levels) {
        qWarning() << Q_FUNC_INFO << "was called with invalid level" << level;
        return 0;
    }
    return std::max(height >> level, 1);
}

int TextureData::getDepth(int level) const
{
    if (level < 0 || level >= levels) {
        qWarning() << Q_FUNC_INFO << "was called with invalid level" << level;
        return 0;
    }
    return std::max(depth >> level, 1);
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

Texture Texture::create1DTexture(Format format, int width)
{
    return Texture(TextureData::create(Type::Texture1D, format, width, 1, 1, 1, 1));
}

Texture Texture::create1DTexture(int levels, Texture::Format format, int width)
{
    return Texture(TextureData::create(Type::Texture1D, format, width, 1, 1, 1, levels));
}

Texture Texture::create1DTextureArray(Format format, int width, int layers)
{
    return Texture(TextureData::create(Type::Texture1DArray, format, width, 1, 1, layers, 1));
}

Texture Texture::create1DTextureArray(int levels, Texture::Format format, int width, int layers)
{
    return Texture(TextureData::create(Type::Texture1DArray, format, width, 1, 1, layers, levels));
}

Texture Texture::create2DTexture(Format format, int width, int height)
{
    return Texture(TextureData::create(Type::Texture2D, format, width, height, 1, 1, 1));
}

Texture Texture::create2DTexture(int levels, Format format, int width, int height)
{
    return Texture(TextureData::create(Type::Texture2D, format, width, height, 1, 1, levels));
}

Texture Texture::create2DTextureArray(
        Format format, int width, int height, int layers)
{
    return Texture(TextureData::create(Type::Texture2DArray, format, width, height, 1, layers, 1));
}

Texture Texture::create2DTextureArray(
        int levels, Texture::Format format, int width, int height, int layers)
{
    return Texture(TextureData::create(
                       Type::Texture2DArray, format, width, height, 1, layers, levels));
}

Texture Texture::create3DTexture(Format format, int width, int height, int depth)
{
    return Texture(TextureData::create(Type::Texture3D, format, width, height, depth, 1, 1));
}

Texture Texture::create3DTexture(
        int levels, Texture::Format format, int width, int height, int depth)
{
    return Texture(TextureData::create(Type::Texture3D, format, width, height, depth, 1, levels));
}

Texture Texture::createCubeMapTexture(Texture::Format format, int size)
{
    return Texture(TextureData::create(Type::TextureCubeMap, format, size, size, 1, 1, 1));
}

Texture Texture::createCubeMapTexture(int levels, Texture::Format format, int size)
{
    return Texture(TextureData::create(Type::TextureCubeMap, format, size, size, 1, 1, levels));
}

Texture Texture::createCubeMapTextureArray(Texture::Format format, int size, int layers)
{
    return Texture(TextureData::create(
                       Type::TextureCubeMapArray, format, size, size, 1, layers, 1));
}

Texture Texture::createCubeMapTextureArray(int levels, Texture::Format format, int size, int layers)
{
    return Texture(TextureData::create(
                       Type::TextureCubeMapArray, format, size, size, 1, layers, levels));
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

int Texture::width(int level) const
{
    return d ? d->getWidth(level) : 0;
}

int Texture::height() const
{
    return d ? d->height : 0;
}

int Texture::height(int level) const
{
    return d ? d->getHeight(level) : 0;
}

int Texture::depth() const
{
    return d ? d->depth : 0;
}

int Texture::depth(int level) const
{
    return d ? d->getDepth(level) : 0;
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
    Performs a deep-copying of this texture
*/
Texture Texture::copy() const
{
    Texture result(TextureData::create(
                       d->type, d->format, d->width, d->height, d->depth, d->layers, d->levels));
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

uchar *Texture::dataImpl(int side, int layer, int level)
{
    Q_UNUSED(level);
    Q_ASSERT(!level);
    if (!d)
        return nullptr;
    detach();

    // In case detach ran out of memory...
    if (!d)
        return nullptr;

    return d->data + bytesPerImage() * layer * side;
}

uchar *Texture::dataImpl(int side, int layer, int level) const
{
    Q_UNUSED(level);
    Q_ASSERT(!level);
    return d ? d->data + bytesPerImage() * layer * side : nullptr;
}

bool operator==(const Texture &lhs, const Texture &rhs)
{
    // same object, or shared?
    if (lhs.d == rhs.d)
        return true;

    if (!lhs.d || !rhs.d)
        return false;

    // obviously different stuff?
    if (lhs.d->type != rhs.d->type
            || lhs.d->format != rhs.d->format
            || lhs.d->width  != rhs.d->width
            || lhs.d->height != rhs.d->height
            || lhs.d->depth  != rhs.d->depth
            || lhs.d->layers != rhs.d->layers
            || lhs.d->levels != rhs.d->levels)
        return false;

    // sanity check for data sizes
    if (lhs.d->nbytes != rhs.d->nbytes)
        return false;

    return memcmp(lhs.d->data, rhs.d->data, size_t(rhs.d->nbytes)) == 0;
}

bool operator!=(const Texture &lhs, const Texture &rhs)
{
    return !(lhs == rhs);
}

QDataStream &operator<<(QDataStream &stream, const Texture &texture)
{
    stream << quint32(texture.type())
           << quint32(texture.format())
           << quint32(texture.width())
           << quint32(texture.height())
           << quint32(texture.depth())
           << quint32(texture.layers())
           << quint32(texture.levels())
           << QByteArray::fromRawData(
                  reinterpret_cast<const char *>(texture.data()), int(texture.bytes()));
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Texture &texture)
{
    texture = Texture();
    quint32 type;
    quint32 format;
    quint32 width;
    quint32 height;
    quint32 depth;
    quint32 layers;
    quint32 levels;
    QByteArray data;
    stream >> type
            >> format
            >> width
            >> height
            >> depth
            >> layers
            >> levels
            >> data;
    if (stream.status() == QDataStream::Ok) {
        auto result = Texture(TextureData::create(
                                  Texture::Type(type),
                                  Texture::Format(format),
                                  int(width),
                                  int(height),
                                  int(depth),
                                  int(layers),
                                  int(levels)));
        if (result.bytes() == data.size()) {
            memmove(result.data(), data.data(), size_t(data.size()));
            texture = result;
        }
    }
    return stream;
}
