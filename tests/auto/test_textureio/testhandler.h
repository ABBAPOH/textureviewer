#pragma once

#include <TextureLib/TextureIOHandler>
#include <TextureLib/TextureIOHandlerPlugin>

class TestHandler : public TextureIOHandler
{
public:
    bool read(Texture &texture) override;
    bool write(const Texture &contents) override;
};

class TestHandlerPlugin : public TextureIOHandlerPlugin
{
    Q_OBJECT
    Q_DISABLE_COPY(TestHandlerPlugin)
    Q_PLUGIN_METADATA(IID "org.arch.ImageDocument.TestHandlerPlugin" FILE "test.json")

public:
    TestHandlerPlugin() = default;
    TestHandlerPlugin(TestHandlerPlugin &&) = delete;
    ~TestHandlerPlugin() override = default;

    TestHandlerPlugin& operator=(TestHandlerPlugin &&) = delete;

    std::unique_ptr<TextureIOHandler> create(const QMimeType &mimeType) override;
    Capabilities capabilities(const QMimeType &mimeType) const override;
};
