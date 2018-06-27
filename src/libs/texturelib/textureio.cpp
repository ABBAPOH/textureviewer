#include "textureio.h"

#include "textureiohandler.h"
#include "textureiohandlerdatabase.h"
#include "textureioresult.h"

#include <experimental/optional>

#include <QtCore/QFile>
#include <QtCore/QMimeDatabase>
#include <QDebug>

using Capability = TextureIOHandlerPlugin::Capability;
using Capabilities = TextureIOHandlerPlugin::Capabilities;

static inline QIODevice::OpenMode caps2OpenMode(Capabilities caps)
{
    QIODevice::OpenMode result;
    if (caps & Capability::CanRead)
        result |= QIODevice::ReadOnly;
    if (caps & Capability::CanWrite)
        result |= QIODevice::WriteOnly;
    return result;
}

class TextureIOPrivate
{
    Q_DECLARE_PUBLIC(TextureIO)
    TextureIO *q_ptr {nullptr};

public:
    explicit TextureIOPrivate(TextureIO *qq) : q_ptr(qq) {}

    TextureIOResult ensureDeviceOpened(Capabilities caps);
    TextureIOResult ensureHandlerCreated(Capabilities caps);
    void resetHandler();

    std::unique_ptr<TextureIOHandler> handler;

    QString fileName;
    std::unique_ptr<QFile> file;

    QIODevice *device {nullptr};
    std::experimental::optional<QMimeType> mimeType;
};

TextureIOResult TextureIOPrivate::ensureDeviceOpened(Capabilities caps)
{
    if (!device)
        return TextureIOResult::Status::DeviceError;

    const auto mode = caps2OpenMode(caps);

    if ((mode & QIODevice::ReadOnly) && file && !file->exists())
        return TextureIOResult::Status::FileNotFound;

    if (!(device->openMode() & mode)) {
        if (!device->open(mode | device->openMode()))
            return TextureIOResult::Status::DeviceError;
    }

    return TextureIOResult::Status::Ok;
}

TextureIOResult TextureIOPrivate::ensureHandlerCreated(Capabilities caps)
{
    const auto ok = ensureDeviceOpened(caps);
    if (!ok)
        return ok;

    if (handler)
        return TextureIOResult::Status::Ok;

    auto mt = QMimeType();
    if (!mimeType) {
        // mimeType is not set, try to guess from file
        if ((device->openMode() & QIODevice::ReadOnly))
            mt = QMimeDatabase().mimeTypeForData(device->peek(256));
    } else {
        mt = *mimeType;
    }

    if (!mt.isValid())
        return TextureIOResult::Status::InvalidMimeType;

    auto db = TextureIOHandlerDatabase::instance();
    handler = db->create(device, mt, caps);
    if (!handler)
        return TextureIOResult::Status::UnsupportedMimeType;

    return TextureIOResult::Status::Ok;
}

void TextureIOPrivate::resetHandler()
{
    handler.reset();
}

/*!
    \class TextureIO
    Basic Image unput/output.

    This class is used for a customised image reading/writing.
*/

/*!
    Creates an TextureIO object.
*/
TextureIO::TextureIO() :
    d_ptr(new TextureIOPrivate(this))
{
}

/*!
    Creates an TextureIO object with the given \a fileName and \a mimeType.
*/
TextureIO::TextureIO(const QString &fileName, const QMimeType &mimeType) :
    d_ptr(new TextureIOPrivate(this))
{
    setFileName(fileName);
    if (mimeType.isValid()) // invalid mimetype will overwrite detected type
        setMimeType(mimeType);
}

/*!
    Creates an TextureIO object with the given \a device and \a mimeType.
*/
TextureIO::TextureIO(QIODevice *device, const QMimeType &mimeType) :
    d_ptr(new TextureIOPrivate(this))
{
    setDevice(device);
    setMimeType(mimeType);
}

