#include "texture_p.h"
#include "textureio.h"

#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>

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

TextureData *TextureData::create(
        TextureFormat format,
        int width,
        int height,
        int depth,
        bool isCubemap,
        int levels,
        int layers,
        Texture::Alignment align,
        Texture::Data data,
        Texture::DataDeleter deleter)
{
    std::unique_ptr<TextureData> result;

    if (width <= 0 || height <= 0 || depth <= 0 || layers <= 0 || format == TextureFormat::Invalid) {
        qCWarning(texture) << "Invalid parameter(s) passed to Texture::create";
        return nullptr;
    }

    if (isCubemap) {
        if (width != height || depth != 1) {
            qCWarning(texture)
                    << "Width should be equal to height and depth should be 1 for a cuve texture";
            return nullptr;
        }
    } else {
        if (depth > 1 && layers > 1) {
            qCWarning(texture) << "Arrays of 3d textures are not supported";
            return nullptr;
        }
    }

    const auto uwidth = uint(width);
    const auto uheight = uint(height);
    const auto udepth = uint(depth);
    const auto ufaces = isCubemap ? uint(6) : uint(1);
    const auto ulayers = uint(layers);

    qsizetype totalBytes = 0;
    std::vector<LevelInfo> levelInfos;
    levelInfos.reserve(levels);

    const auto texelFormat = TextureFormatInfo::formatInfo(format);

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

    result = std::make_unique<TextureData>();

    result->ref.ref();
    result->width = width;
    result->height = height;
    result->depth = depth;
    result->faces = int(ufaces);
    result->levels = levels;
    result->layers = layers;
    result->format = format;
    result->align = align;
    result->compressed = texelFormat.isCompressed();

    result->levelInfos = std::move(levelInfos);

    result->nbytes = totalBytes;
    if (data.empty()) {
        result->data = DataPointer(
                new (std::nothrow) uchar[size_t(result->nbytes)], [](uchar p[]){ delete [] p;});
        if (!result->data)
            return nullptr;
    } else {
        if (data.size_bytes() != result->nbytes) {
            qCWarning(texture) << "Invalid data size:"
                               << data.size_bytes() << "!=" << result->nbytes;
            return nullptr;
        }
        if (deleter)
            result->data = DataPointer(data.data(), deleter);
        else
            result->data = DataPointer(data.data());
    }

    return result.release();
}

qsizetype TextureData::calculateBytesPerLine(
        const TextureFormatInfo &format, quint32 width, Texture::Alignment align)
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
        const TextureFormatInfo &format, quint32 width, quint32 height, Texture::Alignment align)
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
    TextureFormat format = TextureFormat::Invalid;
    switch (image.format()) {
    case QImage::Format_ARGB32:
        format = TextureFormat::RGBA8_Unorm;
        copy = image.convertToFormat(QImage::Format_RGBA8888);
        break;
    case QImage::Format_RGB888:
        format = TextureFormat::RGB8_Unorm;
        copy = image;
        break;
    default:
        qCWarning(texture) << "unsupported image format" << image.format();
        return;
    }

    auto result = Texture::create(format, {copy.width(), copy.height()}, {});
    const auto bytesPerLine = result.bytesPerLine();
    if (bytesPerLine < copy.bytesPerLine()) {
        qCWarning(texture) << Q_FUNC_INFO
                           << "texture line size is less than an image line size"
                           << result.bytesPerLine() << "<" << copy.bytesPerLine();
        return;
    }

    const auto data = result.imageData({});
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

Texture Texture::create(TextureFormat format, Texture::Size size, Texture::Dimentions dimentions, Texture::Alignment align)
{
    return Texture(
            TextureData::create(
                    format,
                    size.width(), size.height(), size.depth(),
                    dimentions.isCubemap(), dimentions.levels(), dimentions.layers(),
                    align));
}

