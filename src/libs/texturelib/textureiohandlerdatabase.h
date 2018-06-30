#pragma once

#include "texturelib_global.h"

#include <TextureLib/TextureIOHandler>
#include <memory>

class QPluginLoader;

class TEXTURELIB_EXPORT TextureIOHandlerDatabase
{
    Q_DISABLE_COPY(TextureIOHandlerDatabase)
public:
    TextureIOHandlerDatabase();
    ~TextureIOHandlerDatabase();

    using Capabilities = TextureIOHandlerPlugin::Capabilities;

    std::unique_ptr<TextureIOHandler> create(QIODevice *device, const QMimeType &mimeType, Capabilities caps);
    std::vector<QMimeType> availableMimeTypes(Capabilities caps) const;
    TextureIOHandlerPlugin *plugin(const QString &mimeType) const;
    void registerPlugin(const QString &mimeType, TextureIOHandlerPlugin *plugin);

    static TextureIOHandlerDatabase *instance();

private:
    QHash<QString, TextureIOHandlerPlugin *> map;
    std::vector<std::unique_ptr<QPluginLoader>> loaders;
};
