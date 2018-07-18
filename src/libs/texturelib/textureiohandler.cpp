#include "textureiohandler.h"

/*!
    \class TextureIOHandler

    Base class for all handlers.
*/

/*!
    Creates an TextureIOHandler object.
*/
TextureIOHandler::TextureIOHandler() = default;

/*!
    Destroys the TextureIOHandler object.
*/
TextureIOHandler::~TextureIOHandler() = default;

/*!
    \fn QIODevicePointer TextureIOHandler::device() const

    Returns the device currently assigned to the TextureIOHandler. If no device
    has been assigned, nullptr is returned.
*/

/*!
    \fn bool TextureIOHandler::canRead()

    Returns true if an image can be read from the device (i.e., the image format is supported,
    the device can be read from and the initial header information suggests that the image can be
    read); otherwise returns false.

    When reimplementing canRead(), make sure that the I/O device (device()) is left in its original
    state (e.g., by using peek() rather than read()).
*/

/*!
    \fn bool TextureIOHandler::readHeader(ImageHeader &header)

    Read an metadata from the device, and stores it in \a header. Returns true if the header
    is successfully read; otherwise returns false.

    This method is called before read() and should not read actual image data if possible.
*/

/*!
    \fn bool TextureIOHandler::read(ImageContents &contents)

    Read an image data from the device, and stores it in \a contents. Returns true if the contents
    is successfully read; otherwise returns false.
*/

/*!
    Writes the \a contents with the given \a options to the assigned device. Returns true on
    success; otherwise returns false.

    The default implementation does nothing, and simply returns false.
*/
bool TextureIOHandler::write(const Texture &texture)
{
    Q_UNUSED(texture);
    return false;
}

/*!
    \class TextureIOHandlerPlugin

    Base class for image handler plugins.

    Plugins serves as factories for TextureIOHandler objects.
*/

/*!
    \fn TextureIOHandlerPlugin::TextureIOHandlerPlugin()

    Constructs a new object with the given parent.
*/

/*!
    \fn QByteArray TextureIOHandlerPlugin::name() const

    Returns the name of this plugin.
*/

/*!
    \fn TextureIOHandler *TextureIOHandlerPlugin::create(QIODevice *device, const QMimeType &mimeType)

    Creates and returns a QTextureIOHandler subclass, with \a device and \a mimeType set.
    The mimeType must come from the values listed in the "MimeTypes" entry in
    the plugin metadata.
*/

/*!
    \fn ImageFormatInfo::Capabilities TextureIOHandlerPlugin::capabilities(QIODevice *device, const QMimeType &mimeType)

    Returns the capabilities of the plugin, based on the data in the \a device
    and the mime type \a mimeType. If device is 0, it should simply report
    whether the format can be read or written. Otherwise, it should attempt to
    determine whether the given mime type can be read from or written to device.
    It should do this without changing the state of device (typically by using
    QIODevice::peek()).
*/
