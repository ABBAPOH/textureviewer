#include <QtTest>

#include <TextureLib/TexelFormat>

class TestTexelFormat : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstructed();
    void constructed_data();
    void constructed();
    void compare_data();
    void compare();
    void testFindOGLFormat();
#if defined(Q_OS_LINUX)
    void benchFindOGLFormatConst();
#endif // Q_OS_LINUX
    void benchFindOGLFormatLinear();
};

void TestTexelFormat::defaultConstructed()
{
    TexelFormat format;

    QCOMPARE(format.format(), TextureFormat::Invalid);
    QCOMPARE(format.bitsPerTexel(), 0);
    QCOMPARE(format.blockSize(), 0);
    QCOMPARE(format.isCompressed(), false);
    QCOMPARE(format.oglTextureFormat(), QOpenGLTexture::NoFormat);
    QCOMPARE(format.oglPixelFormat(), QOpenGLTexture::NoSourceFormat);
    QCOMPARE(format.oglPixelType(), QOpenGLTexture::NoPixelType);
}

void TestTexelFormat::constructed_data()
{
    QTest::addColumn<TextureFormat>("format");
    QTest::addColumn<int>("bitsPerTexel");
    QTest::addColumn<int>("blockSize");
    QTest::addColumn<QOpenGLTexture::TextureFormat>("textureFormat");
    QTest::addColumn<QOpenGLTexture::PixelFormat>("pixelFormat");
    QTest::addColumn<QOpenGLTexture::PixelType>("pixelType");

    QTest::newRow("RGBA_8888")
            << TextureFormat::RGBA8Unorm << 32 << 0
            << QOpenGLTexture::RGBA8_UNorm << QOpenGLTexture::RGBA << QOpenGLTexture::UInt8;
}

void TestTexelFormat::constructed()
{
    QFETCH(TextureFormat, format);
    QFETCH(int, bitsPerTexel);
    QFETCH(int, blockSize);
    QFETCH(QOpenGLTexture::TextureFormat, textureFormat);
    QFETCH(QOpenGLTexture::PixelFormat, pixelFormat);
    QFETCH(QOpenGLTexture::PixelType, pixelType);

    const TexelFormat texelFormat = {
        format,
        bitsPerTexel,
        blockSize,
        textureFormat,
        pixelFormat,
        pixelType
    };

    QCOMPARE(texelFormat.format(), format);
    QCOMPARE(texelFormat.bitsPerTexel(), bitsPerTexel);
    QCOMPARE(texelFormat.blockSize(), blockSize);
    QCOMPARE(texelFormat.oglTextureFormat(), textureFormat);
    QCOMPARE(texelFormat.oglPixelFormat(), pixelFormat);
    QCOMPARE(texelFormat.oglPixelType(), pixelType);
}

void TestTexelFormat::compare_data()
{
    QTest::addColumn<TextureFormat>("format");
    QTest::addColumn<int>("bitsPerTexel");
    QTest::addColumn<int>("blockSize");
    QTest::addColumn<QOpenGLTexture::TextureFormat>("textureFormat");
    QTest::addColumn<QOpenGLTexture::PixelFormat>("pixelFormat");
    QTest::addColumn<QOpenGLTexture::PixelType>("pixelType");

    QTest::newRow("RGBA_8888")
            << TextureFormat::RGBA8Unorm << 32 << 0
            << QOpenGLTexture::RGBA8_UNorm << QOpenGLTexture::RGBA << QOpenGLTexture::UInt8;

}

void TestTexelFormat::compare()
{
    QFETCH(TextureFormat, format);
    QFETCH(int, bitsPerTexel);
    QFETCH(int, blockSize);
    QFETCH(QOpenGLTexture::TextureFormat, textureFormat);
    QFETCH(QOpenGLTexture::PixelFormat, pixelFormat);
    QFETCH(QOpenGLTexture::PixelType, pixelType);

    const TexelFormat texelFormat0;
    const TexelFormat texelFormat1 = {
        format,
        bitsPerTexel,
        blockSize,
        textureFormat,
        pixelFormat,
        pixelType
    };

    TexelFormat texelFormat2 = {
        format,
        bitsPerTexel,
        blockSize,
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

void TestTexelFormat::testFindOGLFormat()
{
    const auto formats = TexelFormat::texelFormats();
    for (const auto &i: formats) {
        if (i.format() == TextureFormat::RXGB)
            continue; // Not supported in ktx
        auto j = TexelFormat::findOGLFormatLinear(i.oglTextureFormat(), i.oglPixelFormat(), i.oglPixelType());
        QCOMPARE(j, i);
    }
}

#if defined(Q_OS_LINUX)
void TestTexelFormat::benchFindOGLFormatConst()
{
    const auto formats = TexelFormat::texelFormats();
    QBENCHMARK {
        for (const auto &i: formats) {
            TexelFormat::findOGLFormatConst(i.oglTextureFormat(), i.oglPixelFormat(), i.oglPixelType());
        }
    }
}
#endif // Q_OS_LINUX

void TestTexelFormat::benchFindOGLFormatLinear()
{
    const auto formats = TexelFormat::texelFormats();
    QBENCHMARK {
        for (const auto &i: formats) {
            TexelFormat::findOGLFormatLinear(i.oglTextureFormat(), i.oglPixelFormat(), i.oglPixelType());
        }
    }
}

QTEST_APPLESS_MAIN(TestTexelFormat)

#include "test_texelformat.moc"
