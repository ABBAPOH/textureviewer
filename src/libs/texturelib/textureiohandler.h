#pragma once

#include "texturelib_global.h"

#include <TextureLib/Texture>
#include <QtCore/QMimeType>

class TEXTURELIB_EXPORT TextureIOHandler
{
public:
    TextureIOHandler();
    virtual ~TextureIOHandler();

    QIODevice *device() const;
    void setDevice(QIODevice *device);

    QMimeType mimeType() const;
    void setMimeType(const QMimeType &mimeType);

    virtual bool canRead() const = 0;
    virtual bool read(Texture &texture) = 0;
    virtual bool write(const Texture &texture);

private:
    QIODevice *_device {nullptr};
    QMimeType _mimeType;
    QByteArray _subType;

    friend class ImageDocument;
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

    virtual QByteArray name() const = 0;
    virtual Capabilities capabilities(QIODevice *device, const QMimeType &mimeType) const = 0;

    virtual std::unique_ptr<TextureIOHandler> create(QIODevice *device, const QMimeType &mimeType) = 0;
};
