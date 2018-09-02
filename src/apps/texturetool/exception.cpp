#include "exception.h"

namespace TextureTool {

/*!
    \class Exception
    Base class for TextureTool's exceptions
*/

/*!
    \fn Exception::Exception()
    Constructs an Exception object.
*/

/*!
    Destroys the Exception object.
*/
Exception::~Exception() noexcept = default;

/*!
    \brief Returns the name of the class.
    \reimp
*/
const char *Exception::what() const noexcept
{
    return "Exception";
}

/*!
    \class ExitException
    This exception is used in command line parsers to terminate the program.
    We don't rely on exit(0) call because stack is not unwinded and destructors are not called,
    so we throw this exception, and catch it in main().
*/

/*!
    \fn ExitException::ExitException(int code = 0);
    Constructs ExitException object with the given \a code.
*/

/*!
    Destroys the ExitException object.
*/
ExitException::~ExitException() noexcept = default;

/*!
    \fn int ExitException::code() const noexcept;
    Returns the code that should be return from the program.
*/

/*!
    \reimp
*/
const char *ExitException::what() const noexcept
{
    return "ExitException";
}

/*!
    \class RuntimeError
    Exception with a localized string describing occured error.
*/

/*!
    \fn RuntimeError::RuntimeError(const QString &message)
    Constructs RuntimeError object with the given \a message.
*/

/*!
    \fn RuntimeError::RuntimeError(QString &&message)
    Move-constructs RuntimeError object with the given \a message.
*/

/*!
    Destroys the RuntimeError object.
*/
RuntimeError::~RuntimeError() noexcept = default;

/*!
    \fn const QString &RuntimeError::message() const noexcept
    Returns the message describing the error.
*/

/*!
    \reimp
*/
const char *RuntimeError::what() const noexcept
{
    return "RuntimeError";
}

} // namespace TextureTool
