#pragma once

#include <QtCore/QString>
#include <exception>
#include <QException>

namespace TextureTool {

class Exception: public std::exception
{
public:
    Exception();
    ~Exception() Q_DECL_NOEXCEPT override;
    const char *what() const Q_DECL_NOEXCEPT override;
};

class ExitException: public Exception
{
public:
    ExitException(int code = 0);
    ~ExitException() Q_DECL_NOEXCEPT override;
    inline int code() const Q_DECL_NOEXCEPT { return _code; }
    const char *what() const Q_DECL_NOEXCEPT override;

private:
    int _code {0};
};

class RuntimeError: public Exception
{
public:
    explicit RuntimeError(const QString &message);
    explicit RuntimeError(QString &&message) Q_DECL_NOEXCEPT;
    ~RuntimeError() Q_DECL_NOEXCEPT override;

    const QString &message() const Q_DECL_NOEXCEPT;
    const char *what() const Q_DECL_NOEXCEPT override;

private:
    QString _message;
};

} // namespace TextureTool
