#pragma once

#include "texturelib_global.h"

#include <TextureLib/Texture>
#include <QtCore/QMimeType>

#include <memory>
#include <ObserverPointer>

class TEXTURELIB_EXPORT TextureIOHandler
{
    Q_DISABLE_COPY(TextureIOHandler)
public:
    using QIODevicePointer = ObserverPointer<QIODevice>;

    TextureIOHandler();
    virtual ~TextureIOHandler();

    QIODevicePointer device() const;
    void setDevice(QIODevicePointer device);

    virtual bool canRead() const = 0;
    virtual bool read(Texture &texture) = 0;
    virtual bool write(const Texture &texture);

private:
    QIODevicePointer m_device;
};

class TEXTURELIB_EXPORT TextureIOHandlerPlugin : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureIOHandlerPlugin)
public:
    enum Capability {
        CanRead = 0x1,
        CanWrite = 0x2,
        ReadWrite = CanRead | CanWrite
    };
    using Capabilities = QFlags<Capability>;
    Q_FLAG(Capabilities)

    explicit TextureIOHandlerPlugin() {}

    virtual Capabilities capabilities(const QMimeType &mimeType) const = 0;

    virtual std::unique_ptr<TextureIOHandler> create(const QMimeType &mimeType) = 0;
};
