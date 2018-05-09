#include <QtTest>

#include <TextureLib/TextureIOResult>

Q_DECLARE_METATYPE(TextureIOResult::Status)

class TestTextureIOResult : public QObject
{
    Q_OBJECT
private slots:
    void defaultConstruction();
    void construction_data();
    void construction();
    void copy();
    void assign();
};

void TestTextureIOResult::defaultConstruction()
{
    TextureIOResult result;
    QCOMPARE(result.status(), TextureIOResult::Status::Ok);
    QVERIFY(result.toBool());
    QVERIFY(bool(result));
    QCOMPARE(!result, false);
}

void TestTextureIOResult::construction_data()
{
    QTest::addColumn<TextureIOResult::Status>("status");
    QTest::addColumn<bool>("ok");

    QTest::newRow("Ok") << TextureIOResult::Status::Ok << true;
    QTest::newRow("InvalidMimeType") << TextureIOResult::Status::InvalidMimeType << false;
    QTest::newRow("FileNotFound") << TextureIOResult::Status::FileNotFound << false;
    QTest::newRow("DeviceError") << TextureIOResult::Status::DeviceError << false;
    QTest::newRow("UnsupportedMimeType") << TextureIOResult::Status::UnsupportedMimeType << false;
    QTest::newRow("IOError") << TextureIOResult::Status::HandlerError << false;
}

void TestTextureIOResult::construction()
{
    QFETCH(TextureIOResult::Status, status);
    QFETCH(bool, ok);

    TextureIOResult result(status);
    QCOMPARE(result.status(), status);
    QCOMPARE(result.toBool(), ok);
    QCOMPARE(bool(result), ok);
    QCOMPARE(!result, !ok);
}

void TestTextureIOResult::copy()
{
    TextureIOResult r1(TextureIOResult::Status::Ok);
    TextureIOResult r2(r1);

    QCOMPARE(r1.status(), r2.status());
    QCOMPARE(r1.toBool(), r2.toBool());
    QCOMPARE(bool(r1), bool(r2));
    QCOMPARE(!r1, !r2);
    QVERIFY(r1 == r2);
    QVERIFY(!(r1 != r2));
}

void TestTextureIOResult::assign()
{
    TextureIOResult r1(TextureIOResult::Status::Ok);
    TextureIOResult r2(TextureIOResult::Status::InvalidMimeType);

    QVERIFY(!(r1 == r2));
    QVERIFY(r1 != r2);

    r1 = r2;

    QCOMPARE(r1.status(), r2.status());
    QCOMPARE(r1.toBool(), r2.toBool());
    QCOMPARE(bool(r1), bool(r2));
    QCOMPARE(!r1, !r2);
    QVERIFY((r1 == r2));
    QVERIFY(!(r1 != r2));
}

QTEST_APPLESS_MAIN(TestTextureIOResult)

#include "test_textureioresult.moc"
