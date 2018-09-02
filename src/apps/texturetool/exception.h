#pragma once

#include <QtCore/QString>
#include <exception>
#include <QException>

namespace TextureTool {

class Exception: public std::exception
{
public:
    Exception() noexcept = default;
    ~Exception() noexcept override;
    const char *what() const noexcept override;
};

class ExitException: public Exception
{
public:
    explicit ExitException(int code = 0) noexcept : m_code(code) {}
    ~ExitException() noexcept override;
    int code() const noexcept { return m_code; }
    const char *what() const noexcept override;

private:
    int m_code {0};
};

class RuntimeError: public Exception
{
public:
    explicit RuntimeError(const QString &message) noexcept : m_message(message) {}
    explicit RuntimeError(QString &&message) noexcept : m_message(std::move(message)) {}
    ~RuntimeError() noexcept override;

    const QString &message() const noexcept { return m_message; }
    const char *what() const noexcept override;

private:
    QString m_message;
};

} // namespace TextureTool
