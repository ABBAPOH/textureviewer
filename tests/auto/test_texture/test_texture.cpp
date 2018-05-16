#include <QtTest>
#include <TextureLib/Texture>

class TestTexture : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstructed();
    void construct1D_data();
    void construct1D();
    void construct1DArray_data();
    void construct1DArray();
    void construct2D_data();
    void construct2D();
    void construct2DArray_data();
    void construct2DArray();
    void construct3D_data();
    void construct3D();
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

void TestTexture::construct1D_data()
{
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("levels");
    QTest::addColumn<qsizetype>("bytes");

    QTest::newRow("ARGB32, 1x1x1, 1") << Texture::Format::ARGB32 << 1 << 1 << qsizetype(4);
    QTest::newRow("ARGB32, 64x1x1, 1") << Texture::Format::ARGB32 << 64 << 1 << qsizetype(256);
    QTest::newRow("ARGB32, 100x1x1, 1") << Texture::Format::ARGB32 << 100 << 1 << qsizetype(400);
}

void TestTexture::construct1D()
{
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, levels);
    QFETCH(qsizetype, bytes);

    auto texture = Texture::create1DTexture(width, format);
    QCOMPARE(texture.isNull(), false);
    QCOMPARE(texture.format(), format);
    QCOMPARE(texture.width(), width);
    QCOMPARE(texture.height(), 1);
    QCOMPARE(texture.depth(), 1);
    QCOMPARE(texture.layers(), 1);
    QCOMPARE(texture.levels(), levels);
    QCOMPARE(texture.bytes(), bytes);
}

void TestTexture::construct1DArray_data()
{
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("layers");
    QTest::addColumn<int>("levels");
    QTest::addColumn<qsizetype>("bytes");

    QTest::newRow("ARGB32, 1x1x1, 1, 1") << Texture::Format::ARGB32 << 1 << 1 << 1 << qsizetype(4);
    QTest::newRow("ARGB32, 100x1x1, 1, 1") << Texture::Format::ARGB32 << 100 << 1 << 1 << qsizetype(400);
    QTest::newRow("ARGB32, 100x1x1, 8, 1") << Texture::Format::ARGB32 << 100 << 8 << 1 << qsizetype(3200);
}

void TestTexture::construct1DArray()
{
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, layers);
    QFETCH(int, levels);
    QFETCH(qsizetype, bytes);

    auto texture = Texture::create1DTextureArray(width, layers, format);
    QCOMPARE(texture.isNull(), false);
    QCOMPARE(texture.format(), format);
    QCOMPARE(texture.width(), width);
    QCOMPARE(texture.height(), 1);
    QCOMPARE(texture.depth(), 1);
    QCOMPARE(texture.layers(), layers);
    QCOMPARE(texture.levels(), levels);
    QCOMPARE(texture.bytes(), bytes);
}

void TestTexture::construct2D_data()
{
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("levels");
    QTest::addColumn<qsizetype>("bytes");

    QTest::newRow("ARGB32, 1x1x1, 1") << Texture::Format::ARGB32 << 1 << 1 << 1 << qsizetype(4);
    QTest::newRow("ARGB32, 64x64x1, 1") << Texture::Format::ARGB32 << 64 << 64 << 1 << qsizetype(16384);
    QTest::newRow("ARGB32, 100x100x1, 1") << Texture::Format::ARGB32 << 100 << 100 << 1 << qsizetype(40000);
}

void TestTexture::construct2D()
{
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, levels);
    QFETCH(qsizetype, bytes);

    auto texture = Texture::create2DTexture(width, height, format);
    QCOMPARE(texture.isNull(), false);
    QCOMPARE(texture.format(), format);
    QCOMPARE(texture.width(), width);
    QCOMPARE(texture.height(), height);
    QCOMPARE(texture.depth(), 1);
    QCOMPARE(texture.layers(), 1);
    QCOMPARE(texture.levels(), levels);
    QCOMPARE(texture.bytes(), bytes);
}

void TestTexture::construct2DArray_data()
{
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("layers");
    QTest::addColumn<int>("levels");
    QTest::addColumn<qsizetype>("bytes");

    QTest::newRow("ARGB32, 1x1x1, 1, 1") << Texture::Format::ARGB32 << 1 << 1 << 1 << 1 << qsizetype(4);
    QTest::newRow("ARGB32, 100x100x1, 1, 1") << Texture::Format::ARGB32 << 100 << 100 << 1 << 1 << qsizetype(40000);
    QTest::newRow("ARGB32, 100x100x1, 8, 1") << Texture::Format::ARGB32 << 100 << 100 << 8 << 1 << qsizetype(320000);
}

void TestTexture::construct2DArray()
{
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, layers);
    QFETCH(int, levels);
    QFETCH(qsizetype, bytes);

    auto texture = Texture::create2DTextureArray(width, height, layers, format);
    QCOMPARE(texture.isNull(), false);
    QCOMPARE(texture.format(), format);
    QCOMPARE(texture.width(), width);
    QCOMPARE(texture.height(), height);
    QCOMPARE(texture.depth(), 1);
    QCOMPARE(texture.layers(), layers);
    QCOMPARE(texture.levels(), levels);
    QCOMPARE(texture.bytes(), bytes);
}

void TestTexture::construct3D_data()
{
    QTest::addColumn<Texture::Format>("format");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");
    QTest::addColumn<int>("depth");
    QTest::addColumn<int>("levels");
    QTest::addColumn<qsizetype>("bytes");

    QTest::newRow("ARGB32, 1x1x1, 1") << Texture::Format::ARGB32 << 1 << 1 << 1 << 1 << qsizetype(4);
    QTest::newRow("ARGB32, 64x64x64, 1") << Texture::Format::ARGB32 << 64 << 64 << 64 << 1 << qsizetype(1048576);
    QTest::newRow("ARGB32, 100x100x100, 1") << Texture::Format::ARGB32 << 100 << 100 << 100 << 1 << qsizetype(4000000);
}

void TestTexture::construct3D()
{
    QFETCH(Texture::Format, format);
    QFETCH(int, width);
    QFETCH(int, height);
    QFETCH(int, depth);
    QFETCH(int, levels);
    QFETCH(qsizetype, bytes);

    auto texture = Texture::create3DTexture(width, height, depth, format);
    QCOMPARE(texture.isNull(), false);
    QCOMPARE(texture.format(), format);
    QCOMPARE(texture.width(), width);
    QCOMPARE(texture.height(), height);
    QCOMPARE(texture.depth(), depth);
    QCOMPARE(texture.layers(), 1);
    QCOMPARE(texture.levels(), levels);
    QCOMPARE(texture.bytes(), bytes);
}

QTEST_MAIN(TestTexture)

#include "test_texture.moc"
