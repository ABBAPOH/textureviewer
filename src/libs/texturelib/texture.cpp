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
    const auto bytesPerTexel = size_t(format.bytesPerTexel());
    const auto blockSize = size_t(format.blockSize());

    if (bytesPerTexel) {
        if (bytesPerTexel && std::numeric_limits<qsizetype>::max() / bytesPerTexel / width < 1) {
            qCWarning(texture) << "potential integer overflow";
            return 0;
        }
        if (align == Texture::Alignment::Byte)
            return qsizetype(width * bytesPerTexel);
        if (align == Texture::Alignment::Word)
            return qsizetype(((width * bytesPerTexel + 3u) >> 2u) << 2u);
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

    if (format.bytesPerTexel()) {
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

/*!
  \enum Texture::Side

  This enum describes possible sides of a cubemap.
  \var Texture::Side Texture::PositiveX
  Positive X side
  \var Texture::Side Texture::NegativeX
  Negative X side
  \var Texture::Side Texture::PositiveY
  Positive Y side
  \var Texture::Side Texture::NegativeY
  Negative Y side
  \var Texture::Side Texture::PositiveZ
  Positive Z side
  \var Texture::Side Texture::NegativeZ
  Negative Z side
*/

/*!
  \enum Texture::Alignment
  This enum describes texture data alignment

  \var Texture::Alignment Texture::Byte
  One byte alignment

  \var Texture::Alignment Texture::Word
  Four byte alignment
*/

/*!
  \enum Texture::IsCubemap
  This enum is a simply strict bool used to prevent using unnamed bools

  \var Texture::IsCubemap Texture::No
  Aka false

  \var Texture::IsCubemap Texture::Yes
  Aka true
*/

/*!
  \class Texture::Size
  \brief Helper class used in Texture constructors.

  This class simplifies construction of the Texture instances by encapsulating 1D, 2D and 3D sizes
  in a single parameter passed to a constructor.

  So, the texture can be created as follows.
  \snippet texture/texture.cpp 0

  Or, in a more verbose way
  \snippet texture/texture.cpp 1
*/

/*!
  \fn Texture::Size::Size() noexcept
  \brief Constructs a null size.

  \sa isNull(), isValid()
*/

/*!
  \fn Texture::Size::Size(int width) noexcept
  \brief Constructs a Size instance with the given \a width.
*/

/*!
  \fn Texture::Size::Size(int width, int height) noexcept
  \brief Constructs a Size instance with the given \a width and \a height.
*/

/*!
  \fn Texture::Size::Size(int width, int height, int depth) noexcept
  \brief Constructs a Size instance with the given \a width, \a height and \a depth.
*/

/*!
  \property bool Texture::Size::isNull
  \brief This property holds if the Size instance is null.

  A null instance has zero width, height and depth.
*/

/*!
  \property bool Texture::Size::isValid
  \brief This property holds if the Size instance is valid.

  A valid instance has width, height and depth greater than zero.
*/

/*!
  \fn bool Texture::Size::width() const noexcept
  \brief Returns the width.

  \sa Texture::Size::setWidth() Texture::Size::height, Texture::Size::depth
*/

/*!
  \fn void Texture::Size::setWidth(int w) noexcept
  \brief Sets the width.

  \sa Texture::Size::width() Texture::Size::setHeight, Texture::Size::setDepth
*/

/*!
  \fn bool Texture::Size::height() const noexcept
  \brief Returns the height.

  \sa Texture::Size::width, Texture::Size::depth
*/

/*!
  \fn void Texture::Size::setHeight(int h) noexcept
  \brief Sets the height.

  \sa Texture::Size::height() Texture::Size::setWidth, Texture::Size::setDepth
*/

/*!
  \fn bool Texture::Size::depth() const noexcept
  \brief Returns the height.

  \sa Texture::Size::width, Texture::Size::heigth
*/

/*!
  \fn void Texture::Size::setDepth(int d) noexcept
  \brief Sets the depth.

  \sa Texture::Size::depth() Texture::Size::setWidth, Texture::Size::setHeight
*/

/*!
  \class Texture::ArraySize
  \brief Helper class used in Texture constructors.

  This class simplifies construction of the Texture instances by encapsulating levels, layers and
  faces dimensions in a single parameter passed to a constructor.

  So, the texture can be created as follows.
  \snippet texture/texture.cpp 2
*/

/*!
  \fn Texture::ArraySize::ArraySize() noexcept
  \brief Constructs a null size.

  \sa isNull(), isValid()
*/

/*!
  \fn Texture::ArraySize::ArraySize(int levels, int layers = 1) noexcept
  \brief Constructs an ArraySize instance with the given \a levels and \a layers.
*/

/*!
  \fn Texture::ArraySize::ArraySize(IsCubemap isCumemap, int levels = 1, int layers = 1) noexcept
  \brief Constructs an ArraySize instance with the given \a isCumemap, \a levels and \a layers.
*/

/*!
  \property bool Texture::ArraySize::isNull
  \brief This property holds if the ArraySize instance is null.

  A null instance has zero faces, levels and layers.
*/

/*!
  \property bool Texture::ArraySize::isValid
  \brief This property holds if the ArraySize instance is valid.

  A valid instance has faces, levels and layers greater than zero.
*/

/*!
  \fn bool Texture::ArraySize::faces() const noexcept
  \brief Return the faces count.
*/

/*!
  \fn bool Texture::ArraySize::levels() const noexcept
  \brief Returns the levels count.
*/

/*!
  \fn void Texture::ArraySize::setLevels(int levels) noexcept
  \brief Sets the levels count.
*/

/*!
  \fn bool Texture::ArraySize::layers() const noexcept
  \brief Returns the layers count.
*/

/*!
  \fn void Texture::ArraySize::setLayers(int layers) noexcept
  \brief Sets the layers count.
*/

/*!
  \class Texture::ArrayIndex
  \brief Helper class used in Texture::imageData() function.

  This class simplifies access to the raw data of the texture by encapsulating level, layer and
  face dimensions in a single parameter. This allows to have only 2 overloads of the
  Texture::imageData() function.
*/

/*!
  \fn Texture::ArrayIndex::ArrayIndex(int level = 0, int layer = 0) noexcept
  \brief Constructs an ArrayIndex instance with the given \a level and \a layer.
*/

/*!
  \fn Texture::ArrayIndex::ArrayIndex(Texture::Side side, int level = 0, int layer = 0) noexcept
  \brief Constructs an ArrayIndex instance with the given \a side, \a level and \a layer.

  Passing \a side that is out of Texture::Side enum results to undefined behavior.
*/

/*!
  \fn bool Texture::ArrayIndex::isNull() const noexcept
  \brief Returns true if the ArrayIndex instance is null.

  A null instance has zero face, level and layer.
*/

/*!
  \fn bool Texture::ArrayIndex::isValid() const noexcept
  \brief Returns true if the ArrayIndex instance is valid.

  A valid instance has face, level and layer greater than or equal to zero.
*/

/*!
  \fn Texture::Side Texture::ArrayIndex::side() const noexcept
  \brief Returns the side.
*/

/*!
  \fn void Texture::ArrayIndex::setSide(Texure::Side side) noexcept
  \brief Sets the side.
*/

/*!
  \fn int Texture::ArrayIndex::face() const noexcept
  \brief Returns the face.
*/

/*!
  \fn int Texture::ArrayIndex::level() const noexcept
  \brief Returns the level.
*/

/*!
  \fn void Texture::ArrayIndex::setLevel(int level) noexcept
  \brief Sets the level.
*/

/*!
  \fn int Texture::ArrayIndex::layer() const noexcept
  \brief Returns the layer.
*/

/*!
  \fn void Texture::ArrayIndex::setLayer(int layer) noexcept
  \brief Sets the layer.
*/

/*!
  \typedef Texture::Data
  Type alias for a gsl::span\<uchar\>

  A view to a mutable data.
*/

/*!
  \typedef Texture::ConstData
  Type alias for a gsl::span\<const uchar\>

  A view to a constant data.
*/

/*!
  \class Texture
  \brief Texture is a hardware-independent container for a pixel data.

  Unlike QOpenGLTexture, Texture is a simple storage (an array of bytes) and is not tied to any
  specific API. However, it can be converted to QOpenGLTexture via Utils::makeOpenGLTexture() function.

  Texture is a multi-dimentional array of "images" (which can be 1d, 2d or 3d). There are 3
  dimentions - levels (for mimpamps), layers (for texture arrays) and faces (for faces of the cubemap).

  Helper classes are used to simplify Texture construction - the sizes are packed in a Texture::Size
  (so you can create 1d, 2d or 3d Texture instances without 3 overloads for each type) and texture
  dimensions are packed in an Texture::ArraySize (so you can create mipmaped textures, texture array
  and/or cubemaps).

  Texture provides access to the internal data via data() and imageData() functions.

  \note For cubemaps, width should be same as height and depth should be equal to 1.
  \note For 3d textures, arrays are not supported as no format does so.
*/

/*!
  \fn Texture::Texture() noexcept
  \brief Constructs a null texture

  \sa isNull()
*/

/*!
  \brief Constructs a shallow copy of the \a other texture.
*/
Texture::Texture(const Texture &other)
    : d(other.d)
{
    if (d)
        d->ref.ref();
}

/*!
  \fn Texture::Texture(Texture &&other) noexcept
  \brief Move-constructs a Texture instance with the given \a other Texture.
*/

/*!
  \brief Constructs a Texture instance with the contents of the \a file.

  In case of an error, a null texture is constructed and error is logged to the stderr.

  \sa isNull()
*/
Texture::Texture(const QString &file)
    : d(nullptr)
{
    auto ok = load(file);
    if (!ok)
        qCWarning(texture) << "Can't load texture from file" << file << toUserString(ok);
}

/*!
  \brief Constructs a Texture instance with the contents of the \a file.

  In case of an error, a null texture is constructed and error is logged to the stderr.

  \sa isNull()
*/
Texture::Texture(QStringView file) :
    Texture(file.toString())
{
}

/*!
  \brief Constructs a Texture instance with the contents of the \a image.

  An image is converted to a single 2D texture with no mipmaps.

  If image has aplha, the constucted texture will have TextureFormat::RGBA8_Unorm format.

  If image has no aplha, the constucted texture will have TextureFormat::RGB8_Unorm format.

  \note Currently, only images with QImage::Format_ARGB32 or QImage::Format_RGB888 are supported.
  In other cases, null texture is constructed.

  \sa isNull()
*/

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

    auto result = Texture(format, {copy.width(), copy.height()});
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

/*!
  \brief Constructs a Texture instance with the given \a format, \a size, \a dimensions and \a align.

  Passing invalid parameters leads to construction of a null texture; an exact error is logged to
  stderr. Invalid parameters are sizes less than zero, or too big to fit whole texture in memory.

  \sa isNull()
*/

Texture::Texture(
        TextureFormat format,
        Size size,
        ArraySize dimensions,
        Texture::Alignment align)
{
    d = TextureData::create(
            format,
            size.width(), size.height(), size.depth(),
            dimensions.isCubemap(), dimensions.levels(), dimensions.layers(),
            align);
}

/*!
  \brief Constructs a Texture instance with the given \a data, \a format, \a size, \a dimensions and \a align.

  Passing invalid parameters leads to construction of a null texture; an exact error is logged to
  stderr. Invalid parameters are sizes less than zero, or too big to fit whole texture in memory.

  This constructor doesn't allocate memory but instead uses the given \a data.
  Data must have the size matching given parameters.

  The default destructor (i.e. delete [] data) is used for cleaning the data.

  \sa isNull()
*/

Texture::Texture(
        Data data,
        TextureFormat format,
        Size size,
        ArraySize dimensions,
        Texture::Alignment align)
{
    if (data.empty())
        return;

    d = TextureData::create(
            format,
            size.width(), size.height(), size.depth(),
            dimensions.isCubemap(), dimensions.levels(), dimensions.layers(),
            align,
            data);
}

/*!
  \brief Constructs a Texture instance with the given \a data, \a deleter, \a format, \a size, \a dimensions and \a align.

  Passing invalid parameters leads to construction of a null texture; an exact error is logged to
  stderr. Invalid parameters are sizes less than zero, or too big to fit whole texture in memory.

  This constructor doesn't allocate memory but instead uses the given \a data.
  Data must have the size matching given parameters.

  The \a deleter is used for cleaning the data. You can pass an empty function to prevent data from
  cleaning. This trick can be used to implement sharing of the data between different Texture
  instances.

  \sa isNull()
*/

Texture::Texture(
        Data data,
        Texture::DataDeleter deleter,
        TextureFormat format,
        Size size,
        ArraySize dimensions,
        Texture::Alignment align)
{
    if (data.empty())
        return;

    d = TextureData::create(
            format,
            size.width(), size.height(), size.depth(),
            dimensions.isCubemap(), dimensions.levels(), dimensions.layers(),
            align,
            data,
            deleter);
}

/*!
  \brief Destroys the Texture instance.
*/
Texture::~Texture()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
  \brief Assigns a shallow copy of the given \a other texture to this texture and returns a reference to this texture.
*/
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

/*!
  \fn Texture &Texture::operator=(Texture &&other) noexcept
  \brief Move-assigns \a other to this Texture instance.
*/

/*!
  \brief Calculates an amount of bytes required for the line of the \a given format, \a width and \a align.
*/
qsizetype Texture::calculateBytesPerLine(TextureFormat format, int width, Alignment align)
{
    return TextureData::calculateBytesPerLine(
                TextureFormatInfo::formatInfo(format), quint32(width), align);
}

/*!
  \brief Calculates an amount of bytes required for the slice of the given \a format, \a width and \a align.
*/
qsizetype Texture::calculateBytesPerSlice(TextureFormat format, int width, int height, Alignment align)
{
    return TextureData::calculateBytesPerSlice(
                TextureFormatInfo::formatInfo(format), quint32(width), quint32(height), align);
}

/*!
  \brief Returns true if this instance is a null texture.

  Null texture has no allocated data and all properties are null
*/
bool Texture::isNull() const
{
    return !d;
}

/*!
  \brief Returns the format of the texture.
*/
TextureFormat Texture::format() const
{
    return d ? d->format : TextureFormat::Invalid;
}

/*!
  \brief Returns the alignment of the texture data.

  The data can have different alignment meaning that each line of the data is aligned to a specific
  boundary.
*/
Texture::Alignment Texture::alignment() const
{
    return d ? d->align : Alignment::Byte;
}

/*!
  \brief Returns true if the texture has one of the compressed formats.
*/
bool Texture::isCompressed() const
{
    return d && d->compressed;
}

/*!
  \brief Returns the width of the biggest mipmap.
*/
int Texture::width() const
{
    return d ? d->width : 0;
}

/*!
  \brief Returns the width of the mipmap at the given \a level.
*/
int Texture::width(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->levelWidth(level);
}

/*!
  \brief Returns the height of the biggest mipmap.
*/
int Texture::height() const
{
    return d ? d->height : 0;
}

/*!
  \brief Returns the height of the mipmap at the given \a level.
*/
int Texture::height(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->levelHeight(level);
}

/*!
  \brief Returns the depth of the biggest mipmap.
*/
int Texture::depth() const
{
    return d ? d->depth : 0;
}

/*!
  \brief Returns the depth of the mipmap at the given \a level.
*/
int Texture::depth(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->levelDepth(level);
}

/*!
  \brief Returns the width, height and depth of the biggest mipmap packed in a Texture::Size
  object.
*/
Texture::Size Texture::size() const
{
    return d ? Size(d->width, d->height, d->depth) : Size();
}

/*!
  \brief Returns the width, height and depth of the mipmap at the given \a level packed in a
  Texture::Size object.
*/
Texture::Size Texture::size(int level) const
{
    if (!d)
        return Size();

    CHECK_LEVEL(level, Size());

    return Size(d->levelWidth(level), d->levelHeight(level), d->levelDepth(level));
}

/*!
  \brief Returns the faces count.
*/
int Texture::faces() const
{
    return d ? d->faces : 0;
}

/*!
  \brief Returns the levels count.
*/
int Texture::levels() const
{
    return d ? d->levels : 0;
}

/*!
  \brief Returns the layers count.
*/
int Texture::layers() const
{
    return d ? d->layers : 0;
}

/*!
  \brief Returns the texture dimensions.
*/
Texture::ArraySize Texture::arraySize() const
{
    return d ? ArraySize(IsCubemap(d->faces == 6), d->levels, d->layers) : ArraySize();
}

/*!
  \brief Returns the total size of the texture data in bytes.
*/
qsizetype Texture::bytes() const
{
    return d ? d->nbytes : 0;
}

/*!
  \brief Returns the amount of bytes per one texel (texture pixel).
*/
qsizetype Texture::bytesPerTexel() const
{
    return d ? TextureFormatInfo::formatInfo(d->format).bytesPerTexel() : 0;
}

/*!
  \brief Returns the amount of bytes per line of the mipmap at the given \a level.
*/
qsizetype Texture::bytesPerLine(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->bytesPerLine(level);
}

/*!
  \brief Returns the amount of bytes per slice of the mipmap at the given \a level.
*/
qsizetype Texture::bytesPerSlice(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->bytesPerSlice(level);
}

/*!
  \brief Returns the amount of bytes used by the whole mipmap at the given \a level.
*/
qsizetype Texture::bytesPerImage(int level) const
{
    if (!d)
        return 0;

    CHECK_LEVEL(level, 0);

    return d->bytesPerImage(level);
}

/*!
  \brief Returns the data of the image at the given \a index.
*/
auto Texture::imageData(ArrayIndex index) -> Data
{
    const auto data = dataImpl(index.face(), index.level(), index.layer());
    if (!data)
        return {};
    return {data, bytesPerImage(index.level())};
}

/*!
  \brief Returns the constant data of the image at the given \a index.
*/
auto Texture::imageData(ArrayIndex index) const -> ConstData
{
    const auto data = dataImpl(index.face(), index.level(), index.layer());
    if (!data)
        return {};
    return {data, bytesPerImage(index.level())};
}

/*!
  \brief Returns the constant data of the image at the given \a index.
*/
auto Texture::constImageData(ArrayIndex index) const -> ConstData
{
    const auto data = dataImpl(index.face(), index.level(), index.layer());
    if (!data)
        return {};
    return {data, bytesPerImage(index.level())};
}

/*!
  \brief Converts this texture to a texture with the given \a alignment.

  The format of the resulting texture is the same as the format of this texture.
*/
Texture Texture::convert(Texture::Alignment align) const
{
    return convert(format(), align);
}

/*!
  \brief Converts this texture to a texture with the given \a format.

  The alignment of the resulting texture is the same as the alignment of this texture.
*/
Texture Texture::convert(TextureFormat format) const
{
    return convert(format, alignment());
}

/*!
  \brief Converts this texture to a texture with the given \a format and \a align.
*/
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

    const auto srcBytesPerTexel = this->bytesPerTexel() >> 3u;
    const auto dstBytesPerTexel = result.bytesPerTexel() >> 3u;

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
                const auto depth = d->levelDepth(level);
                for (int z = 0; z < depth; ++z) {
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
  \brief Performs a deep-copying of this texture
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

/*!
  \brief Converts this texture to a QImage.
*/
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

    const auto bytesPerLine = copy.bytesPerLine();
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

/*!
  \brief Loads a texture from the given \a file.
*/
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

/*!
  \brief Saves this texture to the given \a file.
*/
TextureIOResult Texture::save(const QString &file)
{
    TextureIO io(file);
    return io.write(*this);
}

Texture::Texture(TextureData *dd)
    : d(dd)
{
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

QDebug operator<<(QDebug& d, const Texture::ArrayIndex& index)
{
    auto s = QString("Texture::ArrayIndex(face = %1, level = %2, layer = %3)").arg(
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
