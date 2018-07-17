#include "ddshandler.h"

#include <TextureLib/TextureIOHandlerPlugin>

class DdsHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(DdsHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.DdsHandlerPlugin" FILE "dds.json")

public:
    DdsHandlerPlugin() = default;

    std::unique_ptr<TextureIOHandler> create(const QMimeType &mimeType) override
    {
        if (mimeType.name() == u"image/x-dds")
            return std::make_unique<DDSHandler>();
        return nullptr;
    }

    Capabilities capabilities(const QMimeType &mimeType) const override
    {
        if (mimeType.name() == u"image/x-dds")
            return Capabilities(DdsHandlerPlugin::CanRead | DdsHandlerPlugin::CanWrite);
        return Capabilities();
    }
};

#include "main.moc"
