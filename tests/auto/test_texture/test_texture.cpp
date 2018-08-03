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

    QCOMPARE(tex.imageData({}).data(), nullptr);
    QCOMPARE(tex.constData().data(), nullptr);
}

void TestTexture::construct_data()
{
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<bool>("cubemap");
    QTest::addColumn<int>("levels");
    QTest::addColumn<int>("layers");
    QTest::addColumn<qsizetype>("bytes");

    // 1D texture
    QTest::newRow("RGBA_8888, 1x1x1, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm // type / format
            << 1 << 1 << 1 // width / height / depth
            << false // cubemap
            << 1 << 1 // levels / layers
            << qsizetype(4); // result
    QTest::newRow("RGBA_8888, 64x1x1, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 64 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(256);
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(400);
    // 1D texture array
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(400);
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=8")
            << Texture::Format::RGBA8Unorm
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 8
            << qsizetype(3200);
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=10")
            << Texture::Format::RGBA8Unorm
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 10
            << qsizetype(4000);
    // 2D texture
    QTest::newRow("RGBA_8888, 1x1x1, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 1 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(4);
    QTest::newRow("RGBA_8888, 64x64x1, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 64 << 64 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(16384);
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(40000);
    // 2D array
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(40000);
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=8")
            << Texture::Format::RGBA8Unorm
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 8
            << qsizetype(320000);
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=10")
            << Texture::Format::RGBA8Unorm
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 10
            << qsizetype(400000);
    // 3D texture
    QTest::newRow("RGBA_8888, 64x64x64, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 64 << 64 << 64
            << false // cubemap
            << 1 << 1
            << qsizetype(1048576);
    QTest::newRow("RGBA_8888, 100x100x100, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 100 << 100 << 100
            << false // cubemap
            << 1 << 1
            << qsizetype(4000000);
    // CubeMap
    QTest::newRow("RGBA_8888, size=1, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 1 << 1 << 1
            << true // cubemap
            << 1 << 1
            << qsizetype(24);
    QTest::newRow("RGBA_8888, size=64, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 64 << 64 << 1
            << true // cubemap
            << 1 << 1
            << qsizetype(98304);
    // CubeMap Array
    QTest::newRow("RGBA_8888, size=64, levels=1, layers=1")
            << Texture::Format::RGBA8Unorm
            << 64 << 64 << 1
            << true // cubemap
            << 1 << 1
            << qsizetype(98304);
    QTest::newRow("RGBA_8888, size=64, levels=1, layers=8")
            << Texture::Format::RGBA8Unorm
            << 64 << 64 << 1
            << true // cubemap
            << 1 << 8
            << qsizetype(786432);
}

void TestTexture::construct()
{
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, depth);
    QFETCH(bool, cubemap);
    QFETCH(int, layers);
    QFETCH(int, levels);
    QFETCH(qsizetype, bytes);

    const auto texture = Texture::create(
                format, {width, height, depth}, Texture::IsCubemap(cubemap), levels, layers);

    QVERIFY(!texture.isNull());
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
    QTest::addColumn<qsizetype>("bpl1");
    QTest::addColumn<qsizetype>("bpl4");

    QTest::newRow("RGBA_8888, w=1") << Texture::Format::RGBA8Unorm << 1 << qsizetype(4) << qsizetype(4);
    QTest::newRow("RGBA_8888, w=2") << Texture::Format::RGBA8Unorm << 2 << qsizetype(8) << qsizetype(8);
    QTest::newRow("RGBA_8888, w=5") << Texture::Format::RGBA8Unorm << 5 << qsizetype(20) << qsizetype(20);
    QTest::newRow("RGBA_8888, w=8") << Texture::Format::RGBA8Unorm << 8 << qsizetype(32) << qsizetype(32);

    QTest::newRow("RGB_888, w=1") << Texture::Format::RGB8Unorm << 1 << qsizetype(3) << qsizetype(4);
    QTest::newRow("RGB_888, w=5") << Texture::Format::RGB8Unorm << 5 << qsizetype(15) << qsizetype(16);
    QTest::newRow("RGB_888, w=8") << Texture::Format::RGB8Unorm << 8 << qsizetype(24) << qsizetype(24);

    QTest::newRow("BGR_565, w=1") << Texture::Format::BGR565Unorm << 1 << qsizetype(2) << qsizetype(4);
    QTest::newRow("BGR_565, w=5") << Texture::Format::BGR565Unorm << 5 << qsizetype(10) << qsizetype(12);
    QTest::newRow("BGR_565, w=8") << Texture::Format::BGR565Unorm << 8 << qsizetype(16) << qsizetype(16);

    QTest::newRow("L8, w=1") << Texture::Format::L8Unorm << 1 << qsizetype(1) << qsizetype(4);
    QTest::newRow("L8, w=5") << Texture::Format::L8Unorm << 5 << qsizetype(5) << qsizetype(8);
    QTest::newRow("L8, w=8") << Texture::Format::L8Unorm << 8 << qsizetype(8) << qsizetype(8);

    QTest::newRow("DXT1, w=1") << Texture::Format::Bc1RgbUnorm << 1 << qsizetype(8) << qsizetype(8);
    QTest::newRow("DXT1, w=5") << Texture::Format::Bc1RgbUnorm << 5 << qsizetype(16) << qsizetype(16);
    QTest::newRow("DXT1, w=8") << Texture::Format::Bc1RgbUnorm << 8 << qsizetype(16) << qsizetype(16);

    QTest::newRow("DXT5, w=1") << Texture::Format::Bc1RgbUnorm << 1 << qsizetype(8) << qsizetype(8);
    QTest::newRow("DXT5, w=5") << Texture::Format::Bc1RgbUnorm << 5 << qsizetype(16) << qsizetype(16);
    QTest::newRow("DXT5, w=8") << Texture::Format::Bc1RgbUnorm << 8 << qsizetype(16) << qsizetype(16);
}

void TestTexture::bytesPerLine()
{
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(qsizetype, bpl1);
    QFETCH(qsizetype, bpl4);

    const auto result1 = Texture::calculateBytesPerLine(format, width, Texture::Alignment::Byte);
    QCOMPARE(result1, bpl1);

    const auto result4 = Texture::calculateBytesPerLine(format, width, Texture::Alignment::Word);
    QCOMPARE(result4, bpl4);
}

void TestTexture::offset_data()
{
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<int>("levels");
    QTest::addColumn<int>("layers");
    QTest::addColumn<int>("level");
    QTest::addColumn<int>("layer");
    QTest::addColumn<qsizetype>("offset");

    QTest::newRow("RGBA_8888, 1x1x1, level 0/1, layer 0/1")
            << Texture::Format::RGBA8Unorm
            << 1 << 1 << 1
            << 1 << 1
            << 0 << 0
            << qsizetype(0);

    QTest::newRow("RGBA_8888, 64x64x1, level 0/1, layer 1/10")
            << Texture::Format::RGBA8Unorm
            << 64 << 64 << 1
            << 1 << 10
            << 0 << 1
            << qsizetype(1 * 64*64*4);

    QTest::newRow("RGBA_8888, 64x64x1, level 1/2, layer 0/1")
            << Texture::Format::RGBA8Unorm
            << 64 << 64 << 1
            << 2 << 1
            << 1 << 0
            << qsizetype(64*64*4);

    QTest::newRow("RGBA_8888, 64x64x1, level 2/3, layer 0/1")
            << Texture::Format::RGBA8Unorm
            << 64 << 64 << 1
            << 3 << 1
            << 2 << 0
            << qsizetype(64*64*4 + 32*32*4);
}

void TestTexture::offset()
{
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, depth);
    QFETCH(int, levels);
    QFETCH(int, layers);
    QFETCH(int, level);
    QFETCH(int, layer);
    QFETCH(qsizetype, offset);

    const auto texture = Texture::create(format, {width, height, depth}, levels, layers);

    QVERIFY(!texture.isNull());
    QCOMPARE(texture.offset({level, layer}), offset);
}

QTEST_MAIN(TestTexture)

#include "test_texture.moc"
