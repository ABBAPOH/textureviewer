#include <QtTest>

#include <TextureLib/TextureFormatInfo>

class TestTextureFormat : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstructed();
    void constructed_data();
    void constructed();
    void compare_data();
    void compare();
};

void TestTextureFormat::defaultConstructed()
{
    TextureFormatInfo format;

    QCOMPARE(format.format(), TextureFormat::Invalid);
    QCOMPARE(format.type(), TextureFormatInfo::Type::None);
    QCOMPARE(format.size(), 0);
    QCOMPARE(format.bytesPerTexel(), 0);
    QCOMPARE(format.blockSize(), 0);
    QCOMPARE(format.isCompressed(), false);
    QCOMPARE(format.oglTextureFormat(), QOpenGLTexture::NoFormat);
    QCOMPARE(format.oglPixelFormat(), QOpenGLTexture::NoSourceFormat);
    QCOMPARE(format.oglPixelType(), QOpenGLTexture::NoPixelType);
}

void TestTextureFormat::constructed_data()
{
    QTest::addColumn<TextureFormat>("format");
    QTest::addColumn<TextureFormatInfo::Type>("type");
    QTest::addColumn<int>("size");
    QTest::addColumn<QOpenGLTexture::TextureFormat>("textureFormat");
    QTest::addColumn<QOpenGLTexture::PixelFormat>("pixelFormat");
    QTest::addColumn<QOpenGLTexture::PixelType>("pixelType");

    QTest::newRow("RGBA_8888")
            << TextureFormat::RGBA8_Unorm << TextureFormatInfo::Type::UnsignedNormalized << 32
            << QOpenGLTexture::RGBA8_UNorm << QOpenGLTexture::RGBA << QOpenGLTexture::UInt8;
}

void TestTextureFormat::constructed()
{
    QFETCH(TextureFormat, format);
    QFETCH(TextureFormatInfo::Type, type);
    QFETCH(int, size);
    QFETCH(QOpenGLTexture::TextureFormat, textureFormat);
    QFETCH(QOpenGLTexture::PixelFormat, pixelFormat);
    QFETCH(QOpenGLTexture::PixelType, pixelType);

    const TextureFormatInfo texelFormat = {
        format,
        type,
        size,
        textureFormat,
        pixelFormat,
        pixelType
    };

    QCOMPARE(texelFormat.format(), format);
    QCOMPARE(texelFormat.type(), type);
    QCOMPARE(texelFormat.size(), size);
    QCOMPARE(texelFormat.oglTextureFormat(), textureFormat);
    QCOMPARE(texelFormat.oglPixelFormat(), pixelFormat);
    QCOMPARE(texelFormat.oglPixelType(), pixelType);
}

void TestTextureFormat::compare_data()
{
    QTest::addColumn<TextureFormat>("format");
    QTest::addColumn<TextureFormatInfo::Type>("type");
    QTest::addColumn<int>("size");
    QTest::addColumn<QOpenGLTexture::TextureFormat>("textureFormat");
    QTest::addColumn<QOpenGLTexture::PixelFormat>("pixelFormat");
    QTest::addColumn<QOpenGLTexture::PixelType>("pixelType");

    QTest::newRow("RGBA_8888")
            << TextureFormat::RGBA8_Unorm << TextureFormatInfo::Type::UnsignedNormalized << 32
            << QOpenGLTexture::RGBA8_UNorm << QOpenGLTexture::RGBA << QOpenGLTexture::UInt8;

}

void TestTextureFormat::compare()
{
    QFETCH(TextureFormat, format);
    QFETCH(TextureFormatInfo::Type, type);
    QFETCH(int, size);
    QFETCH(QOpenGLTexture::TextureFormat, textureFormat);
    QFETCH(QOpenGLTexture::PixelFormat, pixelFormat);
    QFETCH(QOpenGLTexture::PixelType, pixelType);

    const TextureFormatInfo texelFormat0;
    const TextureFormatInfo texelFormat1 = {
        format,
        type,
        size,
        textureFormat,
        pixelFormat,
        pixelType
    };

    TextureFormatInfo texelFormat2 = {
        format,
        type,
        size,
        textureFormat,
        pixelFormat,
        pixelType
    };

    QVERIFY(texelFormat1 == texelFormat1);
    QVERIFY(texelFormat2 == texelFormat2);
    QVERIFY(texelFormat1 == texelFormat2);
    QVERIFY(texelFormat1 != texelFormat0);
    QVERIFY(texelFormat2 != texelFormat0);

    texelFormat2 = texelFormat0;
    QVERIFY(texelFormat2 == texelFormat0);
    QVERIFY(texelFormat2 != texelFormat1);
}

QTEST_APPLESS_MAIN(TestTextureFormat)

#include "test_textureformat.moc"
