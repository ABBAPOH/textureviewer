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

    TextureIOHandler();
    TextureIOHandler(TextureIOHandler &&) = default;
    virtual ~TextureIOHandler();

    TextureIOHandler &operator=(TextureIOHandler &&) = default;

    inline QIODevicePointer device() const { return m_device; }
    inline void setDevice(QIODevicePointer device) { m_device = device; }

    virtual bool read(Texture &texture) = 0;
    virtual bool write(const Texture &texture);

private:
    QIODevicePointer m_device;
};
