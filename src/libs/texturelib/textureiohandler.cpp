#include "textureiohandler.h"

/*!
    \class TextureIOHandler

    Base class for all handlers.
*/

/*!
    \fn TextureIOHandler::TextureIOHandler() noexcept

    Constructs a TextureIOHandler object.
*/

/*!
    Destroys the TextureIOHandler object.
*/
TextureIOHandler::~TextureIOHandler() noexcept = default;

/*!
    \fn QIODevicePointer TextureIOHandler::device() const

    \brief Returns the device currently assigned to the TextureIOHandler.

    If no device has been assigned, nullptr is returned.
*/

/*!
    \fn bool TextureIOHandler::read(Texture &texture)

    Reimplement this function to read a texture data from the device.

    The read data is stored to the given \a texture. Should return true if the data is successfully
    read; otherwise should return false.

    Specific error can be logged to the stderr.
*/

/*!
    Reimplement this function to write the given \a texture data to the device.

    Should return true if the data is successfully written; otherwise should return false.

    Specific error can be logged to the stderr.

    The default implementation does nothing, and simply returns false.
*/
bool TextureIOHandler::write(const Texture &texture)
{
    Q_UNUSED(texture);
    return false;
}
