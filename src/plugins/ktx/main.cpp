#include "ktxhandler.h"

#include <TextureLib/TextureIOHandlerPlugin>

class KtxHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(KtxHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.DdsHandlerPlugin" FILE "ktx.json")

public:
    KtxHandlerPlugin() = default;

    std::unique_ptr<TextureIOHandler> create(QStringView mimeType) override
    {
        if (mimeType == u"image/x-ktx")
            return std::make_unique<KtxHandler>();
        return nullptr;
    }

    Capabilities capabilities(QStringView mimeType) const override
    {
        if (mimeType == u"image/x-ktx")
            return Capability::CanRead;
        return {};
    }
};

#include "main.moc"
