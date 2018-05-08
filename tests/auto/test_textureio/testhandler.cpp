#include "testhandler.h"
#include "testhandler_p.h"

#include <QtPlugin>
#include <QDebug>

QDataStream &operator <<(QDataStream &stream, const TestImageData &data)
{
    stream << data.magic;
    stream << quint32(data.type);
    stream << quint32(data.format);
    stream << data.width;
    stream << data.height;
    stream << data.depth;
    stream << data.layers;
    stream << data.levels;
    stream << data.data;

    return stream;
}

QDataStream &operator >>(QDataStream &stream, TestImageData &data)
{
    quint32 magic = 0;
    quint32 value;
    stream >> magic;
    if (magic != 0)
        return stream;

    data = TestImageData();

    data.magic = magic;
    stream >> value;
    data.type = Texture::Type(value);
    stream >> value;
    data.format = Texture::Format(value);
    stream >> data.width;
    stream >> data.height;
    stream >> data.depth;
    stream >> data.layers;
    stream >> data.levels;
    stream >> data.data;

    return stream;
}

bool TestHandler::canRead() const
{
    return true;
}

bool TestHandler::read(Texture &texture)
{
    QDataStream s(device());
    s >> _data;
    if (s.status() != QDataStream::Ok)
        return false;

    Texture result;

    switch (_data.type) {
    case Texture::Type::Texture2D:
        result = Texture::create2DTexture(_data.width, _data.height, _data.format);
        break;
    default:
        qWarning() << "Unsupported texture type" << int(_data.type);
    }

    if (result.isNull()) {
        qWarning() << "Invalid parameters to create*Texture";
        return false;
    }

    if (result.bytes() != _data.data.size()) {
        qWarning() << "Stored size differs from texture size";
        return false;
    }

    memmove(result.data(), _data.data.data(), size_t(_data.data.size()));
    texture = result;

    return true;
}

bool TestHandler::write(const Texture &texture)
{
    _data = TestImageData();

    _data.type = texture.type();
    _data.format = texture.format();
    _data.width = texture.width();
    _data.height = texture.height();
    _data.depth = texture.depth();
    _data.layers = texture.layers();
    _data.levels = texture.levels();
    _data.data = QByteArray((char*)texture.data(), int(texture.bytes()));

    QDataStream stream(device());
    stream << _data;

    if (stream.status() != QDataStream::Ok)
        return false;

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