/*!
    Creates an TextureIO object with the given \a fileName and \a mimeType.
*/
TextureIO::TextureIO(const QString &fileName, const QString &mimeType) :
    d_ptr(new TextureIOPrivate(this))
{
    setFileName(fileName);
    if (!mimeType.isEmpty()) // invalid mimetype will overwrite detected type
        setMimeType(mimeType);
}

/*!
    Creates an TextureIO object with the given \a device and \a mimeType.
*/
TextureIO::TextureIO(QIODevice *device, const QString &mimeType) :
    d_ptr(new TextureIOPrivate(this))
{
    setDevice(device);
    setMimeType(mimeType);
}

/*!
    Destroys TextureIO object.
*/
TextureIO::~TextureIO()
{
}

/*!
    \property TextureIO::fileName
    This property holds the filename that is set to this TextureIO object.
*/

QString TextureIO::fileName() const
{
    Q_D(const TextureIO);

    return d->fileName;
}

void TextureIO::setFileName(const QString &fileName)
{
    Q_D(TextureIO);

    if (d->fileName == fileName)
        return;

    d->file.reset(new QFile(fileName));
    d->device = d->file.get();
    d->mimeType = QMimeDatabase().mimeTypeForFile(fileName);
    d->fileName = fileName;
    d->resetHandler();
}

/*!
    \property TextureIO::device
    This property holds the device that is set to this TextureIO object.
*/

QIODevice *TextureIO::device() const
{
    Q_D(const TextureIO);

    return d->device;
}

void TextureIO::setDevice(QIODevice *device)
{
    Q_D(TextureIO);

    d->file.reset();
    d->device = device;
    d->resetHandler();
}

/*!
    \property TextureIO::mimeType
    This property holds the mime type that is set to this TextureIO object.

    If no mime type is set, it is automatically determined from the device contents before reading.
*/

QMimeType TextureIO::mimeType() const
{
    Q_D(const TextureIO);
    return d->mimeType ? *d->mimeType : QMimeType();
}

void TextureIO::setMimeType(const QMimeType &mimeType)
{
    Q_D(TextureIO);
    if (mimeType.isValid())
        d->mimeType = mimeType;
    else
        d->mimeType = std::experimental::nullopt;
    d->resetHandler();
}

void TextureIO::setMimeType(const QString &mimeType)
{
    Q_D(TextureIO);
    if (!mimeType.isEmpty())
        // here we can have optional pointing to an invalid QMimeType,
        // leads to error in ensureHandlerCreated
        d->mimeType = QMimeDatabase().mimeTypeForName(mimeType);
    else
        d->mimeType = std::experimental::nullopt;
    d->resetHandler();
}

/*!
    Reads the contents of an texture file.
*/
std::pair<TextureIOResult, Texture> TextureIO::read()
{
    Q_D(TextureIO);

    auto ok = d->ensureHandlerCreated(Capability::CanRead);
    if (!ok)
        return {ok, Texture()};

    Texture texture;
    if (!d->handler->read(texture))
        ok = TextureIOResult::Status::HandlerError;

    return {ok, ok ? texture : Texture()};
}

/*!
    Writes the given \a contents with the given \a options to the device.

    Returns the status of the operation.
*/
TextureIOResult TextureIO::write(const Texture &contents)
{
    Q_D(TextureIO);
    auto ok = d->ensureHandlerCreated(Capability::CanWrite);
    if (!ok)
        return ok;

    if (!d->handler->write(contents))
        return TextureIOResult::Status::HandlerError;

    if (d->file)
        d->file->flush();
    return TextureIOResult::Status::Ok;
}

QString TextureIO::pluginsDirPath()
{
#if defined(Q_OS_LINUX)
    return QStringLiteral("/../lib/textureviewer/plugins/");
#elif defined(Q_OS_MAC)
    return QStringLiteral("/../PlugIns/");
#elif defined(Q_OS_WIN)
    return QStringLiteral("/plugins/");
#else
    return QString();
#endif
}
