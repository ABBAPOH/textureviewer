#include <QtTest>
#include <TextureLib/Texture>

class TestTexture : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstructed();

    void construct_data();
    void construct();
};

void TestTexture::defaultConstructed()
{
    Texture tex;
    QCOMPARE(tex.isNull(), true);
    QCOMPARE(tex.type(), Texture::Type::None);
    QCOMPARE(tex.format(), Texture::Format::Invalid);
    QCOMPARE(tex.width(), 0);
    QCOMPARE(tex.height(), 0);
    QCOMPARE(tex.depth(), 0);
    QCOMPARE(tex.levels(), 0);
    QCOMPARE(tex.layers(), 0);

    QCOMPARE(tex.bytesPerTexel(), 0);
    QCOMPARE(tex.bytesPerLine(), 0);
    QCOMPARE(tex.bytesPerImage(), 0);
    QCOMPARE(tex.bytes(), 0);

    QCOMPARE(tex.data(), nullptr);
    QCOMPARE(tex.constData(), nullptr);

    QCOMPARE(tex.scanLine(0), nullptr);
    QCOMPARE(tex.constScanLine(0), nullptr);
}

void TestTexture::construct_data()
{
    QTest::addColumn<Texture::Type>("type");
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<int>("levels");
    QTest::addColumn<int>("layers");
    QTest::addColumn<qsizetype>("bytes");

    // 1D texture
    QTest::newRow("ARGB32, 1x1x1, levels=1, layers=1")
            << Texture::Type::Texture1D << Texture::Format::ARGB32 << 1 << 1 << 1 << 1 << 1 << qsizetype(4);
    QTest::newRow("ARGB32, 64x1x1, levels=1, layers=1")
            << Texture::Type::Texture1D << Texture::Format::ARGB32 << 64 << 1 << 1 << 1 << 1 << qsizetype(256);
    QTest::newRow("ARGB32, 100x1x1, levels=1, layers=1")
            << Texture::Type::Texture1D << Texture::Format::ARGB32 << 100 << 1 << 1 << 1 << 1 << qsizetype(400);
    // 1D texture array
    QTest::newRow("ARGB32, 100x1x1, levels=1, layers=1")
            << Texture::Type::Texture1DArray << Texture::Format::ARGB32 << 100 << 1 << 1 << 1 << 1 << qsizetype(400);
    QTest::newRow("ARGB32, 100x1x1, levels=1, layers=8")
            << Texture::Type::Texture1DArray << Texture::Format::ARGB32 << 100 << 1 << 1 << 1 << 8 << qsizetype(3200);
    QTest::newRow("ARGB32, 100x1x1, levels=1, layers=10")
            << Texture::Type::Texture1DArray << Texture::Format::ARGB32 << 100 << 1 << 1 << 1 << 10 << qsizetype(4000);
    // 2D texture
    QTest::newRow("ARGB32, 1x1x1, levels=1, layers=1")
            << Texture::Type::Texture2D << Texture::Format::ARGB32 << 1 << 1 << 1 << 1 << 1 << qsizetype(4);
    QTest::newRow("ARGB32, 64x64x1, levels=1, layers=1")
            << Texture::Type::Texture2D << Texture::Format::ARGB32 << 64 << 64 << 1 << 1 << 1 << qsizetype(16384);
    QTest::newRow("ARGB32, 100x100x1, levels=1, layers=1")
            << Texture::Type::Texture2D << Texture::Format::ARGB32 << 100 << 100 << 1 << 1 << 1 << qsizetype(40000);
    // 2D array
    QTest::newRow("ARGB32, 100x100x1, levels=1, layers=1")
            << Texture::Type::Texture2DArray << Texture::Format::ARGB32 << 100 << 100 << 1 << 1 << 1 << qsizetype(40000);
    QTest::newRow("ARGB32, 100x100x1, levels=1, layers=8")
            << Texture::Type::Texture2DArray << Texture::Format::ARGB32 << 100 << 100 << 1 << 1 << 8 << qsizetype(320000);
    QTest::newRow("ARGB32, 100x100x1, levels=1, layers=10")
            << Texture::Type::Texture2DArray << Texture::Format::ARGB32 << 100 << 100 << 1 << 1 << 10 << qsizetype(400000);
    // 3D texture
    QTest::newRow("ARGB32, 64x64x64, levels=1, layers=1")
            << Texture::Type::Texture3D << Texture::Format::ARGB32 << 64 << 64 << 64 << 1 << 1 << qsizetype(1048576);
    QTest::newRow("ARGB32, 100x100x100, levels=1, layers=1")
            << Texture::Type::Texture3D << Texture::Format::ARGB32 << 100 << 100 << 100 << 1 << 1 << qsizetype(4000000);
}

void TestTexture::construct()
{
    QFETCH(Texture::Type, type);
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, depth);
    QFETCH(int, layers);
    QFETCH(int, levels);
    QFETCH(qsizetype, bytes);

    Texture texture;

    switch (type) {
    case Texture::Type::Texture1D:
        texture = Texture::create1DTexture(format, width);
        break;
    case Texture::Type::Texture1DArray:
        texture = Texture::create1DTextureArray(format, width, layers);
        break;
    case Texture::Type::Texture2D:
        texture = Texture::create2DTexture(format, width, height);
        break;
    case Texture::Type::Texture2DArray:
        texture = Texture::create2DTextureArray(format, width, height, layers);
        break;
    case Texture::Type::Texture3D:
        texture = Texture::create3DTexture(format, width, height, depth);
        break;
    default:
        QVERIFY(false);
        break;
    }

    QCOMPARE(texture.isNull(), false);
    QCOMPARE(texture.format(), format);
    QCOMPARE(texture.width(), width);
    QCOMPARE(texture.height(), height);
    QCOMPARE(texture.depth(), depth);
    QCOMPARE(texture.layers(), layers);
    QCOMPARE(texture.levels(), levels);
    QCOMPARE(texture.bytes(), bytes);
}

QTEST_MAIN(TestTexture)

#include "test_texture.moc"
