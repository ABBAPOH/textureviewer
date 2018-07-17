#pragma once

#include <TextureLib/TextureIOHandler>
#include <TextureLib/TextureIOHandlerPlugin>

class TestHandler : public TextureIOHandler
{
public:
    virtual bool read(Texture &texture) override;
    virtual bool write(const Texture &contents) override;
};

class TestHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(TestHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.TestHandlerPlugin" FILE "test.json")

public:
    TestHandlerPlugin();
    std::unique_ptr<TextureIOHandler> create(const QMimeType &mimeType) override;
    Capabilities capabilities(const QMimeType &mimeType) const override;
};
