#include <QtTest>

#include <TextureLib/ColorVariant>

class TestColorVariant : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstruction();
    void typeConstruction();
    void copy();
    void assign();
    void compare();
};

void TestColorVariant::defaultConstruction()
{
    ColorVariant c;
    QCOMPARE(c.type(), ColorVariant::Type::Invalid);
    QCOMPARE(c.isEmpty(), true);

    QCOMPARE(c.value<Rgba32Signed>(), Rgba32Signed());
    QCOMPARE(c.value<QRgb>(), QRgb());
    QCOMPARE(c.value<Rgba64Float>(), Rgba64Float());
    QCOMPARE(c.value<Rgba64Signed>(), Rgba64Signed());
    QCOMPARE(c.value<QRgba64>(), QRgba64());
    // 128-bit types are all the same
}

void TestColorVariant::typeConstruction()
{
    const auto rgba1 = rgba32Signed(100, 0, -50);
    const auto c1 = ColorVariant(rgba1);
    QCOMPARE(c1.type(), ColorVariant::Type::Rgba32Signed);
    QCOMPARE(c1.isEmpty(), false);
    QCOMPARE(c1.value<Rgba32Signed>(), rgba1);

    const auto rgba2 = qRgb(255, 50, 0);
    const auto c2 = ColorVariant(rgba2);
    QCOMPARE(c2.type(), ColorVariant::Type::Rgba32Unsigned);
    QCOMPARE(c2.isEmpty(), false);
    QCOMPARE(c2.value<QRgb>(), rgba2);

    const auto rgba3 = rgba64Float(1.0f, 0.0f, -1.0f);
    const auto c3 = ColorVariant(rgba3);
    QCOMPARE(c3.type(), ColorVariant::Type::Rgba64Float);
    QCOMPARE(c3.isEmpty(), false);
    QCOMPARE(c3.value<Rgba64Float>(), rgba3);

    const auto rgba4 = rgba64Signed(0x7f00, 0, -0x7f00);
    const auto c4 = ColorVariant(rgba4);
    QCOMPARE(c4.type(), ColorVariant::Type::Rgba64Signed);
    QCOMPARE(c4.isEmpty(), false);
    QCOMPARE(c4.value<Rgba64Signed>(), rgba4);

    const auto rgba5 = qRgba64(0xff00, 0x00ff, 0, 0xffff);
    const auto c5 = ColorVariant(rgba5);
    QCOMPARE(c5.type(), ColorVariant::Type::Rgba64Unsigned);
    QCOMPARE(c5.isEmpty(), false);
    QCOMPARE(c5.value<QRgba64>(), rgba5);

    const auto rgba6 = rgba128Float(1.0f, 0.0f, -1.0f);
    const auto c6 = ColorVariant(rgba6);
    QCOMPARE(c6.type(), ColorVariant::Type::Rgba128Float);
    QCOMPARE(c6.isEmpty(), false);
    QCOMPARE(c6.value<Rgba128Float>(), rgba6);
}

void TestColorVariant::copy()
{
    const auto rgba = qRgb(255, 50, 0);
    const auto c0 = ColorVariant();
    const auto c1 = ColorVariant(rgba);

    auto c2 = ColorVariant(c0);
    QCOMPARE(c2.isEmpty(), true);

    auto c3 = ColorVariant(c1);
    QCOMPARE(c3.isEmpty(), false);
    QCOMPARE(c3.value<QRgb>(), rgba);
}

void TestColorVariant::assign()
{
    const auto rgba = qRgb(255, 50, 0);
    const auto c1 = ColorVariant(rgba);

    auto c2 = ColorVariant();
    QCOMPARE(c2.isEmpty(), true);
    c2 = c1;
    QCOMPARE(c2.isEmpty(), false);
    QCOMPARE(c2.value<QRgb>(), rgba);
}

void TestColorVariant::compare()
{
    const auto c1 = ColorVariant(qRgb(255, 50, 0));
    const auto c2 = ColorVariant(qRgb(255, 50, 0));
    const auto c3 = ColorVariant(qRgb(127, 25, 0));

    QVERIFY(c1 == c2);
    QVERIFY(c1 != c3);

    QVERIFY(c3 < c1);
    QVERIFY(c1 > c3);

    QVERIFY(c3 <= c1);
    QVERIFY(c1 >= c3);
}

QTEST_APPLESS_MAIN(TestColorVariant)

#include "test_colorvariant.moc"
