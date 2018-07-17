#include "pkmhandler.h"

class PkmHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(PkmHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.PkmHandlerPlugin" FILE "pkm.json")

public:
    PkmHandlerPlugin() = default;

    std::unique_ptr<TextureIOHandler> create(const QMimeType &mimeType) override
    {
        if (mimeType.name() == u"image/x-pkm")
            return std::make_unique<PkmHandler>();
        return nullptr;
    }

    Capabilities capabilities(const QMimeType &mimeType) const override
    {
        if (mimeType.name() == u"image/x-pkm")
            return CanRead;
        return Capabilities();
    }
};

#include "main.moc"
