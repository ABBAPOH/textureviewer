#ifndef ANYCOLOR_H
#define ANYCOLOR_H

#include <TextureLib/RgbaTypes>

class AnyColor
{
public:
    enum class Type {
        Invalid = 0,
        Rgba32Signed,
        Rgba32Unsigned,
        Rgba32 = Rgba32Unsigned,
        Rgba64Float,
        Rgba64Signed,
        Rgba64Unsigned,
        Rgba64 = Rgba64Unsigned,
        Rgba128Float,
        Rgba128Signed,
        Rgba128Unsigned,
        Rgba128 = Rgba128Unsigned,
    };

    constexpr AnyColor() noexcept
        : m_zero(0)
        , m_type(Type::Invalid)
    {}
    constexpr AnyColor(Rgba32Signed rgba) noexcept
        : m_rgba32Signed(rgba)
        , m_type(Type::Rgba32Signed)
    {}
    constexpr AnyColor(QRgb rgba) noexcept
        : m_rgba32Unsigned(rgba)
        , m_type(Type::Rgba32Unsigned)
    {}
    constexpr AnyColor(Rgba64Float rgba) noexcept
        : m_rgba64Float(rgba)
        , m_type(Type::Rgba64Float)
    {}
    constexpr AnyColor(Rgba64Signed rgba) noexcept
        : m_rgba64Signed(rgba)
        , m_type(Type::Rgba64Signed)
    {}
    constexpr AnyColor(QRgba64 rgba) noexcept
        : m_rgba64Unsigned(rgba)
        , m_type(Type::Rgba64Unsigned)
    {}
    constexpr AnyColor(Rgba128Float rgba) noexcept
        : m_rgba128Float(rgba)
        , m_type(Type::Rgba128Float)
    {}
    constexpr AnyColor(Rgba128Signed rgba) noexcept
        : m_rgba128Signed(rgba)
        , m_type(Type::Rgba128Signed)
    {}
    constexpr AnyColor(Rgba128 rgba) noexcept
        : m_rgba128Unsigned(rgba)
        , m_type(Type::Rgba128Unsigned)
    {}

    constexpr Type type() const noexcept { return m_type; }
    constexpr bool isEmpty() const noexcept { return m_type == Type::Invalid; }

    template<typename T>
    constexpr T convert() const noexcept;

private:
    // TODO (abbapoh): use std::variant?
    union {
        quint64 m_zero {0};
        Rgba32Signed  m_rgba32Signed;
        QRgb          m_rgba32Unsigned;
        Rgba64Float   m_rgba64Float;
        Rgba64Signed  m_rgba64Signed;
        QRgba64       m_rgba64Unsigned;
        Rgba128Float  m_rgba128Float;
        Rgba128Signed m_rgba128Signed;
        Rgba128       m_rgba128Unsigned;
    };
    Type m_type {Type::Invalid};

    friend constexpr bool operator==(const AnyColor &lhs, const AnyColor &rhs);
};

template<typename T>
constexpr T AnyColor::convert() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return T();
    case Type::Rgba32Signed: return Private::convertRgba<T>(m_rgba32Signed);
    case Type::Rgba32Unsigned: return Private::convertRgba<T>(m_rgba32Unsigned);

    case Type::Rgba64Float: return Private::convertRgba<T>(m_rgba64Float);
    case Type::Rgba64Signed: return Private::convertRgba<T>(m_rgba64Signed);
    case Type::Rgba64Unsigned: return Private::convertRgba<T>(m_rgba64Unsigned);

    case Type::Rgba128Float: return Private::convertRgba<T>(m_rgba128Float);
    case Type::Rgba128Signed: return Private::convertRgba<T>(m_rgba128Signed);
    case Type::Rgba128Unsigned: return Private::convertRgba<T>(m_rgba128Unsigned);
    }
    return {};
}

constexpr bool operator==(const AnyColor &lhs, const AnyColor &rhs)
{
    if (lhs.type() != rhs.type())
        return false;
    switch (lhs.type()) {
    case AnyColor::Type::Invalid:
        return true;
    case AnyColor::Type::Rgba32Signed:
        return quint32(lhs.m_rgba32Signed) == quint32(rhs.m_rgba32Signed);
    case AnyColor::Type::Rgba32Unsigned:
        return lhs.m_rgba32Unsigned == rhs.m_rgba32Unsigned;

    case AnyColor::Type::Rgba64Float:
        return lhs.m_rgba64Float == rhs.m_rgba64Float;
    case AnyColor::Type::Rgba64Signed:
        return quint64(lhs.m_rgba64Signed) == quint64(rhs.m_rgba64Signed);
    case AnyColor::Type::Rgba64Unsigned:
        return quint64(lhs.m_rgba64Unsigned) == quint64(rhs.m_rgba64Unsigned);

    case AnyColor::Type::Rgba128Float:
        return lhs.m_rgba128Float == rhs.m_rgba128Float;
    case AnyColor::Type::Rgba128Signed:
        return lhs.m_rgba128Signed == rhs.m_rgba128Signed;
    case AnyColor::Type::Rgba128Unsigned:
        return lhs.m_rgba128Unsigned == rhs.m_rgba128Unsigned;
    }
}

constexpr bool operator!=(const AnyColor &lhs, const AnyColor &rhs)
{
    return !(lhs == rhs);
}

// helpers

constexpr inline Rgba32Signed rgba32Signed(const AnyColor &color) noexcept
{ return color.convert<Rgba32Signed>(); }
constexpr inline QRgb qRgba(const AnyColor &color) noexcept
{ return color.convert<QRgb>(); }
constexpr inline Rgba64Float rgba64Float(const AnyColor &color) noexcept
{ return color.convert<Rgba64Float>(); }
constexpr inline Rgba64Signed rgba64Signed(const AnyColor &color) noexcept
{ return color.convert<Rgba64Signed>(); }
constexpr inline QRgba64 qRgba64(const AnyColor &color) noexcept
{ return color.convert<QRgba64>(); }
constexpr inline Rgba128Float rgba128Float(const AnyColor &color) noexcept
{ return color.convert<Rgba128Float>(); }

#endif // ANYCOLOR_H