Texture Texture::create(
        Data data,
        TextureFormat format,
        Size size,
        Dimentions dimentions,
        Alignment align,
        DataDeleter deleter)
{
    if (data.empty())
        return Texture();

    return Texture(
            TextureData::create(
                    format,
                    size.width(), size.height(), size.depth(),
                    dimentions.isCubemap(), dimentions.levels(), dimentions.layers(),
                    align,
                    data,
                    deleter));
}

qsizetype Texture::calculateBytesPerLine(TextureFormat format, int width, Alignment align)
{
    return TextureData::calculateBytesPerLine(
                TextureFormatInfo::formatInfo(format), quint32(width), align);
}

qsizetype Texture::calculateBytesPerSlice(TextureFormat format, int width, int height, Alignment align)
{
    return TextureData::calculateBytesPerSlice(
                TextureFormatInfo::formatInfo(format), quint32(width), quint32(height), align);
}

bool Texture::isNull() const
{
    return !d;
}

TextureFormat Texture::format() const
{
    return d ? d->format : TextureFormat::Invalid;
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
    return d ? TextureFormatInfo::formatInfo(d->format).bitsPerTexel() : 0;
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
    const auto data = dataImpl(index.face(), index.level(), index.layer());
    if (!data)
        return {};
    return {data, bytesPerImage(index.level())};
}

auto Texture::imageData(Index index) const -> ConstData
{
    const auto data = dataImpl(index.face(), index.level(), index.layer());
    if (!data)
        return {};
    return {data, bytesPerImage(index.level())};
}

auto Texture::constImageData(Index index) const -> ConstData
{
    const auto data = dataImpl(index.face(), index.level(), index.layer());
    if (!data)
        return {};
    return {data, bytesPerImage(index.level())};
}

Texture Texture::convert(Texture::Alignment align) const
{
    return convert(format(), align);
}

Texture Texture::convert(TextureFormat format) const
{
    return convert(format, alignment());
}

