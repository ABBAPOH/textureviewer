#include "textureioresult.h"

/*!
    \enum TextureIOError
    This enum descrives possible io errors that occur when reading and saving textures.

    \var TextureIOError InvalidMimeType
    Invalid mimetype is passed to the TextureIO
    \var TextureIOError FileNotFound
    TextureIO was used with a file name, but no file was found with that name.
    \var TextureIOError DeviceError
    TextureIO encountered a device error when performing an I/O operaton. You can consult your
    particular device for more details on what went wrong.
    \var TextureIOError UnsupportedMimeType
    The requested mime type is not supported.
    \var TextureIOError HandlerError
    An error occured within a handler, which means that data is corrupted (when reading) or the
    passed texture can't be saved (when writing). See stderr to see the details.
*/

/*!
    Returns the human-readable message describing the status.
    \related TextureIOError
*/
QString toUserString(TextureIOError status)
{
    switch (status) {
    case TextureIOError::InvalidMimeType:
        return TextureIOResult::tr("Invalid mimetype");
    case TextureIOError::FileNotFound:
        return TextureIOResult::tr("File not found");
    case TextureIOError::DeviceError:
        return TextureIOResult::tr("Device error");
    case TextureIOError::UnsupportedMimeType:
        return TextureIOResult::tr("Unsupported format");
    case TextureIOError::HandlerError:
        return TextureIOResult::tr("Handler error");
    }
    return QString();
}

/*!
    \class TextureIOResult

    This class is an emulation of the std::excected<TextureIOError> typedef.

    Such emulation is needed because it is not sure when and if we have the std::excected class.
*/

/*!
    \fn TextureIOResult::TextureIOResult() noexcept
    Constructs TextureIOResult instance with no error.
*/

/*!
    \fn TextureIOResult::TextureIOResult(TextureIOError error) noexcept
    Constructs TextureIOResult with the given \a error.
*/

/*!
    \fn TextureIOResult::TextureIOResult(const TextureIOResult &lhs)
    Copy-constructs TextureIOResult with the given \a lhs TextureIOResult.
*/

/*!
    \fn TextureIOResult::~TextureIOResult() noexcept
    Destroys TextureIOResult object.
*/

/*!
    Assigns \a other TextureIOResult to this result.
*/
TextureIOResult &TextureIOResult::operator=(const TextureIOResult& other) noexcept
{
    if (this == &other)
        return *this;
    m_value = other.m_value;
    return *this;
}

/*!
    \fn TextureIOError TextureIOResult::error() const
    Returns error code.
*/

/*!
    \fn TextureIOResult::operator bool() const noexcept
    Returns true if this instance contains no error.

    \sa toBool(), bool TextureIOResult::operator!()
*/

/*!
    \fn bool TextureIOResult::operator!() const noexcept
    Returns true if this instance has an error.

    \sa toBool(), TextureIOResult::operator bool()
*/

/*!
    \fn inline bool operator==(const TextureIOResult &lhs, const TextureIOResult &rhs)
    \related TextureIOResult
    Returns true if \a lhs is equal to the \a rhs.
*/

/*!
    \fn inline bool operator!=(const TextureIOResult &lhs, const TextureIOResult &rhs)
    \related TextureIOResult
    Returns true if \a lhs is not equal to the \a rhs.
*/

/*!
    Returns the human-readable message describing the \a result.
    \related TextureIOResult
*/
QString toUserString(const TextureIOResult &result)
{
    return result ? TextureIOResult::tr("No error") : toUserString(result.error());
}
