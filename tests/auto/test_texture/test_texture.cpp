#include <QtTest>
#include <TextureLib/Texture>

class TestTexture : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstructed();

    void construct_data();
    void construct();
    void constructWithData();
    void constructWithInvalidData();
    void bytesPerLine_data();
    void bytesPerLine();
};

void TestTexture::defaultConstructed()
{
    Texture tex;
    QCOMPARE(tex.isNull(), true);
    QCOMPARE(tex.format(), TextureFormat::Invalid);
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
    QTest::addColumn<TextureFormat>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<bool>("cubemap");
    QTest::addColumn<int>("levels");
    QTest::addColumn<int>("layers");
    QTest::addColumn<qsizetype>("bytes");

    // 1D texture
    QTest::newRow("RGBA_8888, 1x1x1, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm // type / format
            << 1 << 1 << 1 // width / height / depth
            << false // cubemap
            << 1 << 1 // levels / layers
            << qsizetype(4); // result
    QTest::newRow("RGBA_8888, 64x1x1, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 64 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(256);
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(400);
    // 1D texture array
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(400);
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=8")
            << TextureFormat::RGBA8_Unorm
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 8
            << qsizetype(3200);
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=10")
            << TextureFormat::RGBA8_Unorm
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 10
            << qsizetype(4000);
    // 2D texture
    QTest::newRow("RGBA_8888, 1x1x1, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 1 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(4);
    QTest::newRow("RGBA_8888, 64x64x1, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 64 << 64 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(16384);
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(40000);
    // 2D array
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(40000);
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=8")
            << TextureFormat::RGBA8_Unorm
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 8
            << qsizetype(320000);
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=10")
            << TextureFormat::RGBA8_Unorm
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 10
            << qsizetype(400000);
    // 3D texture
    QTest::newRow("RGBA_8888, 64x64x64, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 64 << 64 << 64
            << false // cubemap
            << 1 << 1
            << qsizetype(1048576);
    QTest::newRow("RGBA_8888, 100x100x100, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 100 << 100 << 100
            << false // cubemap
            << 1 << 1
            << qsizetype(4000000);
    // CubeMap
    QTest::newRow("RGBA_8888, size=1, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 1 << 1 << 1
            << true // cubemap
            << 1 << 1
            << qsizetype(24);
    QTest::newRow("RGBA_8888, size=64, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 64 << 64 << 1
            << true // cubemap
            << 1 << 1
            << qsizetype(98304);
    // CubeMap Array
    QTest::newRow("RGBA_8888, size=64, levels=1, layers=1")
            << TextureFormat::RGBA8_Unorm
            << 64 << 64 << 1
            << true // cubemap
            << 1 << 1
            << qsizetype(98304);
    QTest::newRow("RGBA_8888, size=64, levels=1, layers=8")
            << TextureFormat::RGBA8_Unorm
            << 64 << 64 << 1
            << true // cubemap
            << 1 << 8
            << qsizetype(786432);
}

void TestTexture::construct()
{
    QFETCH(TextureFormat, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, depth);
    QFETCH(bool, cubemap);
    QFETCH(int, layers);
    QFETCH(int, levels);
    QFETCH(qsizetype, bytes);

    const auto texture = Texture(
                format, {width, height, depth}, {Texture::IsCubemap(cubemap), levels, layers});

    QVERIFY(!texture.isNull());
    QCOMPARE(texture.format(), format);
    QCOMPARE(texture.width(), width);
    QCOMPARE(texture.height(), height);
    QCOMPARE(texture.depth(), depth);
    QCOMPARE(texture.layers(), layers);
    QCOMPARE(texture.levels(), levels);
    QCOMPARE(texture.bytes(), bytes);
}

void TestTexture::constructWithData()
{
    int width = 256;
    int height = 256;
    qsizetype size = width * height * 4;
    std::unique_ptr<uchar[]> data(new uchar[size]);

    {
        const auto deleter = [&data](uchar []) { data.reset(); };
        auto texture = Texture({data.get(), size}, deleter, TextureFormat::RGBA8_Unorm, {width, height});
        QVERIFY(!texture.isNull());
    }

    QVERIFY(!data);
}

void TestTexture::constructWithInvalidData()
{
    int width = 256;
    int height = 256;
    qsizetype size = width * height;
    std::unique_ptr<uchar[]> data(new uchar[size]);

    {
        const auto deleter = [&data](uchar []) { data.reset(); };
        auto texture = Texture({data.get(), size}, deleter, TextureFormat::RGBA8_Unorm, {width, height});
        QVERIFY(texture.isNull());
    }

    QVERIFY(data);
}

void TestTexture::bytesPerLine_data()
{
    QTest::addColumn<TextureFormat>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<qsizetype>("bpl1");
    QTest::addColumn<qsizetype>("bpl4");

    QTest::newRow("RGBA_8888, w=1") << TextureFormat::RGBA8_Unorm << 1 << qsizetype(4) << qsizetype(4);
    QTest::newRow("RGBA_8888, w=2") << TextureFormat::RGBA8_Unorm << 2 << qsizetype(8) << qsizetype(8);
    QTest::newRow("RGBA_8888, w=5") << TextureFormat::RGBA8_Unorm << 5 << qsizetype(20) << qsizetype(20);
    QTest::newRow("RGBA_8888, w=8") << TextureFormat::RGBA8_Unorm << 8 << qsizetype(32) << qsizetype(32);

    QTest::newRow("RGB_888, w=1") << TextureFormat::RGB8_Unorm << 1 << qsizetype(3) << qsizetype(4);
    QTest::newRow("RGB_888, w=5") << TextureFormat::RGB8_Unorm << 5 << qsizetype(15) << qsizetype(16);
    QTest::newRow("RGB_888, w=8") << TextureFormat::RGB8_Unorm << 8 << qsizetype(24) << qsizetype(24);

    QTest::newRow("BGR_565, w=1") << TextureFormat::BGR565_Unorm << 1 << qsizetype(2) << qsizetype(4);
    QTest::newRow("BGR_565, w=5") << TextureFormat::BGR565_Unorm << 5 << qsizetype(10) << qsizetype(12);
    QTest::newRow("BGR_565, w=8") << TextureFormat::BGR565_Unorm << 8 << qsizetype(16) << qsizetype(16);

    QTest::newRow("L8, w=1") << TextureFormat::L8_Unorm << 1 << qsizetype(1) << qsizetype(4);
    QTest::newRow("L8, w=5") << TextureFormat::L8_Unorm << 5 << qsizetype(5) << qsizetype(8);
    QTest::newRow("L8, w=8") << TextureFormat::L8_Unorm << 8 << qsizetype(8) << qsizetype(8);

    QTest::newRow("DXT1, w=1") << TextureFormat::Bc1Rgb_Unorm << 1 << qsizetype(8) << qsizetype(8);
    QTest::newRow("DXT1, w=5") << TextureFormat::Bc1Rgb_Unorm << 5 << qsizetype(16) << qsizetype(16);
    QTest::newRow("DXT1, w=8") << TextureFormat::Bc1Rgb_Unorm << 8 << qsizetype(16) << qsizetype(16);

    QTest::newRow("DXT5, w=1") << TextureFormat::Bc1Rgb_Unorm << 1 << qsizetype(8) << qsizetype(8);
    QTest::newRow("DXT5, w=5") << TextureFormat::Bc1Rgb_Unorm << 5 << qsizetype(16) << qsizetype(16);
    QTest::newRow("DXT5, w=8") << TextureFormat::Bc1Rgb_Unorm << 8 << qsizetype(16) << qsizetype(16);
}

void TestTexture::bytesPerLine()
{
    QFETCH(TextureFormat, format);
    QFETCH(int, width);
    QFETCH(qsizetype, bpl1);
    QFETCH(qsizetype, bpl4);

    const auto result1 = Texture::calculateBytesPerLine(format, width, Texture::Alignment::Byte);
    QCOMPARE(result1, bpl1);

    const auto result4 = Texture::calculateBytesPerLine(format, width, Texture::Alignment::Word);
    QCOMPARE(result4, bpl4);
}

QTEST_MAIN(TestTexture)

#include "test_texture.moc"
