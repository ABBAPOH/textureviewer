#include <QtTest>

#include <TextureLib/Rgba64Float>

Q_DECLARE_METATYPE(HalfFloat)

class TestRgbaGeneric : public QObject
{
    Q_OBJECT
private slots:
    void testNormalize();
    void defaultConstruction();
    void api_data();
    void api();
    void copy();
    void assign();
    void equals();
};

void TestRgbaGeneric::testNormalize()
{
    // quint8 -> float
    QCOMPARE((Private::normalize<float, quint8>(0)), 0.0f);
    QCOMPARE((Private::normalize<float, quint8>(127)), 127.0f / 255.0f);
    QCOMPARE((Private::normalize<float, quint8>(0xff)), 1.0f);

    // float -> quint8
    QCOMPARE((Private::normalize<quint8, float>(0.0f)), 0);
    QCOMPARE((Private::normalize<quint8, float>(0.497f)), 127);
    QCOMPARE((Private::normalize<quint8, float>(1.0f)), 0xff);

    // quint16 -> float
    QCOMPARE((Private::normalize<float, quint16>(0)), 0.0f);
    QCOMPARE((Private::normalize<float, quint16>(0x7fff)), 1.0f * 0x7fff / 0xffff);
    QCOMPARE((Private::normalize<float, quint16>(0xffff)), 1.0f);

    // float -> quint16
    QCOMPARE((Private::normalize<quint16, float>(0.0f)), 0);
    QCOMPARE((Private::normalize<quint16, float>(1.0f * 0x7fff / 0xffff)), 0x7fff);
    QCOMPARE((Private::normalize<quint16, float>(1.0f)), 0xffff);

    // quint8 -> HalfFloat
    QCOMPARE((Private::normalize<HalfFloat, quint8>(0)), HalfFloat(0.0f));
    QCOMPARE((Private::normalize<HalfFloat, quint8>(127)), HalfFloat(127.0f / 255.0f));
    QCOMPARE((Private::normalize<HalfFloat, quint8>(0xff)), HalfFloat(1.0f));

    // float -> quint8
    QCOMPARE((Private::normalize<quint8, HalfFloat>(HalfFloat(0.0f))), 0);
    QCOMPARE((Private::normalize<quint8, HalfFloat>(HalfFloat(0.497f))), 127);
    QCOMPARE((Private::normalize<quint8, HalfFloat>(HalfFloat(1.0f))), 0xff);

    // float -> HalfFloat
    QCOMPARE((Private::normalize<HalfFloat, float>(0.0f)), HalfFloat(0));
    QCOMPARE((Private::normalize<HalfFloat, float>(0.5f)), HalfFloat(0.5f));
    QCOMPARE((Private::normalize<HalfFloat, float>(1.0f)), HalfFloat(1.0f));

    // HalfFloat -> float
    QCOMPARE((Private::normalize<float, HalfFloat>(HalfFloat(0.0f))), 0.0f);
    QCOMPARE((Private::normalize<float, HalfFloat>(HalfFloat(0.5f))), 0.5f);
    QCOMPARE((Private::normalize<float, HalfFloat>(HalfFloat(1.0f))), 1.0f);

    // quint8 -> quint16
    QCOMPARE((Private::normalize<quint16, quint8>(0)), 0);
    // TODO (abbapoh): fix conversion
    QCOMPARE((Private::normalize<quint16, quint8>(0x7f)), 0x7F7F);
    QCOMPARE((Private::normalize<quint16, quint8>(0xff)), 0xffff);
}

void TestRgbaGeneric::defaultConstruction()
{
    RgbaGeneric<float> c;
    QCOMPARE(c.red(), 0.0f);
    QCOMPARE(c.green(), 0.0f);
    QCOMPARE(c.blue(), 0.0f);
    QCOMPARE(c.alpha(), 0.0f);
}

void TestRgbaGeneric::api_data()
{
    QTest::addColumn<float>("red");
    QTest::addColumn<float>("green");
    QTest::addColumn<float>("blue");
    QTest::addColumn<float>("alpha");

    QTest::newRow("black") << 0.0f << 0.0f << 0.0f << 0.0f;
    QTest::newRow("red") << float(0) << 0.0f << 0.0f << float(0);
    QTest::newRow("red green") << 1.0f << 1.0f << 0.0f << 1.0f;
    QTest::newRow("white") << 1.0f << 1.0f << 1.0f << float(0);
    QTest::newRow("negative red") << -1.0f << 0.0f << 0.0f << -1.0f;
    QTest::newRow("negative white") << -1.0f << -1.0f << -1.0f << -1.0f;
}

void TestRgbaGeneric::api()
{
    QFETCH(float, red);
    QFETCH(float, green);
    QFETCH(float, blue);
    QFETCH(float, alpha);

    // construction
    RgbaGeneric<float> c(red, green, blue, alpha);

    QCOMPARE(c.red(), red);
    QCOMPARE(c.green(), green);
    QCOMPARE(c.blue(), blue);
    QCOMPARE(c.alpha(), alpha);

    // qHelpers

    QCOMPARE(getRed(c), red);
    QCOMPARE(getGreen(c), green);
    QCOMPARE(getBlue(c), blue);
    QCOMPARE(getAlpha(c), alpha);

    // setters
    RgbaGeneric<float> c2;

    c2.setRed(red);
    c2.setGreen(green);
    c2.setBlue(blue);
    c2.setAlpha(alpha);

    QCOMPARE(c2.red(), red);
    QCOMPARE(c2.green(), green);
    QCOMPARE(c2.blue(), blue);
    QCOMPARE(c2.alpha(), alpha);
}

void TestRgbaGeneric::copy()
{
    RgbaGeneric<float> c1(1.0f, 0.75f, 0.5f, 0.25f);
    RgbaGeneric<float> c2(c1);

    QCOMPARE(c2.red(), c1.red());
    QCOMPARE(c2.green(), c1.green());
    QCOMPARE(c2.blue(), c1.blue());
    QCOMPARE(c2.alpha(), c1.alpha());
}

void TestRgbaGeneric::assign()
{
    RgbaGeneric<float> c1(HalfFloat(1.0f), 1.0f, 1.0f, 1.0f);
    RgbaGeneric<float> c2;

    c2 = c1;

    QCOMPARE(c2.red(), c1.red());
    QCOMPARE(c2.green(), c1.green());
    QCOMPARE(c2.blue(), c1.blue());
    QCOMPARE(c2.alpha(), c1.alpha());
}

void TestRgbaGeneric::equals()
{
    RgbaGeneric<float> c1(HalfFloat(1.0f), 1.0f, 1.0f, 1.0f);
    RgbaGeneric<float> c2(HalfFloat(1.0f), 1.0f, 1.0f, 1.0f);
    RgbaGeneric<float> c3(HalfFloat(0.5f), 0.5f, 0.5f, 0.5f);

    QVERIFY(c1 == c2);
    QVERIFY(c1 != c3);
    QVERIFY(c2 != c3);
}

QTEST_APPLESS_MAIN(TestRgbaGeneric)

#include "test_rgbageneric.moc"
