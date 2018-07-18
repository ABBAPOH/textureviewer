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
            << Texture::Format::RGBA_8888 // type / format
            << 1 << 1 << 1 // width / height / depth
            << false // cubemap
            << 1 << 1 // levels / layers
            << qsizetype(4); // result
    QTest::newRow("RGBA_8888, 64x1x1, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 64 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(256);
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(400);
    // 1D texture array
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(400);
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=8")
            << Texture::Format::RGBA_8888
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 8
            << qsizetype(3200);
    QTest::newRow("RGBA_8888, 100x1x1, levels=1, layers=10")
            << Texture::Format::RGBA_8888
            << 100 << 1 << 1
            << false // cubemap
            << 1 << 10
            << qsizetype(4000);
    // 2D texture
    QTest::newRow("RGBA_8888, 1x1x1, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 1 << 1 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(4);
    QTest::newRow("RGBA_8888, 64x64x1, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 64 << 64 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(16384);
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(40000);
    // 2D array
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 1
            << qsizetype(40000);
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=8")
            << Texture::Format::RGBA_8888
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 8
            << qsizetype(320000);
    QTest::newRow("RGBA_8888, 100x100x1, levels=1, layers=10")
            << Texture::Format::RGBA_8888
            << 100 << 100 << 1
            << false // cubemap
            << 1 << 10
            << qsizetype(400000);
    // 3D texture
    QTest::newRow("RGBA_8888, 64x64x64, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 64 << 64 << 64
            << false // cubemap
            << 1 << 1
            << qsizetype(1048576);
    QTest::newRow("RGBA_8888, 100x100x100, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 100 << 100 << 100
            << false // cubemap
            << 1 << 1
            << qsizetype(4000000);
    // CubeMap
    QTest::newRow("RGBA_8888, size=1, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 1 << 1 << 1
            << true // cubemap
            << 1 << 1
            << qsizetype(24);
    QTest::newRow("RGBA_8888, size=64, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 64 << 64 << 1
            << true // cubemap
            << 1 << 1
            << qsizetype(98304);
    // CubeMap Array
    QTest::newRow("RGBA_8888, size=64, levels=1, layers=1")
            << Texture::Format::RGBA_8888
            << 64 << 64 << 1
            << true // cubemap
            << 1 << 1
            << qsizetype(98304);
    QTest::newRow("RGBA_8888, size=64, levels=1, layers=8")
            << Texture::Format::RGBA_8888
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
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<int>("levels");
    QTest::addColumn<int>("level");
    QTest::addColumn<qsizetype>("bpt");
    QTest::addColumn<qsizetype>("bpl");
    QTest::addColumn<qsizetype>("bpi");

    QTest::newRow("RGBA_8888, 1x1x1, level 0/1")
            << Texture::Format::RGBA_8888
            << 1 << 1 << 1
            << 1 << 0
            << qsizetype(32) << qsizetype(4) << qsizetype(4);
    QTest::newRow("RGBA_8888, 64x1x1, level 0/1")
            << Texture::Format::RGBA_8888
            << 64 << 1 << 1
            << 1 << 0
            << qsizetype(32) << qsizetype(256) << qsizetype(256);
    QTest::newRow("RGBA_8888, 64x64x64, level 0/1")
            << Texture::Format::RGBA_8888
            << 64 << 64 << 64
            << 1 << 0
            << qsizetype(32) << qsizetype(256) << qsizetype(1048576);
    QTest::newRow("RGBA_8888, 4x4x4, level 0/1")
            << Texture::Format::RGBA_8888
            << 4 << 4 << 4
            << 1 << 0
            << qsizetype(32) << qsizetype(16) << qsizetype(256);
    QTest::newRow("RGBA_8888, 4x4x4, level 0/3")
            << Texture::Format::RGBA_8888
            << 4 << 4 << 4
            << 3 << 0
            << qsizetype(32) << qsizetype(16) << qsizetype(256);
    QTest::newRow("RGBA_8888, 4x4x4, level 1/3")
            << Texture::Format::RGBA_8888
            << 4 << 4 << 4
            << 3 << 1
            << qsizetype(32) << qsizetype(8) << qsizetype(32);
    QTest::newRow("RGBA_8888, 4x4x4, level 2/3")
            << Texture::Format::RGBA_8888
            << 4 << 4 << 4
            << 3 << 2
            << qsizetype(32) << qsizetype(4) << qsizetype(4);
    QTest::newRow("RGBA_8888, 5x4x4, level 0/3")
            << Texture::Format::RGBA_8888
            << 5 << 4 << 4
            << 3 << 0
            << qsizetype(32) << qsizetype(20) << qsizetype(320);
    QTest::newRow("RGBA_8888, 5x4x4, level 1/3")
            << Texture::Format::RGBA_8888
            << 5 << 4 << 4
            << 3 << 1
            << qsizetype(32) << qsizetype(8) << qsizetype(32);
    QTest::newRow("RGBA_8888, 5x4x4, level 2/3")
            << Texture::Format::RGBA_8888
            << 4 << 4 << 4
            << 3 << 2
            << qsizetype(32) << qsizetype(4) << qsizetype(4);
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

    const auto texture = Texture::create(format, {width, height, depth}, levels);

    QVERIFY(!texture.isNull());
    QCOMPARE(texture.bitsPerTexel(), bpt);
    QCOMPARE(texture.bytesPerLine(level), bpl);
    QCOMPARE(texture.bytesPerImage(level), bpi);
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
            << Texture::Format::RGBA_8888
            << 1 << 1 << 1
            << 1 << 1
            << 0 << 0
            << qsizetype(0);

    QTest::newRow("RGBA_8888, 64x64x1, level 0/1, layer 1/10")
            << Texture::Format::RGBA_8888
            << 64 << 64 << 1
            << 1 << 10
            << 0 << 1
            << qsizetype(1 * 64*64*4);

    QTest::newRow("RGBA_8888, 64x64x1, level 1/2, layer 0/1")
            << Texture::Format::RGBA_8888
            << 64 << 64 << 1
            << 2 << 1
            << 1 << 0
            << qsizetype(64*64*4);

    QTest::newRow("RGBA_8888, 64x64x1, level 2/3, layer 0/1")
            << Texture::Format::RGBA_8888
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
