#ifndef TEXTUREIOHANDLERPLUGIN_H
#define TEXTUREIOHANDLERPLUGIN_H

#include "texturelib_global.h"

#include <TextureLib/TextureFormat>

#include <QtCore/QObject>
#include <QtCore/QStringView>

#include <gsl/span>

#include <memory>

class TextureIOHandler;

class TEXTURELIB_EXPORT TextureIOHandlerPlugin : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TextureIOHandlerPlugin)
public:
    enum class Capability {
        CanRead = 0x1u,
        CanWrite = 0x2u,
        ReadWrite = CanRead | CanWrite
    };
    using Capabilities = QFlags<Capability>;
    Q_FLAG(Capabilities)

    struct FormatCapabilites
    {
        TextureFormat format {TextureFormat::Invalid};
        Capabilities capabilities;
    };

    TextureIOHandlerPlugin() = default;
    TextureIOHandlerPlugin(TextureIOHandlerPlugin &&) = delete;
    ~TextureIOHandlerPlugin() override = default;

    TextureIOHandlerPlugin &operator =(TextureIOHandlerPlugin &&) = delete;

    virtual Capabilities capabilities(QStringView mimeType) const = 0;

    virtual std::unique_ptr<TextureIOHandler> create(QStringView mimeType) = 0;

    virtual gsl::span<const FormatCapabilites> formatCapabilites(QStringView mimeType) const
    { Q_UNUSED(mimeType); return {}; }
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TextureIOHandlerPlugin::Capabilities)

#endif // TEXTUREIOHANDLERPLUGIN_H
