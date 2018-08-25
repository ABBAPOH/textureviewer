#ifndef ANYCOLOR_H
#define ANYCOLOR_H

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

class AnyColor
{
public:
    enum class Type {
        Invalid = 0,
//        RGBA8_Snorm,
        RGBA8_Unorm,
//        RGBA16_Float,
//        RGBA16_Snorm,
        RGBA16_Unorm,
//        RGBA32_Float,
    };

    constexpr AnyColor() noexcept
        : m_type(Type::Invalid)
        , m_zero(0)
    {}
    constexpr AnyColor(QRgb rgba) noexcept
        : m_type(Type::RGBA8_Unorm)
        , m_rgba8_Unorm(rgba)
    {}
    constexpr AnyColor(QRgba64 rgba) noexcept
        : m_type(Type::RGBA16_Unorm)
        , m_rgba16_Unorm(rgba)
    {}

    constexpr Type type() const noexcept { return m_type; }

    constexpr QRgb toRgb32_Unorm() const noexcept;
    constexpr QRgba64 toRgb64_Unorm() const noexcept;

private:
    Type m_type {Type::Invalid};
    // TODO (abbapoh): use std::variant?
    union {
        quint64 m_zero {0};
//        quint32 rgba8_snorm;
        QRgb m_rgba8_Unorm;
//        quint64 rgba16_snorm;
        QRgba64 m_rgba16_Unorm;
    };
};

constexpr QRgb AnyColor::toRgb32_Unorm() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::RGBA8_Unorm: return m_rgba8_Unorm;
    case Type::RGBA16_Unorm: return m_rgba16_Unorm.toArgb32();
    default: return {};
    }
}

constexpr QRgba64 AnyColor::toRgb64_Unorm() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::RGBA8_Unorm: return QRgba64::fromArgb32(m_rgba8_Unorm);
    case Type::RGBA16_Unorm: return m_rgba16_Unorm;
    default: return {};
    }
}

#endif // ANYCOLOR_H
