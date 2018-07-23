#include <QtTest>

#include <TextureLib/TexelFormat>

class TestTexelFormat : public QObject
{
    Q_OBJECT
private slots:
    void benchFindOGLFormatConst();
    void benchFindOGLFormatLinear();
};

void TestTexelFormat::benchFindOGLFormatConst()
{
    const auto formats = TexelFormat::texelFormats();
    QBENCHMARK {
        for (const auto &i: formats) {
            TexelFormat::findOGLFormatConst(i.oglTextureFormat(), i.oglPixelFormat(), i.oglPixelType());
        }
    }
}

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
