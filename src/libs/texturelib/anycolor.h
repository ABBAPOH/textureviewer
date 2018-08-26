#ifndef ANYCOLOR_H
#define ANYCOLOR_H

#include <QtGui/QRgb>
#include <QtGui/QRgba64>
#include <TextureLib/RgbaFloat32>

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
        RGBA32_Float,
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
    constexpr AnyColor(RgbaFloat32 rgba) noexcept
        : m_type(Type::RGBA32_Float)
        , m_rgbaFloat32(rgba)
    {}

    constexpr Type type() const noexcept { return m_type; }

    constexpr QRgb toRgba8_Unorm() const noexcept;
    constexpr QRgba64 toRgba16_Unorm() const noexcept;
    constexpr RgbaFloat32 toRgbaFloat32() const noexcept;

private:
    Type m_type {Type::Invalid};
    // TODO (abbapoh): use std::variant?
    union {
        quint64 m_zero {0};
//        quint32 rgba8_snorm;
        QRgb m_rgba8_Unorm;
//        quint64 rgba16_snorm;
        QRgba64 m_rgba16_Unorm;
        RgbaFloat32 m_rgbaFloat32;
    };
};

constexpr QRgb AnyColor::toRgba8_Unorm() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::RGBA8_Unorm: return m_rgba8_Unorm;
    case Type::RGBA16_Unorm: return m_rgba16_Unorm.toArgb32();
    case Type::RGBA32_Float: return qRgba(m_rgbaFloat32);
    default: return {};
    }
}

constexpr QRgba64 AnyColor::toRgba16_Unorm() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::RGBA8_Unorm: return QRgba64::fromArgb32(m_rgba8_Unorm);
    case Type::RGBA16_Unorm: return m_rgba16_Unorm;
    case Type::RGBA32_Float: return qRgba64(m_rgbaFloat32);
    default: return {};
    }
}

constexpr RgbaFloat32 AnyColor::toRgbaFloat32() const noexcept
{
    switch (m_type) {
    case Type::Invalid: return {};
    case Type::RGBA8_Unorm: return rgbaFloat32(m_rgba8_Unorm);
    case Type::RGBA16_Unorm: return rgbaFloat32(m_rgba16_Unorm);
    case Type::RGBA32_Float: return m_rgbaFloat32;
    default: return {};
    }
}

#endif // ANYCOLOR_H
