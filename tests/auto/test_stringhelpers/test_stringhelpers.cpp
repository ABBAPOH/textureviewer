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
    void enumeration_data();
    void enumeration();
    void flags_data();
    void flags();
};

Q_DECLARE_METATYPE(TestStringHelpers::MaybeEnum)
Q_DECLARE_METATYPE(TestStringHelpers::MaybeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(TestStringHelpers::Flags)

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

QTEST_APPLESS_MAIN(TestStringHelpers)

#include "test_stringhelpers.moc"
