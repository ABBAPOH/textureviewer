#include <QtTest>

#include <TextureLib/TexelFormat>

class TestTexelFormat : public QObject
{
    Q_OBJECT
private slots:
#if defined(Q_OS_LINUX)
    void benchFindOGLFormatConst();
#endif // Q_OS_LINUX
    void benchFindOGLFormatLinear();
};

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
