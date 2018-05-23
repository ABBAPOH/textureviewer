#include <QtTest>
#include <TextureLib/Texture>

class TestTexture : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstructed();

    void construct_data();
    void construct();
    void bytesPerLine_data();
    void bytesPerLine();
    void offset_data();
    void offset();
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

    QCOMPARE(tex.bitsPerTexel(), 0);
    QCOMPARE(tex.bytesPerLine(), 0);
    QCOMPARE(tex.bytesPerImage(), 0);
    QCOMPARE(tex.bytes(), 0);

    QCOMPARE(tex.data(), nullptr);
    QCOMPARE(tex.constData(), nullptr);
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
    // CubeMap
    QTest::newRow("ARGB32, size=1, levels=1, layers=1")
            << Texture::Type::TextureCubeMap << Texture::Format::ARGB32 << 1 << 1<< 1 << 1 << 1 << qsizetype(24);
    QTest::newRow("ARGB32, size=64, levels=1, layers=1")
            << Texture::Type::TextureCubeMap << Texture::Format::ARGB32 << 64 << 64 << 1 << 1 << 1 << qsizetype(98304);
    // CubeMap Array
    QTest::newRow("ARGB32, size=64, levels=1, layers=1")
            << Texture::Type::TextureCubeMapArray << Texture::Format::ARGB32 << 64 << 64 << 1 << 1 << 1 << qsizetype(98304);
    QTest::newRow("ARGB32, size=64, levels=1, layers=8")
            << Texture::Type::TextureCubeMapArray << Texture::Format::ARGB32 << 64 << 64 << 1 <<  1 << 8 << qsizetype(786432);
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

    const auto texture = Texture::create(type, format, width, height, depth, levels, layers);

    QVERIFY(!texture.isNull());
    QCOMPARE(texture.type(), type);
    QCOMPARE(texture.format(), format);
    QCOMPARE(texture.width(), width);
    QCOMPARE(texture.height(), height);
    QCOMPARE(texture.depth(), depth);
    QCOMPARE(texture.layers(), layers);
    QCOMPARE(texture.levels(), levels);
    QCOMPARE(texture.bytes(), bytes);
}

void TestTexture::bytesPerLine_data()
{
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<int>("levels");
    QTest::addColumn<int>("level");
    QTest::addColumn<qsizetype>("bpt");
    QTest::addColumn<qsizetype>("bpl");
    QTest::addColumn<qsizetype>("bpi");

    QTest::newRow("ARGB32, 1x1x1, level 0/1") << Texture::Format::ARGB32 << 1 << 1 << 1 << 1 << 0 << qsizetype(32) << qsizetype(4) << qsizetype(4);
    QTest::newRow("ARGB32, 64x1x1, level 0/1") << Texture::Format::ARGB32 << 64 << 1 << 1 << 1 << 0 << qsizetype(32) << qsizetype(256) << qsizetype(256);
    QTest::newRow("ARGB32, 64x64x64, level 0/1") << Texture::Format::ARGB32 << 64 << 64 << 64 << 1 << 0 << qsizetype(32) << qsizetype(256) << qsizetype(1048576);
    QTest::newRow("ARGB32, 4x4x4, level 0/1") << Texture::Format::ARGB32 << 4 << 4 << 4 << 1 << 0 << qsizetype(32) << qsizetype(16) << qsizetype(256);
    QTest::newRow("ARGB32, 4x4x4, level 0/3") << Texture::Format::ARGB32 << 4 << 4 << 4 << 3 << 0 << qsizetype(32) << qsizetype(16) << qsizetype(256);
    QTest::newRow("ARGB32, 4x4x4, level 1/3") << Texture::Format::ARGB32 << 4 << 4 << 4 << 3 << 1 << qsizetype(32) << qsizetype(8) << qsizetype(32);
    QTest::newRow("ARGB32, 4x4x4, level 2/3") << Texture::Format::ARGB32 << 4 << 4 << 4 << 3 << 2 << qsizetype(32) << qsizetype(4) << qsizetype(4);
    QTest::newRow("ARGB32, 5x4x4, level 0/3") << Texture::Format::ARGB32 << 5 << 4 << 4 << 3 << 0 << qsizetype(32) << qsizetype(20) << qsizetype(320);
    QTest::newRow("ARGB32, 5x4x4, level 1/3") << Texture::Format::ARGB32 << 5 << 4 << 4 << 3 << 1 << qsizetype(32) << qsizetype(8) << qsizetype(32);
    QTest::newRow("ARGB32, 5x4x4, level 2/3") << Texture::Format::ARGB32 << 4 << 4 << 4 << 3 << 2 << qsizetype(32) << qsizetype(4) << qsizetype(4);
}

void TestTexture::bytesPerLine()
{
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, depth);
    QFETCH(int, levels);
    QFETCH(int, level);
    QFETCH(qsizetype, bpt);
    QFETCH(qsizetype, bpl);
    QFETCH(qsizetype, bpi);

    const auto texture = Texture::create3DTexture(format, width, height, depth, levels);

    QVERIFY(!texture.isNull());
    QCOMPARE(texture.bitsPerTexel(), bpt);
    QCOMPARE(texture.bytesPerLine(level), bpl);
    QCOMPARE(texture.bytesPerImage(level), bpi);
}

void TestTexture::offset_data()
{
    QTest::addColumn<Texture::Type>("type");
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<int>("levels");
    QTest::addColumn<int>("layers");
    QTest::addColumn<int>("level");
    QTest::addColumn<int>("layer");
    QTest::addColumn<qsizetype>("offset");

    QTest::newRow("ARGB32, 1x1x1, level 0/1, layer 0/1")
            << Texture::Type::Texture2D << Texture::Format::ARGB32
            << 1 << 1 << 1
            << 1 << 1
            << 0 << 0
            << qsizetype(0);

    QTest::newRow("ARGB32, 64x64x1, level 0/1, layer 1/10")
            << Texture::Type::Texture2DArray << Texture::Format::ARGB32
            << 64 << 64 << 1
            << 1 << 10
            << 0 << 1
            << qsizetype(1 * 64*64*4);

    QTest::newRow("ARGB32, 64x64x1, level 1/2, layer 0/1")
            << Texture::Type::Texture2D << Texture::Format::ARGB32
            << 64 << 64 << 1
            << 2 << 1
            << 1 << 0
            << qsizetype(64*64*4);

    QTest::newRow("ARGB32, 64x64x1, level 2/3, layer 0/1")
            << Texture::Type::Texture2D << Texture::Format::ARGB32
            << 64 << 64 << 1
            << 3 << 1
            << 2 << 0
            << qsizetype(64*64*4 + 32*32*4);
}

void TestTexture::offset()
{
    QFETCH(Texture::Type, type);
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, depth);
    QFETCH(int, levels);
    QFETCH(int, layers);
    QFETCH(int, level);
    QFETCH(int, layer);
    QFETCH(qsizetype, offset);

    const auto texture = Texture::create(type, format, width, height, depth, levels, layers);

    QVERIFY(!texture.isNull());
    QCOMPARE(texture.offset(level, layer), offset);
}

QTEST_MAIN(TestTexture)

#include "test_texture.moc"
