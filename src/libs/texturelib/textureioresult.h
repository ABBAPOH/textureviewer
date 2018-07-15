#pragma once

#include "texturelib_global.h"

#include <QtCore/QCoreApplication>

#include <OptionalType>

// TODO: move to TextureIO
enum class TextureIOError
{
    InvalidMimeType,
    FileNotFound,
    DeviceError,
    UnsupportedMimeType,
    HandlerError,
};

QString TEXTURELIB_EXPORT toUserString(TextureIOError status);

// we can't do the following
// using TextureIOResult = std::experimental::expected<void, TextureIOError>;
// so we emulate a subset of an expected API for now

class TEXTURELIB_EXPORT TextureIOResult
{
    Q_DECLARE_TR_FUNCTIONS(TextureIOResult)
public:
    inline TextureIOResult() noexcept
        : m_value(nullOptional())
    {}
    inline TextureIOResult(TextureIOError status) noexcept
        : m_value(status) {}
    inline TextureIOResult(const TextureIOResult &lhs) noexcept
        : m_value(lhs.m_value) {}
    ~TextureIOResult() noexcept = default;

    TextureIOResult &operator=(const TextureIOResult &other) noexcept;

    inline TextureIOError error() const { return *m_value; }

    inline operator bool() const noexcept { return !m_value; }
    inline bool operator !() const noexcept { return !!m_value; }

    friend bool operator==(const TextureIOResult &lhs, const TextureIOResult &rhs) noexcept;
    friend bool operator!=(const TextureIOResult &lhs, const TextureIOResult &rhs) noexcept;

private:
    Optional<TextureIOError> m_value;
};

inline bool operator==(const TextureIOResult &lhs, const TextureIOResult &rhs) noexcept
{
    return lhs.m_value == rhs.m_value;
}

inline bool operator!=(const TextureIOResult &lhs, const TextureIOResult &rhs) noexcept
{
    return lhs.m_value != rhs.m_value;
}

QString TEXTURELIB_EXPORT toUserString(TextureIOResult result);
