#include <QtTest>

#include "testhandler.h"
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

    io.setDevice(TextureIO::QIODevicePointer(&file));
    QCOMPARE(io.device().get(), &file);
    io.setDevice(nullptr);
    QVERIFY(io.device() == nullptr);

    io.setFileName("file.txt");
    QCOMPARE(io.fileName(), QString("file.txt"));
    QVERIFY(qobject_cast<QFile *>(io.device().get()) != nullptr);
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

    QTest::newRow("64x64, ARGB32, 1, 1") << Texture::Type::Texture2D << 64 << 64 << 1 << Texture::Format::RGBA_8888 << 1 << 1;
}

static QByteArray generateData(int width, int height, Texture::Format format)
{
    Texture texture = Texture::create(Texture::Type::Texture2D, format, width, height, 1);

    if (texture.isNull())
        return QByteArray();

    return QByteArray((char*)texture.data().data(), int(texture.bytes()));
}

static Texture createTexture(
        Texture::Type type,
        Texture::Format format,
        int width,
        int height,
        int depth,
        int layers,
        int levels)
{
    Texture result;

    switch (type) {
    case Texture::Type::Texture2D:
        result =  Texture::create(Texture::Type::Texture2D, format, width, height, 1);
        break;
    default:
        return result;
    }
    return result;
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

    auto expectedTexture = createTexture(type, format, width, height, depth, layers, levels);

    QBuffer buffer;
    QVERIFY(buffer.open(QIODevice::WriteOnly));
    QDataStream stream(&buffer);
    stream << expectedTexture;
    buffer.close(); // close device to ensure TextureIO will open it with correct mode

    TextureIO io;
    io.setDevice(TextureIO::QIODevicePointer(&buffer));
    io.setMimeType("application/octet-stream");
    const auto result = io.read();
    QVERIFY2(result, qPrintable(toUserString(result.error())));
    QCOMPARE(*result, expectedTexture);
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

    QTest::newRow("64x64, ARGB32, 1, 1") << Texture::Type::Texture2D << 64 << 64 << 1 << Texture::Format::RGBA_8888 << 1 << 1;
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
    io.setDevice(TextureIO::QIODevicePointer(&buffer));
    io.setMimeType("application/octet-stream");

    Texture expectedTexture;

    switch (type) {
    case Texture::Type::Texture2D:
        expectedTexture =  Texture::create(Texture::Type::Texture2D, format, width, height, 1);
    default:
        break;
    }

    const auto ok = io.write(expectedTexture);
    QVERIFY2(ok, qPrintable(toUserString(ok)));

    buffer.close();

    QVERIFY(buffer.open(QIODevice::ReadOnly));

    Texture texture;
    QDataStream stream(&buffer);
    stream >> texture;

    QCOMPARE(texture, expectedTexture);
}

QTEST_APPLESS_MAIN(TestTextureIO)

#include "test_textureio.moc"
