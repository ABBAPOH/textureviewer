#pragma once

#include <TextureLib/TextureIOHandler>

#include "testhandler_p.h"

class TestHandler : public TextureIOHandler
{
public:
    virtual bool canRead() const override;
    virtual bool read(Texture &texture) override;
    virtual bool write(const Texture &contents) override;

private:
    TestImageData _data;
};

class TestHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(TestHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.TestHandlerPlugin" FILE "test.json")

public:
    TestHandlerPlugin();
    QByteArray name() const override;
    std::unique_ptr<TextureIOHandler> create(QIODevice *device, const QMimeType &mimeType) override;
    Capabilities capabilities(QIODevice *device, const QMimeType &mimeType) const override;
};
