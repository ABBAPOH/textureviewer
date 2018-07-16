#include "textureiohandlerdatabase.h"

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QMimeDatabase>
#include <QtCore/QPluginLoader>

TextureIOHandlerDatabase::TextureIOHandlerDatabase()
{
    for (auto staticPlugin : QPluginLoader::staticPlugins()) {
        auto plugin = staticPlugin.instance();
        auto handlerPlugin = qobject_cast<TextureIOHandlerPlugin *>(plugin);
        if (!handlerPlugin)
            continue;

        const auto metaData = staticPlugin.metaData().value("MetaData").toObject();
        const auto mimeTypes = metaData.value("MimeTypes").toArray();
        for (auto mimeType : mimeTypes) {
            registerPlugin(mimeType.toString(), handlerPlugin);
        }
    }

    for (const auto &folder : qApp->libraryPaths()) {
        QDir dir(folder);
        if (!dir.cd("textureformats"))
            continue;
        for (const auto &fileName : dir.entryList(QDir::Files)) {
            auto loader = std::make_unique<QPluginLoader>(dir.absoluteFilePath(fileName));
            const auto metaData = loader->metaData().value("MetaData").toObject();
            const auto mimeTypes = metaData.value("MimeTypes").toArray();
            if (mimeTypes.isEmpty()) {
                qWarning() << "File" << dir.absoluteFilePath(fileName)
                           << "does not contain 'MimeTypes' key";
                continue;
            }
            const auto plugin = loader->instance();
            if (!plugin) {
                qWarning() << "File" << dir.absoluteFilePath(fileName) << "is not a Qt plugin";
                continue;
            }

            auto handlerPlugin = qobject_cast<TextureIOHandlerPlugin *>(plugin);
            if (!handlerPlugin) {
                qWarning() << "File" << dir.absoluteFilePath(fileName)
                           << "does not contain an textureformat plugin";
                continue;
            }
            for (auto mimeTypeName : mimeTypes) {
                registerPlugin(mimeTypeName.toString(), handlerPlugin);
            }
            loaders.push_back(std::move(loader));
        }
    }
}

TextureIOHandlerDatabase::~TextureIOHandlerDatabase()
{
    // TODO: should we unload plugins?..
    // What is the current Qt policy with those awful QStringLiteral crashes?..
    for (const auto &loader: loaders)
        loader->unload();
}

std::unique_ptr<TextureIOHandler> TextureIOHandlerDatabase::create(QIODevicePointer device, const QMimeType &mimeType, Capabilities caps)
{
    std::unique_ptr<TextureIOHandler> result;
    if (!device || !mimeType.isValid()) {
        qWarning() << "TextureIOHandlerDatabase::create: called with incorrect parameters";
        return result;
    }

    auto plugin = map.value(mimeType.name());
    if (!plugin)
        return result;

    if (!(plugin->capabilities(mimeType) & caps))
        return result;

    result = plugin->create(mimeType);

    if (!result)
        return result;

    result->setDevice(device);

    return result;
}

std::vector<QMimeType> TextureIOHandlerDatabase::availableMimeTypes(Capabilities caps) const
{
    std::vector<QMimeType> result;
    for (auto it = map.begin(), end = map.end(); it != end; it++) {
        auto mt = QMimeDatabase().mimeTypeForName(it.key());
        if (it.value()->capabilities(mt) & caps)
            result.push_back(mt);
    }
    return result;
}

TextureIOHandlerPlugin *TextureIOHandlerDatabase::plugin(const QString &mimeType) const
{
    return map.value(mimeType);
}

void TextureIOHandlerDatabase::registerPlugin(const QString &mimeType, TextureIOHandlerPlugin *plugin)
{
    if (!plugin) {
        qWarning() << "TextureIOHandlerDatabase::registerPlugin: no plugin passed";
        return;
    }

    const QMimeType type = QMimeDatabase().mimeTypeForName(mimeType);
    if (!type.isValid()) {
        qWarning() << "TextureIOHandlerDatabase::registerPlugin: Invalid mimetype" << mimeType;
        return;
    }

    map.insert(mimeType, plugin);
}

Q_GLOBAL_STATIC(TextureIOHandlerDatabase, static_instance)
TextureIOHandlerDatabase *TextureIOHandlerDatabase::instance()
{
    return static_instance();
}
