#include "texture_p.h"
#include "textureio.h"

#include <QtCore/QDebug>
#include <memory>

#define CHECK_SIDE(side, rv) \
    if (side < 0 || side >= d->faces) { \
        qCWarning(texture) << "was called with invalid side" << side; \
        return rv; \
    } \

#define CHECK_LEVEL(level, rv) \
    if (level < 0 || level >= d->levels) { \
        qCWarning(texture) << "was called with invalid level" << level; \
        return rv; \
    } \

#define CHECK_LAYER(layer, rv) \
    if (layer < 0 || layer >= d->layers) { \
        qCWarning(texture) << "was called with invalid layer" << layer; \
        return rv; \
    } \

#define CHECK_POINT(x, y, z, level, rv) \
    if (x < 0 || x >= d->levelWidth(level) \
        || y < 0 || y >= d->levelHeight(level) \
        || z < 0 || z >= d->levelDepth(level)) { \
        qCWarning(texture) << "point (" << x << y << z << ")" << "is out of bounds"; \
        return rv; \
    } \

#define CHECK_ZERO_X(x, rv) \
    if (x != 0) { \
        qCWarning(texture) << "x should be 0"; \
        return rv; \
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

    qsizetype totalBytes = 0;
    std::vector<LevelInfo> levelInfos;

    for (int level = 0; level < levels; ++level) {
        auto w = std::max<uint>(uwidth >> level, 1);
        auto h = std::max<uint>(uheight >> level, 1);
        auto d = std::max<uint>(udepth >> level, 1);

        const auto texelFormat = TexelFormat::texelFormat(format);

        // calculateBytesPerLine already checks overflow
        const auto bytesPerLine = TextureData::calculateBytesPerLine(texelFormat, w); // bytes per scanline
        if (!bytesPerLine)
            return nullptr;

        // calculateBytesPerSlice already checks overflow
        const auto bytesPerSlice = TextureData::calculateBytesPerSlice(texelFormat, w, h);

        // check that single mipmap doesn't overflow
        if (std::numeric_limits<qsizetype>::max() / bytesPerSlice / d / ufaces / ulayers < 1
                || std::numeric_limits<qsizetype>::max() / sizeof(uchar *) / w / h / d / ufaces / ulayers < 1) {
            qCWarning(texture) << "potential integer overflow";
            return nullptr;
        }

        const auto bytesPerLevel = bytesPerSlice * d * ufaces * ulayers;

        // check we didn't overflow total memory
        const auto uTotalBytes = size_t(totalBytes);
        if (size_t(uTotalBytes + size_t(bytesPerLevel)) < uTotalBytes) {
            qCWarning(texture) << "potential integer overflow";
            return nullptr;
        }

        levelInfos.push_back({totalBytes, bytesPerLine, bytesPerSlice, bytesPerLevel});

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
    data->format = format;
    data->type = type;

    data->levelInfos = std::move(levelInfos);

    data->nbytes = totalBytes;
    data->data = static_cast<uchar *>(malloc(size_t(data->nbytes)));

    if (!data->data)
        return nullptr;

    return data.release();
}

qsizetype TextureData::calculateBytesPerLine(
        const TexelFormat &format, quint32 width, Texture::Alignment align)
{
    const auto bitsPerTexel = qsizetype(format.bitsPerTexel());
    const auto blockSize = qsizetype(format.blockSize());

    switch (format.format()) {
    case Texture::Format::Invalid:
    case Texture::Format::FormatsCount:
        return 0;
    case Texture::Format::RGBA_8888:
    case Texture::Format::BGRA_8888:
    case Texture::Format::RGB_888:
        if (bitsPerTexel && std::numeric_limits<qsizetype>::max() / bitsPerTexel / width < 1) {
            qCWarning(texture) << "potential integer overflow";
            return 0;
        }
        if (align == Texture::Alignment::Byte)
            return (width * bitsPerTexel + 7) >> 3;
        else if (align == Texture::Alignment::Word)
            return ((width * bitsPerTexel + 31) >> 5) << 2;
        break;
    case Texture::Format::DXT1:
    case Texture::Format::DXT3:
    case Texture::Format::DXT5:
        if (std::numeric_limits<qsizetype>::max() / blockSize / ((width + 3) / 4) < 1) {
            qCWarning(texture) << "potential integer overflow";
            return 0;
        }
        return std::max(1u, (width + 3) / 4) * blockSize;
    }
    return 0;
}

qsizetype TextureData::calculateBytesPerSlice(
        const TexelFormat &format, quint32 width, quint32 height, Texture::Alignment align)
{
    const auto bytesPerLine = calculateBytesPerLine(format, width, align);

    switch (format.format()) {
    case Texture::Format::RGBA_8888:
    case Texture::Format::BGRA_8888:
    case Texture::Format::RGB_888:
        if (std::numeric_limits<qsizetype>::max() / bytesPerLine / height < 1) {
            qCWarning(texture) << "potential integer overflow";
            return 0;
        }
        return bytesPerLine * height;
    case Texture::Format::DXT1:
    case Texture::Format::DXT3:
    case Texture::Format::DXT5:
        if (std::numeric_limits<qsizetype>::max() / bytesPerLine / ((height + 3) / 4) < 1) {
            qCWarning(texture) << "potential integer overflow";
            return 0;
        }
        return bytesPerLine * std::max(1u, (height + 3) / 4);
    default:
        break;
    }
    return 0;
}

qsizetype TextureData::offset(int side, int level, int layer) const
{
    return levelInfos[level].offset + bytesPerImage(level) * (faces * layer + side);
}

Texture::Texture(const Texture &other)
    : d(other.d)
{
    if (d)
        d->ref.ref();
}

Texture::Texture(const QString &file)
    : d(nullptr)
{
    auto ok = load(file);
    if (!ok)
        qCWarning(texture) << "Can't load texture from file" << file << toUserString(ok);
}

Texture::Texture(QStringView file) :
    Texture(file.toString())
{
}

Texture::Texture(const QImage& image)
    : d(nullptr)
{
    QImage copy;
    Texture::Format format = Texture::Format::Invalid;
    switch (image.format()) {
    case QImage::Format_ARGB32:
        format = Texture::Format::RGBA_8888;
        copy = image.convertToFormat(QImage::Format_RGBA8888);
        break;
    case QImage::Format_RGB888:
        format = Texture::Format::RGB_888;
        copy = image;
        break;
    default:
        qCWarning(texture) << "unsupported image format" << image.format();
        return;
    }

    auto result = Texture::create(Texture::Type::Texture2D, format, copy.width(), copy.height(), 1);
    if (result.bytesPerLine() < copy.bytesPerLine()) {
        qCWarning(texture) << Q_FUNC_INFO
                           << "texture line size is less than an image line size"
                           << result.bytesPerLine() << "<" << copy.bytesPerLine();
        return;
    }

    // We use scanline here because QImage has different alignment
    for (int y = 0; y < result.height(); ++y) {
        const auto line = result.lineData({0, y}, {});
        memcpy(line.data(), copy.scanLine(y), size_t(copy.bytesPerLine()));
    }

    *this = std::move(result);
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

Texture Texture::create(Texture::Type type, Texture::Format format, int width, int height, int depth, int levels, int layers)
{
    Texture result;
    switch (type) {
    case Texture::Type::None:
        return result;
    case Texture::Type::Texture1D:
        if (height != 1 || depth != 1 || layers != 1)
            return result;
        break;
    case Texture::Type::Texture1DArray:
        if (height != 1 || depth != 1)
            return result;
        break;
    case Texture::Type::Texture2D:
        if (depth != 1 || layers != 1)
            return result;
        break;
    case Texture::Type::Texture2DArray:
        if (depth != 1)
            return result;
        break;
    case Texture::Type::Texture3D:
        if (layers != 1)
            return result;
        break;
    case Texture::Type::TextureCubeMap:
        if (width != height || depth != 1 || layers != 1)
            return result;
        break;
    case Texture::Type::TextureCubeMapArray:
        if (width != height || depth != 1)
            return result;
        break;
    }

    result = Texture(TextureData::create(type, format, width, height, depth, layers, levels));
    return result;
}

qsizetype Texture::calculateBytesPerLine(Format format, int width, Alignment align)
{
    return TextureData::calculateBytesPerLine(
                TexelFormat::texelFormat(format), quint32(width), align);
}

qsizetype Texture::calculateBytesPerSlice(Format format, int width, int height, Alignment align)
{
    return TextureData::calculateBytesPerSlice(
                TexelFormat::texelFormat(format), quint32(width), quint32(height), align);
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

bool Texture::isCompressed() const
{
    return d && (d->format == Format::DXT1
                 || d->format == Format::DXT3
                 || d->format == Format::DXT5);
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

qsizetype Texture::bitsPerTexel() const
{
    return d ? TexelFormat::texelFormat(d->format).bitsPerTexel() : 0;
}

qsizetype Texture::bytesPerLine(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->bytesPerLine(level);
}

qsizetype Texture::bytesPerSlice(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->bytesPerSlice(level);
}

qsizetype Texture::bytesPerImage(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->bytesPerImage(level);
}

/*!
    Returns the offset in bytes for the given \a index from the beggining of the data.
*/
qsizetype Texture::offset(Index index) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(index.level(), 0);
    CHECK_LAYER(index.layer(), 0);
    CHECK_SIDE(index.face(), 0);

    return d->offset(index.face(), index.level(), index.layer());
}

auto Texture::imageData(Index index) -> Data
{
    return {dataImpl(index.face(), index.level(), index.layer()), bytesPerImage(index.level())};
}

auto Texture::imageData(Index index) const -> ConstData
{
    return {dataImpl(index.face(), index.level(), index.layer()), bytesPerImage(index.level())};
}

auto Texture::constImageData(Index index) const -> ConstData
{
    return {dataImpl(index.face(), index.level(), index.layer()), bytesPerImage(index.level())};
}

auto Texture::lineData(const Texture::Position &p, const Texture::Index& i) -> Data
{
    if (!d)
        return Data();

    auto data = dataImpl(i.face(), i.level(), i.layer()); // detach here
    if (!data)
        return Data();

    CHECK_ZERO_X(p.x(), Data());
    CHECK_POINT(p.x(), p.y(), p.z(), i.level(), Data());

    const auto pointer = data
            // actually, depth should be always 1 for cubemaps, but do the correct math here
            + d->bytesPerLine(i.level()) * height(i.level()) * depth(i.level()) * i.face()
            + d->bytesPerLine(i.level()) * height(i.level()) * p.z()
            + d->bytesPerLine(i.level()) * p.y();
    const auto size = d->bytesPerLine(i.level());

    return Data(pointer, size);
}

auto Texture::lineData(const Texture::Position &p, const Texture::Index& i) const -> ConstData
{
    if (!d)
        return ConstData();

    auto data = dataImpl(i.face(), i.level(), i.layer());
    if (!data)
        return ConstData();

    CHECK_ZERO_X(p.x(), Data());
    CHECK_POINT(p.x(), p.y(), p.z(), i.level(), ConstData());

    const auto pointer = data
            // actually, depth should be always 1 for cubemaps, but do the correct math here
            + d->bytesPerLine(i.level()) * height(i.level()) * depth(i.level()) * i.face()
            + d->bytesPerLine(i.level()) * height(i.level()) * p.z()
            + d->bytesPerLine(i.level()) * p.y();
    const auto size = d->bytesPerLine(i.level());

    return ConstData(pointer, size);
}

auto Texture::constLineData(const Texture::Position &p, const Texture::Index& index) const -> ConstData
{
    return lineData(p, index);
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
    memcpy(result.data().data(), data().data(), size_t(d->nbytes));

    return result;
}

QImage Texture::toImage() const
{
    if (!d)
        return QImage();

    if (d->type == Texture::Type::TextureCubeMap
            || d->type == Texture::Type::TextureCubeMapArray
            || d->type == Texture::Type::Texture3D) {
        qCWarning(texture) << "unsupported texture type" << int(d->type);
        return QImage();
    }

    QImage::Format imageFormat = QImage::Format_Invalid;
    switch (d->format) {
    case Texture::Format::RGBA_8888:
        imageFormat = QImage::Format_RGBA8888;
        break;
    case Texture::Format::RGB_888:
        imageFormat = QImage::Format_RGB888;
        break;
    default:
        break;
    }

    if (imageFormat == QImage::Format_Invalid) {
        qCWarning(texture) << "unsupported texture format" << int(d->format);
        return QImage();
    }

    QImage result(d->width, d->height, imageFormat);
    if (result.isNull()) {
        qCWarning(texture) << "can't create image";
        return QImage();
    }

    const auto bytesPerLine = this->bytesPerLine();
    if (result.bytesPerLine() < bytesPerLine) {
        qCWarning(texture) << Q_FUNC_INFO
                           << "image line size is less than texture line size"
                           << result.bytesPerLine() << "<" << bytesPerLine;
        return QImage();
    }

    // We use scanline here because QImage has different alignment
    for (int y = 0; y < height(); ++y) {
        const auto line = lineData({0, y}, {});
        memcpy(result.scanLine(y), line.data(), size_t(line.size()));
    }

    return result;
}

TextureIOResult Texture::load(const QString &file)
{
    TextureIO io(file);
    auto result = io.read();
    if (result) {
        *this = *result;
        return TextureIOResult();
    }
    return result.error();
}

TextureIOResult Texture::save(const QString &file)
{
    TextureIO io(file);
    return io.write(*this);
}

Texture::Texture(TextureData *dd):
    d(dd)
{
}

uchar *Texture::dataImpl(int side, int level, int layer)
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

    return d->data + d->offset(side, level, layer);
}

const uchar* Texture::dataImpl(int side, int level, int layer) const
{
    if (!d)
        return nullptr;

    CHECK_SIDE(side, nullptr);
    CHECK_LEVEL(level, nullptr);
    CHECK_LAYER(layer, nullptr);

    return d->data + d->offset(side, level, layer);
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
                  reinterpret_cast<const char *>(texture.data().data()), int(texture.data().size()));
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
            memmove(result.data().data(), data.data(), size_t(data.size()));
            texture = result;
        }
    }
    return stream;
}



QDebug operator<<(QDebug &d, const Texture::Position &position)
{
    auto s = QString("Texture::Position(x = %1, y = %2, z = %3)").arg(
                QString::number(position.x()),
                QString::number(position.y()),
                QString::number(position.z()));
    d << s;
    return d;
}

QDebug operator<<(QDebug& d, const Texture::Index& index)
{
    auto s = QString("Texture::Index(face = %1, level = %2, layer = %3)").arg(
                QString::number(index.face()),
                QString::number(index.level()),
                QString::number(index.layer()));
    d << s;
    return d;
}

Q_LOGGING_CATEGORY(texture, "texturelib.texture")
