#pragma once

#include "texturelib_global.h"

#include <ObserverPointer>

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

class Texture;

class TEXTURELIB_EXPORT TextureIOHandler
{
    Q_DISABLE_COPY(TextureIOHandler)
public:
    using QIODevicePointer = ObserverPointer<QIODevice>;

    TextureIOHandler() noexcept = default;
    TextureIOHandler(TextureIOHandler &&) noexcept = default;
    virtual ~TextureIOHandler() noexcept;

    TextureIOHandler &operator=(TextureIOHandler &&) noexcept = default;

    QIODevicePointer device() const noexcept { return m_device; }
    void setDevice(QIODevicePointer device) noexcept { m_device = device; }

    virtual bool read(Texture &texture) = 0;
    virtual bool write(const Texture &texture);

private:
    QIODevicePointer m_device;
};
