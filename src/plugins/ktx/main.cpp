#include "ktxhandler.h"

#include <TextureLib/TextureIOHandlerPlugin>

class KtxHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(KtxHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.DdsHandlerPlugin" FILE "ktx.json")

public:
    KtxHandlerPlugin() = default;

    std::unique_ptr<TextureIOHandler> create(const QMimeType &mimeType) override
    {
        if (mimeType.name() == u"image/x-ktx")
            return std::make_unique<KtxHandler>();
        return nullptr;
    }

    Capabilities capabilities(const QMimeType &mimeType) const override
    {
        if (mimeType.name() == u"image/x-ktx")
            return Capabilities(KtxHandlerPlugin::CanRead);
        return Capabilities();
    }
};

#include "main.moc"
