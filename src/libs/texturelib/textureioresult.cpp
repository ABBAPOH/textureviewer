#include "textureioresult.h"

/*!
    Returns the human-readable message describing the status.
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
    This is a helper class that wraps status enum.
*/

/*!
    \enum TextureIOStatus
    This enum describes the different types of errors that can occur when
    reading image files.

    \var TextureIOResult::InvalidMimeType
    ImageIO was used with an invalid mime type.

    \var TextureIOResult::FileNotFound
    ImageIO was used with a file name, but no file was found with that name.

    \var TextureIOResult::DeviceError
    ImageIO encountered a device error when reading the image.
    You can consult your particular device for more details on what went wrong.

    \var TextureIOResult::UnsupportedMimeType
    ImageIO does not support the requested mime type.

    \var TextureIOResult::HandlerError
    Error occured within an undelying ImageIOHandler.
*/

/*!
    \fn TextureIOResult::TextureIOResult(Status status = Status::Ok)
    Constructs TextureIOResult with the given \a status code.
*/

/*!
    \fn TextureIOResult::TextureIOResult(const TextureIOResult &lhs)
    Copy-constructs TextureIOResult with the given \a lhs TextureIOResult.
*/

/*!
    \fn TextureIOResult::~TextureIOResult
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
    \fn TextureIOStatus TextureIOStatus() const
    Returns status code.
*/

/*!
    \fn TextureIOResult::operator bool() const
    Returns true if errorCode is equal to TextureIOStatus::Ok.

    \sa toBool(), bool TextureIOResult::operator!()
*/

/*!
    \fn bool TextureIOResult::operator!() const
    Returns true if errorCode is not equal to TextureIOStatus::Ok.

    \sa toBool(), TextureIOResult::operator bool()
*/

/*!
    \fn inline bool operator==(const TextureIOResult &lhs, const TextureIOResult &rhs)
    \related TextureIOResult
    Returns true if \a lhs status() is equal to the \a rhs status().
*/

/*!
    \fn inline bool operator!=(const TextureIOResult &lhs, const TextureIOResult &rhs)
    \related TextureIOResult
    Returns true if \a lhs status() is not equal to the \a rhs status().
*/

/*!
    Returns the human-readable message describing the \a result.
*/
QString toUserString(TextureIOResult result)
{
    return result ? TextureIOResult::tr("No error") : toUserString(result.error());
}
