#ifndef ANYCOLOR_H
#define ANYCOLOR_H

#include <TextureLib/Rgba32Signed>
#include <TextureLib/Rgba64Float>
#include <TextureLib/Rgba64Signed>
#include <TextureLib/Rgba128>

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

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

    constexpr Rgba32Signed  toRgba32Signed()    const noexcept;
    constexpr QRgb          toRgba32Unsigned()  const noexcept;
    constexpr Rgba64Float   toRgba64Float()     const noexcept;
    constexpr Rgba64Signed  toRgba64Signed()    const noexcept;
    constexpr QRgba64       toRgba64Unsigned()  const noexcept;
    constexpr Rgba128Float  toRgba128Float()    const noexcept;
    constexpr Rgba128Signed toRgba128Signed()   const noexcept;
    constexpr Rgba128       toRgba128Unsigned() const noexcept;

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
};

constexpr Rgba32Signed AnyColor::toRgba32Signed() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::Rgba32Signed: return m_rgba32Signed;
    case Type::Rgba32Unsigned: return rgba32Signed(m_rgba32Unsigned);

    case Type::Rgba64Float: return rgba32Signed(m_rgba64Float);
    case Type::Rgba64Signed: return rgba32Signed(m_rgba64Signed);
    case Type::Rgba64Unsigned: return rgba32Signed(m_rgba64Unsigned);

    case Type::Rgba128Float: return rgba32Signed(m_rgba128Float);
    case Type::Rgba128Signed: return rgba32Signed(m_rgba128Signed);
    case Type::Rgba128Unsigned: return rgba32Signed(m_rgba128Unsigned);
    }
}

constexpr QRgb AnyColor::toRgba32Unsigned() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::Rgba32Signed: return qRgba(m_rgba32Signed);
    case Type::Rgba32Unsigned: return m_rgba32Unsigned;

    case Type::Rgba64Float: return qRgba(m_rgba64Float);
    case Type::Rgba64Signed: return qRgba(m_rgba64Signed);
    case Type::Rgba64Unsigned: return m_rgba64Unsigned.toArgb32();

    case Type::Rgba128Float: return qRgba(m_rgba128Float);
    case Type::Rgba128Signed: return qRgba(m_rgba128Signed);
    case Type::Rgba128Unsigned: return qRgba(m_rgba128Unsigned);
    }
}

constexpr QRgba64 AnyColor::toRgba64Unsigned() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::Rgba32Signed: return qRgba64(m_rgba32Signed);
    case Type::Rgba32Unsigned: return QRgba64::fromArgb32(m_rgba32Unsigned);

    case Type::Rgba64Float: return qRgba64(m_rgba64Float);
    case Type::Rgba64Signed: return qRgba64(m_rgba64Signed);
    case Type::Rgba64Unsigned: return m_rgba64Unsigned;

    case Type::Rgba128Float: return qRgba64(m_rgba128Float);
    case Type::Rgba128Signed: return qRgba64(m_rgba128Signed);
    case Type::Rgba128Unsigned: return qRgba64(m_rgba128Unsigned);
    }
}

constexpr Rgba64Float AnyColor::toRgba64Float() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::Rgba32Signed: return rgba64Float(m_rgba32Signed);
    case Type::Rgba32Unsigned: return rgba64Float(m_rgba32Unsigned);

    case Type::Rgba64Float: return m_rgba64Float;
    case Type::Rgba64Signed: return rgba64Float(m_rgba64Signed);
    case Type::Rgba64Unsigned: return rgba64Float(m_rgba64Unsigned);

    case Type::Rgba128Float: return rgba64Float(m_rgba128Float);
    case Type::Rgba128Signed: return rgba64Float(m_rgba128Signed);
    case Type::Rgba128Unsigned: return rgba64Float(m_rgba128Unsigned);
    }
}

constexpr Rgba64Signed AnyColor::toRgba64Signed() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::Rgba32Signed: return rgba64Signed(m_rgba32Signed);
    case Type::Rgba32Unsigned: return rgba64Signed(m_rgba32Unsigned);

    case Type::Rgba64Float: return rgba64Signed(m_rgba64Float);
    case Type::Rgba64Signed: return m_rgba64Signed;
    case Type::Rgba64Unsigned: return rgba64Signed(m_rgba64Unsigned);

    case Type::Rgba128Float: return rgba64Signed(m_rgba128Float);
    case Type::Rgba128Signed: return rgba64Signed(m_rgba128Signed);
    case Type::Rgba128Unsigned: return rgba64Signed(m_rgba128Unsigned);
    }
}

constexpr Rgba128Float AnyColor::toRgba128Float() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::Rgba32Signed: return rgba128Float(m_rgba32Signed);
    case Type::Rgba32Unsigned: return rgba128Float(m_rgba32Unsigned);

    case Type::Rgba64Float: return rgba128Float(m_rgba64Float);
    case Type::Rgba64Signed: return rgba128Float(m_rgba64Signed);
    case Type::Rgba64Unsigned: return rgba128Float(m_rgba64Unsigned);

    case Type::Rgba128Float: return m_rgba128Float;
    case Type::Rgba128Signed: return rgba128Float(m_rgba128Signed);
    case Type::Rgba128Unsigned: return rgba128Float(m_rgba128Unsigned);
    }
}

constexpr Rgba128Signed AnyColor::toRgba128Signed() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::Rgba32Signed: return rgba128Signed(m_rgba32Signed);
    case Type::Rgba32Unsigned: return rgba128Signed(m_rgba32Unsigned);

    case Type::Rgba64Float: return rgba128Signed(m_rgba64Float);
    case Type::Rgba64Signed: return rgba128Signed(m_rgba64Signed);
    case Type::Rgba64Unsigned: return rgba128Signed(m_rgba64Unsigned);

    case Type::Rgba128Float: return rgba128Signed(m_rgba128Float);
    case Type::Rgba128Signed: return m_rgba128Signed;
    case Type::Rgba128Unsigned: return rgba128Signed(m_rgba128Unsigned);
    }
}

constexpr Rgba128 AnyColor::toRgba128Unsigned() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::Rgba32Signed: return rgba128(m_rgba32Signed);
    case Type::Rgba32Unsigned: return rgba128(m_rgba32Unsigned);

    case Type::Rgba64Float: return rgba128(m_rgba64Float);
    case Type::Rgba64Signed: return rgba128(m_rgba64Signed);
    case Type::Rgba64Unsigned: return rgba128(m_rgba64Unsigned);

    case Type::Rgba128Float: return rgba128(m_rgba128Float);
    case Type::Rgba128Signed: return rgba128(m_rgba128Signed);
    case Type::Rgba128Unsigned: return m_rgba128Unsigned;
    }
}

// helpers

constexpr inline Rgba32Signed rgba32Signed(const AnyColor &color) noexcept { return color.toRgba32Signed(); }
constexpr inline QRgb qRgba(const AnyColor &color) noexcept { return color.toRgba32Unsigned(); }
constexpr inline Rgba64Float rgba64Float(const AnyColor &color) noexcept { return color.toRgba64Float(); }
constexpr inline Rgba64Signed rgba64Signed(const AnyColor &color) noexcept { return color.toRgba64Signed(); }
constexpr inline QRgba64 qRgba64(const AnyColor &color) noexcept { return color.toRgba64Unsigned(); }
constexpr inline Rgba128Float rgba128Float(const AnyColor &color) noexcept { return color.toRgba128Float(); }

#endif // ANYCOLOR_H
