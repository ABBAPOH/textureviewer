#pragma once

#include "texturelib_global.h"

#include <QtCore/QMimeType>
#include <QtCore/QObject>

#include <memory>

class TextureIOHandler;

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
