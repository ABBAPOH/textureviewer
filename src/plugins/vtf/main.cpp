#include "vtfhandler.h"

#include <TextureLib/TextureIOHandlerPlugin>

class VTFHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(VTFHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.VTFHandlerPlugin" FILE "vtf.json")
public:
    VTFHandlerPlugin() = default;

    std::unique_ptr<TextureIOHandler> create(QStringView mimeType) override
    {
        if (mimeType == u"image/x-vtf")
            return std::make_unique<VTFHandler>();
        return nullptr;
    }

    Capabilities capabilities(QStringView mimeType) const override
    {
        if (mimeType == u"image/x-vtf")
            return Capability::CanRead;
        return Capabilities();
    }
};

#include "main.moc"
