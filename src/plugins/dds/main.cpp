#include "ddshandler.h"

#include <TextureLib/TextureIOHandlerPlugin>

namespace {

} // namespace

class DdsHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(DdsHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.DdsHandlerPlugin" FILE "dds.json")

public:
    DdsHandlerPlugin() = default;
    DdsHandlerPlugin(DdsHandlerPlugin &&) = delete;
    ~DdsHandlerPlugin() override = default;

    DdsHandlerPlugin &operator =(DdsHandlerPlugin &&) = delete;

    std::unique_ptr<TextureIOHandler> create(QStringView mimeType) override
    {
        if (mimeType == u"image/x-dds")
            return std::make_unique<DDSHandler>();
        return nullptr;
    }

    Capabilities capabilities(QStringView mimeType) const override
    {
        if (mimeType == u"image/x-dds")
            return Capability::CanRead | Capability::CanWrite;
        return {};
    }

    // TextureIOHandlerPlugin interface
public:
    gsl::span<const FormatCapabilites> formatCapabilites(QStringView mimeType) const override
    {
        if (mimeType == u"image/x-dds")
            return DDSHandler::formatCapabilites();
        return {};
    }
};

#include "main.moc"
