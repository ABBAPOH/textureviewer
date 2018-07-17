#include "pkmhandler.h"

class PkmHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(PkmHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.PkmHandlerPlugin" FILE "pkm.json")

public:
    PkmHandlerPlugin() = default;

public: // TextureIOHandlerPlugin interface
    std::unique_ptr<TextureIOHandler> create(const QMimeType &mimeType) override
    {
        Q_UNUSED(mimeType);
        return std::make_unique<PkmHandler>();
    }

    Capabilities capabilities(const QMimeType &mimeType) const override
    {
        Q_UNUSED(mimeType);
        return CanRead;
    }
};

#include "main.moc"
