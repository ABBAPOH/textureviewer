#pragma once

#include "texturelib_global.h"

#include <QtCore/QCoreApplication>

// TODO: move to TextureIO
enum class TextureIOStatus
{
    Ok,
    InvalidMimeType,
    FileNotFound,
    DeviceError,
    UnsupportedMimeType,
    HandlerError,
};

class TEXTURELIB_EXPORT TextureIOResult
{
    Q_DECLARE_TR_FUNCTIONS(TextureIOResult)
public:
    inline TextureIOResult(TextureIOStatus status = TextureIOStatus::Ok) noexcept
        : _status(status) {}
    inline TextureIOResult(const TextureIOResult &lhs) noexcept
        : _status(lhs._status) {}
    ~TextureIOResult() noexcept = default;

    TextureIOResult &operator=(const TextureIOResult &other) noexcept;

    inline TextureIOStatus status() const noexcept { return _status; }
    QString toString() const;
    inline bool toBool() const noexcept { return _status == TextureIOStatus::Ok; }

    inline operator bool() const noexcept { return toBool(); }
    inline bool operator !() const noexcept { return !toBool(); }

private:
    TextureIOStatus _status;
};

inline bool operator==(const TextureIOResult &lhs, const TextureIOResult &rhs) noexcept
{
    return lhs.status() == rhs.status();
}

inline bool operator!=(const TextureIOResult &lhs, const TextureIOResult &rhs) noexcept
{
    return lhs.status() != rhs.status();
}
