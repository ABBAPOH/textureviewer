#include <QtTest>

#include <UtilsLib/StringHelpers>

class TestStringHelpers : public QObject
{
    Q_OBJECT
public:
    enum class Enum {
        Zero = 0,
        One = 1,
        Two = 2
    };
    Q_ENUM(Enum)
    using MaybeEnum = Optional<Enum>;

    enum class Flag {
        Zero = 0,
        One = 1,
        Two = 2
    };
    Q_DECLARE_FLAGS(Flags, Flag)
    Q_FLAG(Flags)
    using MaybeFlags = Optional<Flags>;

private slots:
    void integral();
    void floating();
    void enumeration_data();
    void enumeration();
    void flags_data();
    void flags();
    void datetime();
};

Q_DECLARE_METATYPE(TestStringHelpers::MaybeEnum)
Q_DECLARE_METATYPE(TestStringHelpers::MaybeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(TestStringHelpers::Flags)

void TestStringHelpers::integral()
{
    QCOMPARE(toQString(-10), QStringLiteral("-10"));
    QCOMPARE(toQString(10, 16), QStringLiteral("a"));

    QCOMPARE(*fromQString<int>(QStringLiteral("-10")), -10);
    QCOMPARE(*fromQString<int>(QStringLiteral("-a"), 16), -10);
    QVERIFY(!fromQString<int>(QStringLiteral("invalid")));

    QCOMPARE(toQString(10u), QStringLiteral("10"));
    QCOMPARE(toQString(10u, 16), QStringLiteral("a"));

    QCOMPARE(*fromQString<uint>(QStringLiteral("10")), 10u);
    QCOMPARE(*fromQString<uint>(QStringLiteral("a"), 16), 10u);
    QVERIFY(!fromQString<uint>(QStringLiteral("invalid")));

    QCOMPARE(toQString(-0x1000000000000ll), QStringLiteral("-281474976710656"));
    QCOMPARE(toQString(0x1000000000000ll, 16), QStringLiteral("1000000000000"));

    QCOMPARE(*fromQString<qlonglong>(QStringLiteral("-281474976710656")), -0x1000000000000ll);
    QCOMPARE(*fromQString<qlonglong>(QStringLiteral("-1000000000000"), 16), -0x1000000000000ll);
    QVERIFY(!fromQString<qlonglong>(QStringLiteral("invalid")));
}

void TestStringHelpers::floating()
{
    QCOMPARE(toQString(-10.5f), QStringLiteral("-10.5"));
    QCOMPARE(*fromQString<float>(QStringLiteral("10.5")), 10.5f);

    QCOMPARE(toQString(-10.5), QStringLiteral("-10.5"));
    QCOMPARE(*fromQString<double>(QStringLiteral("10.5")), 10.5);
}

void TestStringHelpers::enumeration_data()
{
    QTest::addColumn<MaybeEnum>("value");
    QTest::addColumn<QString>("string");

    QTest::newRow("invalid") << MaybeEnum(std::nullopt) << QStringLiteral("Invalid");
    QTest::newRow("zero") << MaybeEnum(Enum::Zero) << QStringLiteral("Zero");
    QTest::newRow("one") << MaybeEnum(Enum::One) << QStringLiteral("One");
    QTest::newRow("two") << MaybeEnum(Enum::Two) << QStringLiteral("Two");
}

void TestStringHelpers::enumeration()
{
    QFETCH(MaybeEnum, value);
    QFETCH(QString, string);

    if (value)
        QCOMPARE(::toQString(*value), string);

    const auto opt = ::fromQString<Enum>(string);
    QCOMPARE(opt, value);
}

void TestStringHelpers::flags_data()
{
    QTest::addColumn<MaybeFlags>("value");
    QTest::addColumn<QString>("string");

    QTest::newRow("invalid") << MaybeFlags(std::nullopt) << QStringLiteral("Invalid");
    QTest::newRow("zero") << MaybeFlags(Flag::Zero) << QStringLiteral("Zero");
    QTest::newRow("one") << MaybeFlags(Flag::One) << QStringLiteral("One");
    QTest::newRow("two") << MaybeFlags(Flag::Two) << QStringLiteral("Two");
    QTest::newRow("three") << MaybeFlags(Flag::One | Flag::Two) << QStringLiteral("One|Two");
}

void TestStringHelpers::flags()
{
    QFETCH(MaybeFlags, value);
    QFETCH(QString, string);

    if (value)
        QCOMPARE(::toQString(*value), string);

    const auto opt = ::fromQString<Flags>(string);
    QCOMPARE(opt, value);
}

void TestStringHelpers::datetime()
{
    const auto dt = QDateTime::currentDateTime();
    QCOMPARE(::toQString(dt), dt.toString());
}

QTEST_APPLESS_MAIN(TestStringHelpers)

#include "test_stringhelpers.moc"
