#include "pkmhandler.h"

#include <TextureLib/TextureIOHandlerPlugin>

class PkmHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(PkmHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.PkmHandlerPlugin" FILE "pkm.json")

public:
    PkmHandlerPlugin() = default;

    std::unique_ptr<TextureIOHandler> create(QStringView mimeType) override
    {
        if (mimeType == u"image/x-pkm")
            return std::make_unique<PkmHandler>();
        return nullptr;
    }

    Capabilities capabilities(QStringView mimeType) const override
    {
        if (mimeType == u"image/x-pkm")
            return CanRead | CanWrite;
        return Capabilities();
    }
};

#include "main.moc"
