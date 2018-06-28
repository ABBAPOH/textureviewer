#include <QtTest>

#include <TextureLib/TextureIOResult>

Q_DECLARE_METATYPE(TextureIOError)

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
    QVERIFY(bool(result));
    QCOMPARE(!result, false);
}

void TestTextureIOResult::construction_data()
{
    QTest::addColumn<TextureIOError>("status");
    QTest::addColumn<bool>("ok");

    QTest::newRow("InvalidMimeType") << TextureIOError::InvalidMimeType << false;
    QTest::newRow("FileNotFound") << TextureIOError::FileNotFound << false;
    QTest::newRow("DeviceError") << TextureIOError::DeviceError << false;
    QTest::newRow("UnsupportedMimeType") << TextureIOError::UnsupportedMimeType << false;
    QTest::newRow("IOError") << TextureIOError::HandlerError << false;
}

void TestTextureIOResult::construction()
{
    QFETCH(TextureIOError, status);
    QFETCH(bool, ok);

    TextureIOResult result(status);
    QCOMPARE(result.error(), status);
    QCOMPARE(bool(result), ok);
    QCOMPARE(!result, !ok);
}

void TestTextureIOResult::copy()
{
    TextureIOResult r1(TextureIOError::DeviceError);
    TextureIOResult r2(r1);

    QCOMPARE(r1.error(), r2.error());
    QCOMPARE(bool(r1), bool(r2));
    QCOMPARE(!r1, !r2);
    QVERIFY(r1 == r2);
    QVERIFY(!(r1 != r2));
}

void TestTextureIOResult::assign()
{
    TextureIOResult r1(TextureIOError::DeviceError);
    TextureIOResult r2(TextureIOError::InvalidMimeType);

    QVERIFY(!(r1 == r2));
    QVERIFY(r1 != r2);

    r1 = r2;

    QCOMPARE(r1.error(), r2.error());
    QCOMPARE(bool(r1), bool(r2));
    QCOMPARE(!r1, !r2);
    QVERIFY((r1 == r2));
    QVERIFY(!(r1 != r2));
}

QTEST_APPLESS_MAIN(TestTextureIOResult)

#include "test_textureioresult.moc"
