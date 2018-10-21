#include "textureiohandlerdatabase.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
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
        for (const auto &mimeType : mimeTypes) {
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
            for (const auto &mimeTypeName : mimeTypes) {
                registerPlugin(mimeTypeName.toString(), handlerPlugin);
            }
            loaders.push_back(std::move(loader));
        }
    }
}

TextureIOHandlerDatabase::~TextureIOHandlerDatabase()
{
    // TODO (abbapoh): should we unload plugins?..
    // What is the current Qt policy with those awful QStringLiteral crashes?..
    for (const auto &loader: loaders)
        loader->unload();
}

std::unique_ptr<TextureIOHandler> TextureIOHandlerDatabase::create(QIODevicePointer device, QStringView mimeType, Capabilities caps)
{
    std::unique_ptr<TextureIOHandler> result;
    if (!device || mimeType.isEmpty()) {
        qWarning() << "TextureIOHandlerDatabase::create: called with incorrect parameters";
        return result;
    }

    auto plugin = map.value(mimeType.toString());
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

std::vector<QStringView> TextureIOHandlerDatabase::availableMimeTypes(Capabilities caps) const
{
    std::vector<QStringView> result;
    for (auto it = map.begin(), end = map.end(); it != end; it++) {
        const auto mt = QStringView(it.key());
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

    const auto caps = plugin->formatCapabilites(mimeType);
    m_readableFormats.reserve(m_readableFormats.size() + size_t(caps.size()));
    m_writableFormats.reserve(m_writableFormats.size() + size_t(caps.size()));
    for (const auto &cap: caps) {
        if (cap.capabilities & TextureIOHandlerPlugin::Capability::CanRead)
            m_readableFormats.push_back(cap.format);
        if (cap.capabilities & TextureIOHandlerPlugin::Capability::CanWrite)
            m_writableFormats.push_back(cap.format);
    }
    std::sort(m_readableFormats.begin(), m_readableFormats.end());
    std::sort(m_writableFormats.begin(), m_writableFormats.end());

    map.insert(mimeType, plugin);
}

TextureIOHandlerDatabase *TextureIOHandlerDatabase::instance()
{
    static TextureIOHandlerDatabase staticInstance;
    return &staticInstance;
}
