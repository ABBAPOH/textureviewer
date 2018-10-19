#include "testhandler.h"

#include <TextureLib/Texture>

#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QtPlugin>

bool TestHandler::read(Texture &texture)
{
    QDataStream s(device().get());
    s >> texture;
    if (s.status() != QDataStream::Ok) {
        qWarning() << "Can't read texture";
        return false;
    }

    return true;
}

bool TestHandler::write(const Texture &texture)
{
    QDataStream stream(device().get());
    stream << texture;

    if (stream.status() != QDataStream::Ok) {
        qWarning() << "Can't write texture";
        return false;
    }

    return true;
}

std::unique_ptr<TextureIOHandler> TestHandlerPlugin::create(QStringView mimeType)
{
    Q_UNUSED(mimeType);
    return std::make_unique<TestHandler>();
}

TestHandlerPlugin::Capabilities TestHandlerPlugin::capabilities(QStringView mimeType) const
{
    Q_UNUSED(mimeType);
    return {Capability::CanRead | Capability::CanWrite};
}

Q_IMPORT_PLUGIN(TestHandlerPlugin)
