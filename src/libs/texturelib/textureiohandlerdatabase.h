#pragma once

#include "texturelib_global.h"

#include <TextureLib/TextureIOHandler>
#include <TextureLib/TextureIOHandlerPlugin>

#include <QtCore/QHash>

#include <memory>

class QPluginLoader;

class TEXTURELIB_EXPORT TextureIOHandlerDatabase
{
    Q_DISABLE_COPY(TextureIOHandlerDatabase)
public:
    using QIODevicePointer = TextureIOHandler::QIODevicePointer;

    TextureIOHandlerDatabase();
    TextureIOHandlerDatabase(TextureIOHandlerDatabase &&) = delete;
    ~TextureIOHandlerDatabase();
    TextureIOHandlerDatabase &operator =(TextureIOHandlerDatabase &&) = delete;

    using Capabilities = TextureIOHandlerPlugin::Capabilities;

    std::unique_ptr<TextureIOHandler> create(QIODevicePointer device, QStringView mimeType, Capabilities caps);
    std::vector<QStringView> availableMimeTypes(Capabilities caps) const;
    TextureIOHandlerPlugin *plugin(const QString &mimeType) const;
    void registerPlugin(const QString &mimeType, TextureIOHandlerPlugin *plugin);

    static TextureIOHandlerDatabase *instance();

private:
    QHash<QString, TextureIOHandlerPlugin *> map;
    std::vector<std::unique_ptr<QPluginLoader>> loaders;
};
