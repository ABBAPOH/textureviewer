#include "texture_p.h"

#include <QtCore/QDebug>
#include <memory>

#define CHECK_LEVEL(level, rv) \
    if (level < 0 || level >= d->levels) { \
        qCWarning(texture) << Q_FUNC_INFO << "was called with invalid level" << level; \
        return 0; \
    } \

#define CHECK_LAYER(layer, rv) \
    if (layer < 0 || layer >= d->layers) { \
        qCWarning(texture) << Q_FUNC_INFO << "was called with invalid layer" << layer; \
        return 0; \
    } \

#define CHECK_SIDE(side, rv) \
    if (side < 0 || side >= d->faces) { \
        qCWarning(texture) << Q_FUNC_INFO << "was called with invalid side" << side; \
        return 0; \
    } \

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

    qsizetype totalBytes = 0;
    std::vector<LevelInfo> levelInfos;

    for (int level = 0; level < levels; ++level) {
        auto w = std::max<uint>(uwidth >> level, 1);
        auto h = std::max<uint>(uheight >> level, 1);
        auto d = std::max<uint>(udepth >> level, 1);

        const auto bytesPerLine = ((uwidth * ubbp + 31) >> 5) << 2; // bytes per scanline (must be multiple of 4)

        // TODO: check overflows!!!
        const auto bytesPerLevel = bytesPerLine * h * d * ufaces * ulayers;
        levelInfos.push_back({totalBytes, bytesPerLine, bytesPerLevel});

        totalBytes += bytesPerLevel;

        if (w == 1 && h == 1 && d == 1) {
            levels = level + 1;
            break;
        }
    }

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

    data->levelInfos = std::move(levelInfos);

    data->nbytes = totalBytes;
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

Texture Texture::create1DTexture(Format format, int width, int levels, int layers)
{
    const auto type = layers == -1 ? Type::Texture1D : Type::Texture1DArray;
    layers = layers == -1 ? 1 : layers;
    return Texture(TextureData::create(type, format, width, 1, 1, layers, levels));
}

Texture Texture::create2DTexture(Format format, int width, int height, int levels, int layers)
{
    const auto type = layers == -1 ? Type::Texture2D : Type::Texture2DArray;
    layers = layers == -1 ? 1 : layers;
    return Texture(TextureData::create(type, format, width, height, 1, layers, levels));
}

Texture Texture::create3DTexture(
        Format format, int width, int height, int depth, int levels)
{
    return Texture(TextureData::create(Type::Texture3D, format, width, height, depth, 1, levels));
}

Texture Texture::createCubeMapTexture(Texture::Format format, int size, int levels, int layers)
{
    const auto type = layers == -1 ? Type::TextureCubeMap : Type::TextureCubeMapArray;
    layers = layers == -1 ? 1 : layers;
    return Texture(TextureData::create(type, format, size, size, 1, layers, levels));
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
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->levelWidth(level);
}

int Texture::height() const
{
    return d ? d->height : 0;
}

int Texture::height(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->levelHeight(level);
}

int Texture::depth() const
{
    return d ? d->depth : 0;
}

int Texture::depth(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->levelDepth(level);
}

int Texture::levels() const
{
    return d ? d->levels : 0;
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

qsizetype Texture::bytesPerLine(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->bytesPerLine(level);
}

qsizetype Texture::bytesPerImage(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->bytesPerImage(level);
}

qsizetype Texture::levelOffset(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->levelOffset(level);
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

    Q_ASSERT(result.d->nbytes == d->nbytes);
    memcpy(result.data(), data(), size_t(d->nbytes));

    return result;
}

Texture::Texture(TextureData *dd):
    d(dd)
{
}

uchar *Texture::dataImpl(int side, int layer, int level)
{
    if (!d)
        return nullptr;

    CHECK_SIDE(side, nullptr);
    CHECK_LEVEL(level, nullptr);
    CHECK_LAYER(layer, nullptr);
    detach();

    // In case detach ran out of memory...
    if (!d)
        return nullptr;

    return d->data + d->levelOffset(level) + d->bytesPerImage(level) * layer * side;
}

uchar *Texture::dataImpl(int side, int level, int layer) const
{
    if (!d)
        return nullptr;

    CHECK_SIDE(side, nullptr);
    CHECK_LEVEL(level, nullptr);
    CHECK_LAYER(layer, nullptr);

    return d->data + d->levelOffset(level) + d->bytesPerImage(level) * layer * side;
}

uchar *Texture::texelDataImpl(int side, int x, int y, int z, int level, int layer)
{
    Q_UNREACHABLE();
    return nullptr;
}

uchar *Texture::texelDataImpl(int side, int x, int y, int z, int level, int layer) const
{
    Q_UNREACHABLE();
    return nullptr;
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

Q_LOGGING_CATEGORY(texture, "texturelib.texture")
