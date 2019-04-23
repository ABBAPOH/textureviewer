#ifndef TOSTRINGHELPERS_H
#define TOSTRINGHELPERS_H

#include <QtCore/QFlags>
#include <QtCore/QLatin1String>
#include <QtCore/QMetaEnum>

#include <OptionalType>

// latin1 helpers for enums

template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
QLatin1String enumToLatin1(E value)
{
    static const auto me = QMetaEnum::fromType<E>();
    Q_ASSERT(me.isValid());
    return QLatin1String(me.valueToKey(int(value)));
}

template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
Optional<E> enumFromLatin1(QLatin1String string)
{
    static const auto me = QMetaEnum::fromType<E>();
    Q_ASSERT(me.isValid());
    bool ok = false;
    const auto result = me.keyToValue(string.data(), &ok);
    return ok ? Optional<E>(E(result)) : std::nullopt;
}

// latin1 helpers for flags

template<
        typename F,
        typename E = typename F::enum_type,
        typename = std::enable_if_t<std::is_enum_v<E>>,
        typename = std::enable_if_t<std::is_same_v<F, QFlags<E>>>>
QByteArray flagsToLatin1(F value)
{
    static const auto me = QMetaEnum::fromType<F>();
    Q_ASSERT(me.isValid());
    return me.valueToKeys(int(value));
}

template<
        typename F,
        typename E = typename F::enum_type,
        typename = std::enable_if_t<std::is_enum_v<E>>,
        typename = std::enable_if_t<std::is_same_v<F, QFlags<E>>>>
Optional<F> flagsFromLatin1(QLatin1String string)
{
    static const auto me = QMetaEnum::fromType<F>();
    Q_ASSERT(me.isValid());
    bool ok = false;
    const auto result = me.keysToValue(string.data(), &ok);
    return ok ? Optional<F>(E(result)) : std::nullopt;
}

// integral types (int, uint...)

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
QString toQString(T value, int base = 10) = delete;

template<> inline QString toQString<short>(short value, int base)
{ return QString::number(value, base); }
template<> inline QString toQString<ushort>(ushort value, int base)
{ return QString::number(value, base); }
template<> inline QString toQString<int>(int value, int base)
{ return QString::number(value, base); }
template<> inline QString toQString<uint>(uint value, int base)
{ return QString::number(value, base); }
template<> inline QString toQString<long>(long value, int base)
{ return QString::number(value, base); }
template<> inline QString toQString<ulong>(ulong value, int base)
{ return QString::number(value, base); }
template<> inline QString toQString<qlonglong>(qlonglong value, int base)
{ return QString::number(value, base); }
template<> inline QString toQString<qulonglong>(qulonglong value, int base)
{ return QString::number(value, base); }

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
Optional<T> fromQString(const QString &string, int base = 10) = delete;

template<> inline Optional<short> fromQString<short>(const QString &string, int base)
{
    bool ok = false;
    const auto result = string.toShort(&ok, base);
    return ok ? Optional<short>(result) : nullOptional();
}

template<> inline Optional<ushort> fromQString<ushort>(const QString &string, int base)
{
    bool ok = false;
    const auto result = string.toUShort(&ok, base);
    return ok ? Optional<ushort>(result) : nullOptional();
}

template<> inline Optional<int> fromQString<int>(const QString &string, int base)
{
    bool ok = false;
    const auto result = string.toInt(&ok, base);
    return ok ? Optional<int>(result) : nullOptional();
}

template<> inline Optional<uint> fromQString<uint>(const QString &string, int base)
{
    bool ok = false;
    const auto result = string.toUInt(&ok, base);
    return ok ? Optional<uint>(result) : nullOptional();
}

template<> inline Optional<long> fromQString<long>(const QString &string, int base)
{
    bool ok = false;
    const auto result = string.toLong(&ok, base);
    return ok ? Optional<long>(result) : nullOptional();
}

template<> inline Optional<ulong> fromQString<ulong>(const QString &string, int base)
{
    bool ok = false;
    const auto result = string.toULong(&ok, base);
    return ok ? Optional<ulong>(result) : nullOptional();
}

template<> inline Optional<qlonglong> fromQString<qlonglong>(const QString &string, int base)
{
    bool ok = false;
    const auto result = string.toLongLong(&ok, base);
    return ok ? Optional<qlonglong>(result) : nullOptional();
}

template<> inline Optional<qulonglong> fromQString<qulonglong>(const QString &string, int base)
{
    bool ok = false;
    const auto result = string.toULongLong(&ok, base);
    return ok ? Optional<qulonglong>(result) : nullOptional();
}

// floating types (float, double)

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
QString toQString(T value, char format = 'g', int precision = 6) = delete;

template<> inline QString toQString<float>(float value, char format, int precision)
{ return QString::number(double(value), format, precision); }

template<> inline QString toQString<double>(double value, char format, int precision)
{ return QString::number(value, format, precision); }

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
Optional<T> fromQString(const QString &string) = delete;

template<> inline Optional<float> fromQString<float>(const QString &string)
{
    bool ok = false;
    const auto result = string.toFloat(&ok);
    return ok ? Optional<float>(result) : nullOptional();
}

template<> inline Optional<double> fromQString<double>(const QString &string)
{
    bool ok = false;
    const auto result = string.toDouble(&ok);
    return ok ? Optional<double>(result) : nullOptional();
}

// enum types

template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
QString toQString(E value)
{
    return QString(enumToLatin1(value));
}

template<typename E, typename = std::enable_if_t<std::is_enum_v<E>>>
Optional<E> fromQString(QStringView string)
{
    return enumFromLatin1<E>(QLatin1String(string.toLatin1()));
}

// flag types

template<
        typename F,
        typename E = typename F::enum_type,
        typename = std::enable_if_t<std::is_enum_v<E>>,
        typename = std::enable_if_t<std::is_same_v<F, QFlags<E>>>>
QString toQString(F value)
{
    return QString(flagsToLatin1(value));
}

template<
        typename F,
        typename E = typename F::enum_type,
        typename = std::enable_if_t<std::is_enum_v<E>>,
        typename = std::enable_if_t<std::is_same_v<F, QFlags<E>>>>
Optional<F> fromQString(QStringView string)
{
    return flagsFromLatin1<F>(QLatin1String(string.toLatin1()));
}

// generic type

template<
        typename T,
        typename = std::enable_if_t<std::is_same_v<decltype(std::declval<T>().toString()), QString>>>
QString toQString(const T &value)
{
    return value.toString();
}

#endif // TOSTRINGHELPERS_H
