#include <QtTest>

#include <TextureLib/Rgba64Float>

Q_DECLARE_METATYPE(HalfFloat)

class TestRgba64Float : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstruction();
    void api_data();
    void api();
    void copy();
    void assign();
    void fromRgba32();
    void toRgba32();
    void fromRgba64();
};

void TestRgba64Float::defaultConstruction()
{
    Rgba64Float c;
    QCOMPARE(c.red(), 0);
    QCOMPARE(c.green(), 0);
    QCOMPARE(c.blue(), 0);
    QCOMPARE(c.alpha(), 0);
}

void TestRgba64Float::api_data()
{
    QTest::addColumn<HalfFloat>("red");
    QTest::addColumn<HalfFloat>("green");
    QTest::addColumn<HalfFloat>("blue");
    QTest::addColumn<HalfFloat>("alpha");

    QTest::newRow("black") << HalfFloat(0) << HalfFloat(0) << HalfFloat(0) << HalfFloat(0);
    QTest::newRow("red") << HalfFloat(1.0f) << HalfFloat(0) << HalfFloat(0) << HalfFloat(1.0f);
    QTest::newRow("red green") << HalfFloat(1.0f) << HalfFloat(1.0f) << HalfFloat(0) << HalfFloat(1.0f);
    QTest::newRow("white") << HalfFloat(1.0f) << HalfFloat(1.0f) << HalfFloat(1.0f) << HalfFloat(1.0f);
    QTest::newRow("negative red") << HalfFloat(-1.0f) << HalfFloat(0) << HalfFloat(0) << HalfFloat(-1.0f);
    QTest::newRow("negative white") << HalfFloat(-1.0f) << HalfFloat(-1.0f) << HalfFloat(-1.0f) << HalfFloat(-1.0f);
}

void TestRgba64Float::api()
{
    QFETCH(HalfFloat, red);
    QFETCH(HalfFloat, green);
    QFETCH(HalfFloat, blue);
    QFETCH(HalfFloat, alpha);

    // construction
    Rgba64Float c(red, green, blue, alpha);

    QCOMPARE(c.red(), red);
    QCOMPARE(c.green(), green);
    QCOMPARE(c.blue(), blue);
    QCOMPARE(c.alpha(), alpha);

    // getHelpers

    QCOMPARE(getRed(c), red);
    QCOMPARE(getGreen(c), green);
    QCOMPARE(getBlue(c), blue);
    QCOMPARE(getAlpha(c), alpha);

    // setters
    Rgba64Float c2;

    c2.setRed(red);
    c2.setGreen(green);
    c2.setBlue(blue);
    c2.setAlpha(alpha);

    QCOMPARE(c2.red(), red);
    QCOMPARE(c2.green(), green);
    QCOMPARE(c2.blue(), blue);
    QCOMPARE(c2.alpha(), alpha);
}

void TestRgba64Float::copy()
{
    Rgba64Float c1(HalfFloat(1.0f), HalfFloat(1.0f), HalfFloat(1.0f), HalfFloat(1.0f));
    Rgba64Float c2(c1);

    QCOMPARE(c2.red(), c1.red());
    QCOMPARE(c2.green(), c1.green());
    QCOMPARE(c2.blue(), c1.blue());
    QCOMPARE(c2.alpha(), c1.alpha());
}

void TestRgba64Float::assign()
{
    Rgba64Float c1(HalfFloat(1.0f), HalfFloat(1.0f), HalfFloat(1.0f), HalfFloat(1.0f));
    Rgba64Float c2;

    c2 = c1;

    QCOMPARE(c2.red(), c1.red());
    QCOMPARE(c2.green(), c1.green());
    QCOMPARE(c2.blue(), c1.blue());
    QCOMPARE(c2.alpha(), c1.alpha());
}

void TestRgba64Float::fromRgba32()
{
    const auto rgba1 = rgba64Float(qRgba(127, 100, 50, 255));
    QCOMPARE(getRed(rgba1), HalfFloat(127.0f / 255.0f));
    QCOMPARE(getGreen(rgba1), HalfFloat(100.0f / 255.0f));
    QCOMPARE(getBlue(rgba1), HalfFloat(50.0f / 255.0f));
    QCOMPARE(getAlpha(rgba1), 1.0f);
}

void TestRgba64Float::toRgba32()
{
    const auto rgba32 = qRgba(rgba64Float(HalfFloat(0.5f), HalfFloat(0.3f), HalfFloat(0.1f), HalfFloat(1.0f)));
    QCOMPARE(qRed(rgba32), 128);
    QCOMPARE(qGreen(rgba32), 76);
    QCOMPARE(qBlue(rgba32), 25);
    QCOMPARE(qAlpha(rgba32), 255);
}

void TestRgba64Float::fromRgba64()
{
    const auto rgba = rgba64Float(qRgba64(0xffff, 0xff, 0, 0xffff));
    QCOMPARE(getRed(rgba), HalfFloat(1.0f));
    QCOMPARE(getGreen(rgba), HalfFloat(255.0f / 0xffff));
    QCOMPARE(getBlue(rgba), HalfFloat(0.0f));
    QCOMPARE(getAlpha(rgba), HalfFloat(1.0f));
}

QTEST_APPLESS_MAIN(TestRgba64Float)

#include "test_rgba64float.moc"
