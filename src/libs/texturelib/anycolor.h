#ifndef ANYCOLOR_H
#define ANYCOLOR_H

#include <TextureLib/Rgba32Signed>
#include <TextureLib/Rgba64Float>
#include <TextureLib/Rgba128Float>

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

class AnyColor
{
public:
    enum class Type {
        Invalid = 0,
        RGBA8_Snorm,
        RGBA8_Unorm,
//        RGBA16_Snorm,
        RGBA16_Unorm,
        RGBA16_Float,
        RGBA32_Float,
    };

    constexpr AnyColor() noexcept
        : m_type(Type::Invalid)
        , m_zero(0)
    {}
    constexpr AnyColor(Rgba32Signed rgba) noexcept
        : m_type(Type::RGBA8_Snorm)
        , m_rgba8_Snorm(rgba)
    {}
    constexpr AnyColor(QRgb rgba) noexcept
        : m_type(Type::RGBA8_Unorm)
        , m_rgba8_Unorm(rgba)
    {}
    constexpr AnyColor(Rgba64Float rgba) noexcept
        : m_type(Type::RGBA16_Float)
        , m_rgbaFloat16(rgba)
    {}
    constexpr AnyColor(QRgba64 rgba) noexcept
        : m_type(Type::RGBA16_Unorm)
        , m_rgba16_Unorm(rgba)
    {}
    constexpr AnyColor(Rgba128Float rgba) noexcept
        : m_type(Type::RGBA32_Float)
        , m_rgbaFloat32(rgba)
    {}

    constexpr Type type() const noexcept { return m_type; }

    constexpr Rgba32Signed toRgba8_Snorm() const noexcept;
    constexpr QRgb toRgba8_Unorm() const noexcept;
    constexpr QRgba64 toRgba16_Unorm() const noexcept;
    constexpr Rgba64Float toRgbaFloat16() const noexcept;
    constexpr Rgba128Float toRgbaFloat32() const noexcept;

private:
    Type m_type {Type::Invalid};
    // TODO (abbapoh): use std::variant?
    union {
        quint64 m_zero {0};
        Rgba32Signed m_rgba8_Snorm;
        QRgb m_rgba8_Unorm;
//        quint64 rgba16_snorm;
        QRgba64 m_rgba16_Unorm;
        Rgba64Float m_rgbaFloat16;
        Rgba128Float m_rgbaFloat32;
    };
};

constexpr Rgba32Signed AnyColor::toRgba8_Snorm() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::RGBA8_Snorm: return m_rgba8_Snorm;
    case Type::RGBA8_Unorm: return rgba32Signed(m_rgba8_Unorm);
    case Type::RGBA16_Unorm: return rgba32Signed(m_rgba16_Unorm);
    case Type::RGBA16_Float: return rgba32Signed(m_rgbaFloat16);
    case Type::RGBA32_Float: return rgba32Signed(m_rgbaFloat32);
    default: return {};
    }
}

constexpr QRgb AnyColor::toRgba8_Unorm() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::RGBA8_Snorm: return qRgba(m_rgba8_Snorm);
    case Type::RGBA8_Unorm: return m_rgba8_Unorm;
    case Type::RGBA16_Unorm: return m_rgba16_Unorm.toArgb32();
    case Type::RGBA16_Float: return qRgba(m_rgbaFloat16);
    case Type::RGBA32_Float: return qRgba(m_rgbaFloat32);
    default: return {};
    }
}

constexpr QRgba64 AnyColor::toRgba16_Unorm() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::RGBA8_Snorm: return qRgba64(m_rgba8_Snorm);
    case Type::RGBA8_Unorm: return QRgba64::fromArgb32(m_rgba8_Unorm);
    case Type::RGBA16_Unorm: return m_rgba16_Unorm;
    case Type::RGBA16_Float: return qRgba64(m_rgbaFloat16);
    case Type::RGBA32_Float: return qRgba64(m_rgbaFloat32);
    default: return {};
    }
}

constexpr Rgba64Float AnyColor::toRgbaFloat16() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::RGBA8_Snorm: return rgba64Float(m_rgba8_Snorm);
    case Type::RGBA8_Unorm: return rgba64Float(m_rgba8_Unorm);
    case Type::RGBA16_Unorm: return rgba64Float(m_rgba16_Unorm);
    case Type::RGBA16_Float: return m_rgbaFloat16;
    case Type::RGBA32_Float: return rgba64Float(m_rgbaFloat32);
    default: return {};
    }
}
constexpr Rgba128Float AnyColor::toRgbaFloat32() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::RGBA8_Snorm: return rgba128Float(m_rgba8_Snorm);
    case Type::RGBA8_Unorm: return rgba128Float(m_rgba8_Unorm);
    case Type::RGBA16_Unorm: return rgba128Float(m_rgba16_Unorm);
    case Type::RGBA32_Float: return m_rgbaFloat32;
    default: return {};
    }
}

// qHelpers

constexpr inline QRgb qRgba(const AnyColor &color) noexcept { return color.toRgba8_Unorm(); }
constexpr inline QRgba64 qRgba64(const AnyColor &color) noexcept { return color.toRgba16_Unorm(); }
constexpr inline Rgba32Signed rgba32Signed(const AnyColor &color) noexcept { return color.toRgba8_Snorm(); }
constexpr inline Rgba64Float rgba64Float(const AnyColor &color) noexcept { return color.toRgbaFloat16(); }
constexpr inline Rgba128Float rgba128Float(const AnyColor &color) noexcept { return color.toRgbaFloat32(); }

#endif // ANYCOLOR_H
