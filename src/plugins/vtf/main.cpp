#include "vtfhandler.h"

class VTFHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(VTFHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.VTFHandlerPlugin" FILE "vtf.json")
public:
    VTFHandlerPlugin() = default;

    std::unique_ptr<TextureIOHandler> create(const QMimeType &mimeType) override
    {
        if (mimeType.name() == u"image/x-vtf")
            return std::make_unique<VTFHandler>();
        return nullptr;
    }

    Capabilities capabilities(const QMimeType &mimeType) const override
    {
        if (mimeType.name() == u"image/x-vtf")
            return Capabilities(VTFHandlerPlugin::CanRead);
        return Capabilities();
    }
};

#include "main.moc"
