#pragma once

#include "texturelib_global.h"

#include <TextureLib/Texture>
#include <QtCore/QMimeType>

#include <ObserverPointer>

class TEXTURELIB_EXPORT TextureIOHandler
{
    Q_DISABLE_COPY(TextureIOHandler)
public:
    using QIODevicePointer = ObserverPointer<QIODevice>;

    TextureIOHandler();
    virtual ~TextureIOHandler();

    inline QIODevicePointer device() const { return m_device; }
    inline void setDevice(QIODevicePointer device) { m_device = device; }

    virtual bool read(Texture &texture) = 0;
    virtual bool write(const Texture &texture);

private:
    QIODevicePointer m_device;
};
