#include "testhandler.h"

#include <QtPlugin>
#include <QDebug>

bool TestHandler::canRead() const
{
    return true;
}

bool TestHandler::read(Texture &texture)
{
    QDataStream s(device());
    s >> texture;
    if (s.status() != QDataStream::Ok) {
        qWarning() << "Can't read texture";
        return false;
    }

    return true;
}

bool TestHandler::write(const Texture &texture)
{
    QDataStream stream(device());
    stream << texture;

    if (stream.status() != QDataStream::Ok) {
        qWarning() << "Can't write texture";
        return false;
    }

    return true;
}

TestHandlerPlugin::TestHandlerPlugin()
{
}

QByteArray TestHandlerPlugin::name() const
{
    return "test";
}

std::unique_ptr<TextureIOHandler> TestHandlerPlugin::create(QIODevice *device, const QMimeType &mimeType)
{
    Q_UNUSED(device);
    Q_UNUSED(mimeType);
    return std::make_unique<TestHandler>();
}

TestHandlerPlugin::Capabilities TestHandlerPlugin::capabilities(QIODevice *device,
                                                                const QMimeType &mimeType) const
{
    Q_UNUSED(device);
    Q_UNUSED(mimeType);
    return Capabilities(Capability::CanRead | Capability::CanWrite);
}

Q_IMPORT_PLUGIN(TestHandlerPlugin)
