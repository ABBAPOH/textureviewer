#include "texture_p.h"
#include "textureio.h"

#include <QtCore/QDebug>
#include <memory>

#define CHECK_SIDE(side, rv) \
    if ((side) < 0 || (side) >= d->faces) { \
        qCWarning(texture) << "was called with invalid side" << (side); \
        return (rv); \
    } \

#define CHECK_LEVEL(level, rv) \
    if ((level) < 0 || (level) >= d->levels) { \
        qCWarning(texture) << "was called with invalid level" << (level); \
        return (rv); \
    } \

#define CHECK_LAYER(layer, rv) \
    if ((layer) < 0 || (layer) >= d->layers) { \
        qCWarning(texture) << "was called with invalid layer" << (layer); \
        return (rv); \
    } \

#define CHECK_POINT(x, y, z, level, rv) \
    if ((x) < 0 || (x) >= d->levelWidth(level) \
        || (y) < 0 || (y) >= d->levelHeight(level) \
        || (z) < 0 || (z) >= d->levelDepth(level)) { \
        qCWarning(texture) << "point (" << (x) << (y) << (z) << ")" << "is out of bounds"; \
        return (rv); \
    } \

#define CHECK_ZERO_X(x, rv) \
    if ((x) != 0) { \
        qCWarning(texture) << "x should be 0"; \
        return (rv); \
    } \

namespace {

void memoryCopy(Texture::Data dst, Texture::ConstData src)
{
    memcpy(dst.data(), src.data(), std::min(dst.size_bytes(), src.size_bytes()));
}

int memoryCompare(Texture::ConstData lhs, Texture::ConstData rhs)
{
    if (lhs.size_bytes() != rhs.size_bytes()) {
        return lhs.size_bytes() < rhs.size_bytes() ? -1 : 1;
    }

    return memcmp(lhs.data(), rhs.data(), lhs.size_bytes());
}

} // namespace

TextureData::~TextureData()
{
    free(data);
}

