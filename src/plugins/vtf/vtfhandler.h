#ifndef VTFHANDLER_H
#define VTFHANDLER_H

#include <QtCore/QLoggingCategory>
#include <TextureLib/TextureIOHandler>
#include "vtfheader.h"


class VTFHandler : public TextureIOHandler
{
public:
    VTFHandler() = default;

public: // ImageIOHandler interface
    bool canRead() const override;
    bool read(Texture &texture) override;

private:
    bool canRead(QIODevice *device) const;
};

class VTFHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(VTFHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.VTFHandlerPlugin" FILE "vtf.json")
public:
    VTFHandlerPlugin() = default;

    std::unique_ptr<TextureIOHandler> create(const QMimeType &mimeType) override;
    Capabilities capabilities(const QMimeType &mimeType) const override;
};


Q_DECLARE_LOGGING_CATEGORY(vtfhandler)

#endif // VTFHANDLER_H
