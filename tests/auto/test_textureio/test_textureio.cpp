#include <QtTest>

#include "testhandler.h"
#include "testhandler_p.h"
#include <TextureLib/TextureIO>
#include <TextureLib/private/TextureIOHandlerDatabase>

#include <QtCore/QMimeType>

class TestTextureIO : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void defaultValues();
    void setters();
    void read_data();
    void read();
    void write_data();
    void write();
};

void TestTextureIO::initTestCase()
{
}

void TestTextureIO::defaultValues()
{
    TextureIO io;

    TextureIO io2(QStringLiteral("image.png"));
    QCOMPARE(io2.fileName(), QStringLiteral("image.png"));
}

void TestTextureIO::setters()
{
    QFile file("file.txt");
    file.remove();
    file.open(QIODevice::WriteOnly);
    file.write("Hello, world!");
    file.close();

    TextureIO io;

    io.setDevice(&file);
    QCOMPARE(io.device(), &file);
    io.setDevice(nullptr);
    QVERIFY(io.device() == nullptr);

    io.setFileName("file.txt");
    QCOMPARE(io.fileName(), QString("file.txt"));
    QVERIFY(qobject_cast<QFile *>(io.device()) != nullptr);
    QCOMPARE(io.mimeType().name(), QString("text/plain"));

    io.setMimeType(QMimeDatabase().mimeTypeForName("text/html"));
    QCOMPARE(io.mimeType().name(), QString("text/html"));
}

void TestTextureIO::read_data()
{
    QTest::addColumn<Texture::Type>("type");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("layers");
    QTest::addColumn<int>("levels");

    QTest::newRow("64x64, ARGB32, 1, 1") << Texture::Type::Texture2D << 64 << 64 << 1 << Texture::Format::ARGB32 << 1 << 1;
}

static QByteArray generateData(int width, int height, Texture::Format format)
{
    Texture texture = Texture::create2DTexture(width, height, format);

    if (texture.isNull())
        return QByteArray();

    return QByteArray((char*)texture.data(), int(texture.bytes()));
}

void TestTextureIO::read()
{
    QFETCH(Texture::Type, type);
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, depth);
    QFETCH(int, layers);
    QFETCH(int, levels);

    TestImageData data;
    data.type = type;
    data.format = format;
    data.width = width;
    data.height = height;
    data.depth = depth;
    data.layers = layers;
    data.levels = levels;
    data.data = generateData(width, height, format);

    QBuffer buffer;
    QVERIFY(buffer.open(QIODevice::WriteOnly));
    QDataStream stream(&buffer);
    stream << data;
    buffer.close(); // close device to ensure TextureIO will open it with correct mode

    TextureIO io;
    io.setDevice(&buffer);
    io.setMimeType("application/octet-stream");
    const auto result = io.read();
    const auto ok = result.first;
    QVERIFY2(ok, qPrintable(ok.toString()));

    const auto texture = result.second;
    QCOMPARE(texture.type(), type);
    QCOMPARE(texture.format(), format);
    QCOMPARE(texture.width(), width);
    QCOMPARE(texture.height(), height);
    QCOMPARE(texture.depth(), depth);
    QCOMPARE(texture.layers(), layers);
    QCOMPARE(texture.levels(), levels);

    QCOMPARE(QByteArray((char*)texture.data(), int(texture.bytes())), data.data);
}

void TestTextureIO::write_data()
{
    QTest::addColumn<Texture::Type>("type");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("layers");
    QTest::addColumn<int>("levels");

    QTest::newRow("64x64, ARGB32, 1, 1") << Texture::Type::Texture2D << 64 << 64 << 1 << Texture::Format::ARGB32 << 1 << 1;
}

void TestTextureIO::write()
{
    QFETCH(Texture::Type, type);
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, depth);
    QFETCH(int, layers);
    QFETCH(int, levels);

    QBuffer buffer;

    TextureIO io;
    io.setDevice(&buffer);
    io.setMimeType("application/octet-stream");

    Texture texture;

    switch (type) {
    case Texture::Type::Texture2D:
        texture = Texture::create2DTexture(width, height, format);
    default:
        break;
    }

    const auto ok = io.write(texture);
    QVERIFY2(ok, qPrintable(ok.toString()));

    TestImageData data;
    buffer.close();

    QVERIFY(buffer.open(QIODevice::ReadOnly));
    QDataStream stream(&buffer);
    stream >> data;

    QCOMPARE(data.type, type);
    QCOMPARE(data.format, format);
    QCOMPARE(data.width, width);
    QCOMPARE(data.height, height);
    QCOMPARE(data.depth, depth);
    QCOMPARE(data.layers, layers);
    QCOMPARE(data.levels, levels);
    QCOMPARE(data.data, QByteArray((char*)texture.data(), int(texture.bytes())));
}

QTEST_APPLESS_MAIN(TestTextureIO)

#include "test_textureio.moc"