TextureData *TextureData::create(
        Texture::Format format,
        int width,
        int height,
        int depth,
        bool isCubemap,
        int levels,
        int layers,
        Texture::Alignment align)
{
    std::unique_ptr<TextureData> data;

    if (width <= 0 || height <= 0 || depth <= 0 || layers <= 0 || format == Texture::Format::Invalid)
        return nullptr; // invalid parameter(s)

    const auto uwidth = uint(width);
    const auto uheight = uint(height);
    const auto udepth = uint(depth);
    const auto ufaces = isCubemap ? uint(6) : uint(1);
    const auto ulayers = uint(layers);

    qsizetype totalBytes = 0;
    std::vector<LevelInfo> levelInfos;

    const auto texelFormat = TexelFormat::texelFormat(format);

    for (int level = 0; level < levels; ++level) {
        const auto ulevel = uint(level);
        const auto w = std::max<uint>(uwidth >> ulevel, 1);
        const auto h = std::max<uint>(uheight >> ulevel, 1);
        const auto d = std::max<uint>(udepth >> ulevel, 1);

        // calculateBytesPerLine already checks overflow
        const auto bytesPerLine = TextureData::calculateBytesPerLine(texelFormat, w, align); // bytes per scanline
        if (!bytesPerLine)
            return nullptr;

        // calculateBytesPerSlice already checks overflow
        const auto bytesPerSlice = TextureData::calculateBytesPerSlice(texelFormat, w, h, align);

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

    data = std::make_unique<TextureData>();

    data->ref.ref();
    data->width = width;
    data->height = height;
    data->depth = depth;
    data->faces = int(ufaces);
    data->levels = levels;
    data->layers = layers;
    data->format = format;
    data->align = align;
    data->compressed = texelFormat.isCompressed();

    data->levelInfos = std::move(levelInfos);

    data->nbytes = totalBytes;
    data->data = static_cast<uchar *>(calloc(size_t(data->nbytes), 1));

    if (!data->data)
        return nullptr;

    return data.release();
}

qsizetype TextureData::calculateBytesPerLine(
        const TexelFormat &format, quint32 width, Texture::Alignment align)
{
    const auto bitsPerTexel = size_t(format.bitsPerTexel());
    const auto blockSize = size_t(format.blockSize());

    if (bitsPerTexel) {
        if (bitsPerTexel && std::numeric_limits<qsizetype>::max() / bitsPerTexel / width < 1) {
            qCWarning(texture) << "potential integer overflow";
            return 0;
        }
        if (align == Texture::Alignment::Byte)
            return (width * bitsPerTexel + 7u) >> 3u;
        if (align == Texture::Alignment::Word)
            return ((width * bitsPerTexel + 31u) >> 5u) << 2u;
    } else if (blockSize) { // compressed format
        if (std::numeric_limits<qsizetype>::max() / blockSize / ((width + 3) / 4) < 1) {
            qCWarning(texture) << "potential integer overflow";
            return 0;
        }
        return std::max(1u, (width + 3) / 4) * blockSize;
    }

    Q_UNREACHABLE();
    return 0;
}

qsizetype TextureData::calculateBytesPerSlice(
        const TexelFormat &format, quint32 width, quint32 height, Texture::Alignment align)
{
    const auto bytesPerLine = calculateBytesPerLine(format, width, align);

    if (format.bitsPerTexel()) {
        if (std::numeric_limits<qsizetype>::max() / bytesPerLine / height < 1) {
            qCWarning(texture) << "potential integer overflow";
            return 0;
        }
        return bytesPerLine * height;
    }

    if (format.blockSize()) { // compressed format
        if (std::numeric_limits<qsizetype>::max() / bytesPerLine / ((height + 3) / 4) < 1) {
            qCWarning(texture) << "potential integer overflow";
            return 0;
        }
        return bytesPerLine * std::max(1u, (height + 3) / 4);
    }

    Q_UNREACHABLE();
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

    auto result = Texture::create(format, {copy.width(), copy.height()});
    const auto bytesPerLine = result.bytesPerLine();
    if (bytesPerLine < copy.bytesPerLine()) {
        qCWarning(texture) << Q_FUNC_INFO
                           << "texture line size is less than an image line size"
                           << result.bytesPerLine() << "<" << copy.bytesPerLine();
        return;
    }

    const auto data = imageData({});
    for (int y = 0; y < result.height(); ++y) {
        const auto line = data.subspan(bytesPerLine * y, bytesPerLine);
        memoryCopy(line, {copy.scanLine(y), copy.bytesPerLine()});
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

Texture Texture::create(Format format, Size size, IsCubemap isCubemap, int levels, int layers, Texture::Alignment align)
{
    const auto width = size.width();
    const auto height = size.height();
    const auto depth = size.depth();

    if (isCubemap == IsCubemap::Yes) {
        if (width != height || depth != 1)
            return Texture();
    } else {
        if (depth > 1 && layers > 1) // array of 3d texture are not supported
            return Texture();
    }
    return Texture(TextureData::create(format, width, height, depth, bool(isCubemap), levels, layers, align));
}

Texture Texture::create(Texture::Format format, Texture::Size size, int levels, int layers, Texture::Alignment align)
{
    return create(format, size, IsCubemap::No, levels, layers, align);
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

Texture::Format Texture::format() const
{
    return d ? d->format : Format::Invalid;
}

Texture::Alignment Texture::alignment() const
{
    return d ? d->align : Alignment::Byte;
}

bool Texture::isCompressed() const
{
    return d && d->compressed;
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

int Texture::faces() const
{
    return d ? d->faces : 0;
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

Texture Texture::convert(Texture::Alignment align) const
{
    if (!d)
        return Texture();

    if (isCompressed())
        return *this;

    if (align == alignment())
        return *this;

    auto result = Texture(TextureData::create(
                d->format,
                d->width, d->height, d->faces == 6, d->depth,
                d->layers, d->levels,
                align));

    for (int level = 0; level < d->levels; ++level) {
        for (int layer = 0; layer < d->layers; ++layer) {
            for (int face = 0; face < d->faces; ++face) {
                const auto srcData = imageData({Side(face), level, layer});
                const auto dstData = result.imageData({Side(face), level, layer});
                for (int z = 0; z < d->depth; ++z) {
                    for (int y = 0; y < d->height; ++y) {
                        const auto srcLine = srcData.subspan(
                                    d->bytesPerSlice(level) * z + d->bytesPerLine(y),
                                    d->bytesPerLine(y));
                        const auto dstLine = dstData.subspan(
                                    result.d->bytesPerSlice(level) * z + result.d->bytesPerLine(y),
                                    result.d->bytesPerLine(y));
                        memoryCopy(dstLine, srcLine);
                    }
                }
            }
        }
    }

    return result;
}

/*!
    Performs a deep-copying of this texture
*/
Texture Texture::copy() const
{
    Texture result(TextureData::create(
                       d->format, d->width, d->height, d->faces == 6, d->depth, d->layers, d->levels, d->align));
    if (result.isNull())
        return result;

    Q_ASSERT(result.d->nbytes == d->nbytes);
    memoryCopy(result.data(), data());

    return result;
}

QImage Texture::toImage() const
{
    if (!d)
        return QImage();

    if (d->faces > 1|| d->depth > 1) {
        qCWarning(texture) << "Cubemaps and volumemaps are not supported";
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

    const auto data = imageData({});
    for (int y = 0; y < height(); ++y) {
        const auto line = data.subspan(bytesPerLine * y, bytesPerLine);
        memoryCopy({result.scanLine(y), result.bytesPerLine()}, line);
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
    if (lhs.d->format != rhs.d->format
            || lhs.d->width  != rhs.d->width
            || lhs.d->height != rhs.d->height
            || lhs.d->depth  != rhs.d->depth
            || lhs.d->faces  != rhs.d->faces
            || lhs.d->layers != rhs.d->layers
            || lhs.d->levels != rhs.d->levels)
        return false;

    return memoryCompare({lhs.d->data, lhs.d->nbytes}, {rhs.d->data, rhs.d->nbytes}) == 0;
}

bool operator!=(const Texture &lhs, const Texture &rhs)
{
    return !(lhs == rhs);
}

QDataStream &operator<<(QDataStream &stream, const Texture &texture)
{
    stream << quint32(texture.format())
           << quint32(texture.width())
           << quint32(texture.height())
           << quint32(texture.depth())
           << quint32(texture.faces())
           << quint32(texture.layers())
           << quint32(texture.levels())
           << quint8(texture.alignment())
           << QByteArray::fromRawData(
                  reinterpret_cast<const char *>(texture.data().data()), int(texture.data().size()));
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Texture &texture)
{
    texture = Texture();
    quint32 format;
    quint32 width;
    quint32 height;
    quint32 depth;
    quint32 faces;
    quint32 layers;
    quint32 levels;
    quint8 align;
    QByteArray data;
    stream >> format
            >> width
            >> height
            >> depth
            >> faces
            >> layers
            >> levels
            >> align
            >> data;
    if (stream.status() == QDataStream::Ok) {
        auto result = Texture(TextureData::create(
                                  Texture::Format(format),
                                  int(width),
                                  int(height),
                                  int(depth),
                                  faces > 1,
                                  int(layers),
                                  int(levels),
                                  Texture::Alignment(align)));
        if (result.bytes() == data.size()) {
            memoryCopy(result.data(), {reinterpret_cast<const uchar *>(data.constData()), data.size()});
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

QString toQString(Texture::Format format)
{
    const auto &mo = Texture::staticMetaObject;
    const auto index = mo.indexOfEnumerator("Format");
    const auto en = mo.enumerator(index);
    return en.valueToKey(int(format));
}

Q_LOGGING_CATEGORY(texture, "texturelib.texture")
