#pragma once

#include "texturelib_global.h"

#include <QtCore/QObject>
#include <QtCore/QStringView>

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

    TextureIOHandlerPlugin() = default;
    TextureIOHandlerPlugin(TextureIOHandlerPlugin &&) = delete;
    ~TextureIOHandlerPlugin() override = default;

    TextureIOHandlerPlugin &operator =(TextureIOHandlerPlugin &&) = delete;

    virtual Capabilities capabilities(QStringView mimeType) const = 0;

    virtual std::unique_ptr<TextureIOHandler> create(QStringView mimeType) = 0;
};
