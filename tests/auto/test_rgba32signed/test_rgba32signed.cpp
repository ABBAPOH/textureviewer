#include <QtTest>

#include <TextureLib/Rgba32Signed>

class TestRgba32Signed : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstruction();
    void api_data();
    void api();
    void copy();
    void assign();
};

void TestRgba32Signed::defaultConstruction()
{
    Rgba32Signed c;
    QCOMPARE(c.red(), 0);
    QCOMPARE(c.green(), 0);
    QCOMPARE(c.blue(), 0);
    QCOMPARE(c.alpha(), 0);
}

void TestRgba32Signed::api_data()
{
    QTest::addColumn<qint8>("red");
    QTest::addColumn<qint8>("green");
    QTest::addColumn<qint8>("blue");
    QTest::addColumn<qint8>("alpha");

    QTest::newRow("black") << qint8(0) << qint8(0) << qint8(0) << qint8(0);
    QTest::newRow("red") << qint8(127) << qint8(0) << qint8(0) << qint8(127);
    QTest::newRow("red green") << qint8(127) << qint8(127) << qint8(0) << qint8(127);
    QTest::newRow("white") << qint8(127) << qint8(127) << qint8(127) << qint8(127);
    QTest::newRow("negative red") << qint8(-128) << qint8(0) << qint8(0) << qint8(-128);
    QTest::newRow("negative white") << qint8(-128) << qint8(-128) << qint8(-128) << qint8(-128);
}

void TestRgba32Signed::api()
{
    QFETCH(qint8, red);
    QFETCH(qint8, green);
    QFETCH(qint8, blue);
    QFETCH(qint8, alpha);

    // construction
    Rgba32Signed c(red, green, blue, alpha);

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
    Rgba32Signed c2;

    c2.setRed(red);
    c2.setGreen(green);
    c2.setBlue(blue);
    c2.setAlpha(alpha);

    QCOMPARE(c2.red(), red);
    QCOMPARE(c2.green(), green);
    QCOMPARE(c2.blue(), blue);
    QCOMPARE(c2.alpha(), alpha);
}

void TestRgba32Signed::copy()
{
    Rgba32Signed c1(127, 127, 127, 127);
    Rgba32Signed c2(c1);

    QCOMPARE(c2.red(), c1.red());
    QCOMPARE(c2.green(), c1.green());
    QCOMPARE(c2.blue(), c1.blue());
    QCOMPARE(c2.alpha(), c1.alpha());
}

void TestRgba32Signed::assign()
{
    Rgba32Signed c1(127, 127, 127, 127);
    Rgba32Signed c2;

    c2 = c1;

    QCOMPARE(c2.red(), c1.red());
    QCOMPARE(c2.green(), c1.green());
    QCOMPARE(c2.blue(), c1.blue());
    QCOMPARE(c2.alpha(), c1.alpha());
}

QTEST_APPLESS_MAIN(TestRgba32Signed)

#include "test_rgba32signed.moc"
