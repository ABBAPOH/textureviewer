#include "pkmhandler.h"

#include <TextureLib/TextureIOHandlerPlugin>

class PkmHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(PkmHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.PkmHandlerPlugin" FILE "pkm.json")

public:
    PkmHandlerPlugin() = default;
    PkmHandlerPlugin(PkmHandlerPlugin &&) = delete;
    ~PkmHandlerPlugin() override = default;

    PkmHandlerPlugin &operator =(PkmHandlerPlugin &&) = delete;

    std::unique_ptr<TextureIOHandler> create(QStringView mimeType) override
    {
        if (mimeType == u"image/x-pkm")
            return std::make_unique<PkmHandler>();
        return nullptr;
    }

    Capabilities capabilities(QStringView mimeType) const override
    {
        if (mimeType == u"image/x-pkm")
            return Capability::CanRead | Capability::CanWrite;
        return {};
    }
};

#include "main.moc"
