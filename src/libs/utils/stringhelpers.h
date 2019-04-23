#ifndef TOSTRINGHELPERS_H
#define TOSTRINGHELPERS_H

#include <QtCore/QFlags>
#include <QtCore/QLatin1String>
#include <QtCore/QMetaEnum>

#include <OptionalType>

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

#endif // TOSTRINGHELPERS_H
