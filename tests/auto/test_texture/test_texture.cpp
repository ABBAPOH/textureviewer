#include <QtTest>
#include <TextureLib/Texture>

class TestTexture : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstructed();
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

    QCOMPARE(tex.data(), nullptr);
    QCOMPARE(tex.constData(), nullptr);

    QCOMPARE(tex.scanLine(0), nullptr);
    QCOMPARE(tex.constScanLine(0), nullptr);
}

QTEST_MAIN(TestTexture)

#include "test_texture.moc"
