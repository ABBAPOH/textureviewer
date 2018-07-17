#ifndef PKMHANDLER_H
#define PKMHANDLER_H

#include <QtCore/QLoggingCategory>
#include <TextureLib/TextureIOHandler>

class PkmHandler : public TextureIOHandler
{
    Q_DISABLE_COPY(PkmHandler)
public:
    PkmHandler() = default;

public: // ImageIOHandler interface
    bool canRead() const override { return true; }
    bool read(Texture &texture) override;
};

class PkmHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(PkmHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.PkmHandlerPlugin" FILE "pkm.json")
public:
    PkmHandlerPlugin() = default;

    std::unique_ptr<TextureIOHandler> create(const QMimeType &mimeType) override;
    Capabilities capabilities(const QMimeType &mimeType) const override;
};

Q_DECLARE_LOGGING_CATEGORY(pkmhandler)

#endif // PKMHANDLER_H
