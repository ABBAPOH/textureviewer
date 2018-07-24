#include <QtTest>

#include <TextureLib/TexelFormat>

class TestTexelFormat : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstructed();
    void testFindOGLFormat();
#if defined(Q_OS_LINUX)
    void benchFindOGLFormatConst();
#endif // Q_OS_LINUX
    void benchFindOGLFormatLinear();
};

void TestTexelFormat::defaultConstructed()
{
    TexelFormat format;

    QCOMPARE(format.format(), Texture::Format::Invalid);
    QCOMPARE(format.bitsPerTexel(), 0);
    QCOMPARE(format.blockSize(), 0);
    QCOMPARE(format.isCompressed(), false);
    QCOMPARE(format.oglTextureFormat(), QOpenGLTexture::NoFormat);
    QCOMPARE(format.oglPixelFormat(), QOpenGLTexture::NoSourceFormat);
    QCOMPARE(format.oglPixelType(), QOpenGLTexture::NoPixelType);
}

void TestTexelFormat::testFindOGLFormat()
{
    const auto formats = TexelFormat::texelFormats();
    for (const auto &i: formats) {
        if (i.format() == Texture::Format::RXGB)
            continue; // Not supported in ktx
        auto j = TexelFormat::findOGLFormatLinear(i.oglTextureFormat(), i.oglPixelFormat(), i.oglPixelType());
        if (i != j)
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