Texture Texture::convert(TextureFormat format, Texture::Alignment align) const
{
    if (!d)
        return Texture();

    if (format == d->format && align == d->align) // nothing changed
        return *this;

    auto result = Texture(
            TextureData::create(
                    format,
                    d->width, d->height, d->depth,
                    d->faces == 6, d->levels, d->layers,
                    align));

    if (result.isNull()) // allocation failed
        return Texture();

    decltype(TextureData::getFormatReader(d->format)) reader;
    decltype(TextureData::getFormatWriter(format)) writer;

    if (format != d->format) {
        reader = TextureData::getFormatReader(d->format);
        writer = TextureData::getFormatWriter(format);

        if (!reader) {
            qCWarning(texture) << "Converting is not supported for" << d->format;
            return Texture();
        }

        if (!writer) {
            qCWarning(texture) << "Converting is not supported for" << format;
            return Texture();
        }
    } else {
        if (isCompressed()) // changing alingment for compressed textures has no effect
            return *this;
    }

    const auto srcBytesPerTexel = this->bitsPerTexel() >> 3u;
    const auto dstBytesPerTexel = result.bitsPerTexel() >> 3u;

    const auto convertLine = [&](int width, ConstData srcLine, Data dstLine)
    {
        if (format != d->format) {
            Q_ASSERT(srcBytesPerTexel && dstBytesPerTexel);
            for (int x = 0; x < width; ++x) {
                const auto src = srcLine.subspan(srcBytesPerTexel * x, srcBytesPerTexel);
                const auto dst = dstLine.subspan(dstBytesPerTexel * x, dstBytesPerTexel);
                writer(dst, reader(src));
            }
        } else { // ok, only alingment changed, fast copy
            memoryCopy(dstLine, srcLine);
        }
    };

    for (int level = 0; level < d->levels; ++level) {
        const auto srcBytesPerSlice = d->bytesPerSlice(level);
        const auto dstBytesPerSlice = result.d->bytesPerSlice(level);
        const auto srcBytesPerLine = d->bytesPerLine(level);
        const auto dstBytesPerLine = result.d->bytesPerLine(level);
        for (int layer = 0; layer < d->layers; ++layer) {
            for (int face = 0; face < d->faces; ++face) {
                const auto srcData = imageData({Side(face), level, layer});
                const auto dstData = result.imageData({Side(face), level, layer});
                const auto width = d->levelWidth(level);
                const auto height = d->levelHeight(level);
                for (int z = 0; z < d->levelDepth(level); ++z) {
                    for (int y = 0; y < height; ++y) {
                        const auto srcLine = srcData.subspan(
                                    srcBytesPerSlice * z + srcBytesPerLine * y, srcBytesPerLine);
                        const auto dstLine = dstData.subspan(
                                    dstBytesPerSlice * z + dstBytesPerLine * y, dstBytesPerLine);
                        convertLine(width, srcLine, dstLine);
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
    Texture result(
            TextureData::create(
                    d->format,
                    d->width, d->height, d->depth,
                    d->faces == 6, d->levels, d->layers,
                    d->align));
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

    if (isCompressed()) {
        qCWarning(texture) << "Compressed images are not supported";
        return QImage();
    }

    Texture copy;
    QImage::Format imageFormat = QImage::Format_Invalid;
    switch (d->format) {
    case TextureFormat::A8_Unorm:
        imageFormat = QImage::Format_Alpha8;
        copy = convert(Alignment::Word);
        break;
    case TextureFormat::L8_Unorm:
        imageFormat = QImage::Format_Grayscale8;
        copy = convert(Alignment::Word);
        break;
    case TextureFormat::R8_Unorm:
    case TextureFormat::R16_Unorm:
    case TextureFormat::R16_Float:
    case TextureFormat::RG8_Unorm:
    case TextureFormat::RGB8_Unorm:
    case TextureFormat::BGR8_Unorm:
    case TextureFormat::R32_Float:
    case TextureFormat::RG16_Unorm:
    case TextureFormat::RG16_Float:
    case TextureFormat::RGBX8_Unorm:
    case TextureFormat::BGRX8_Unorm:
    case TextureFormat::RG32_Float:
        imageFormat = QImage::Format_RGB888;
        copy = convert(TextureFormat::RGB8_Unorm, Alignment::Word);
        break;
    case TextureFormat::LA8_Unorm:
    case TextureFormat::RGBA8_Unorm:
    case TextureFormat::BGRA8_Unorm:
    case TextureFormat::ABGR8_Unorm:
    case TextureFormat::RGBA16_Unorm:
    case TextureFormat::RGBA16_Float:
    case TextureFormat::RGBA32_Float:
        imageFormat = QImage::Format_RGBA8888;
        copy = convert(TextureFormat::RGBA8_Unorm, Alignment::Word);
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

    const auto data = copy.imageData({});
    for (int y = 0; y < d->height; ++y) {
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

    return d->data.get() + d->offset(side, level, layer);
}

const uchar* Texture::dataImpl(int side, int level, int layer) const
{
    if (!d)
        return nullptr;

    CHECK_SIDE(side, nullptr);
    CHECK_LEVEL(level, nullptr);
    CHECK_LAYER(layer, nullptr);

    return d->data.get() + d->offset(side, level, layer);
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

    return memoryCompare({lhs.d->data.get(), lhs.d->nbytes}, {rhs.d->data.get(), rhs.d->nbytes}) == 0;
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
                                  TextureFormat(format),
                                  int(width),
                                  int(height),
                                  int(depth),
                                  faces > 1,
                                  int(levels),
                                  int(layers),
                                  Texture::Alignment(align)));
        if (result.bytes() == data.size()) {
            memoryCopy(result.data(), {reinterpret_cast<const uchar *>(data.constData()), data.size()});
            texture = result;
        }
    }
    return stream;
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

QString toQString(Texture::Side side)
{
    const auto &mo = Texture::staticMetaObject;
    const auto index = mo.indexOfEnumerator("Side");
    const auto en = mo.enumerator(index);
    return en.valueToKey(int(side));
}

Q_LOGGING_CATEGORY(texture, "texturelib.texture")
