#pragma once

#include <QtCore/QString>
#include <exception>
#include <QException>

namespace TextureTool {

class Exception: public std::exception
{
public:
    Exception();
    ~Exception() noexcept override;
    const char *what() const noexcept override;
};

class ExitException: public Exception
{
public:
    ExitException(int code = 0);
    ~ExitException() noexcept override;
    inline int code() const noexcept { return m_code; }
    const char *what() const noexcept override;

private:
    int m_code {0};
};

class RuntimeError: public Exception
{
public:
    explicit RuntimeError(const QString &message);
    explicit RuntimeError(QString &&message) noexcept;
    ~RuntimeError() noexcept override;

    const QString &message() const noexcept;
    const char *what() const noexcept override;

private:
    QString m_message;
};

} // namespace TextureTool
