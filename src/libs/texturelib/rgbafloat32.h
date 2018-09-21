#ifndef RGBAFLOAT32_H
#define RGBAFLOAT32_H

#include <TextureLib/Rgba32Signed>
#include <TextureLib/Rgba64Float>

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

class RgbaFloat32
{
public:
    constexpr RgbaFloat32() noexcept = default;
    constexpr RgbaFloat32(float red, float green, float blue, float alpha = 1.0f) noexcept
        : m_red(red)
        , m_green(green)
        , m_blue(blue)
        , m_alpha(alpha)
    {}

    constexpr float red()   const noexcept { return m_red;  }
    constexpr float green() const noexcept { return m_green; }
    constexpr float blue()  const noexcept { return m_blue;  }
    constexpr float alpha() const noexcept { return m_alpha; }

    constexpr void setRed  (float red)   noexcept { m_red   = red;   }
    constexpr void setGreen(float green) noexcept { m_green = green; }
    constexpr void setBlue (float blue)  noexcept { m_blue  = blue;  }
    constexpr void setAlpha(float alpha) noexcept { m_alpha = alpha; }

private:
    float m_red {0.0};
    float m_green {0.0};
    float m_blue {0.0};
    float m_alpha {0.0};
};

namespace Private {

inline float constexpr normalizeF32(qint8 value) noexcept { return 1.0f * value / 0x7f; }
inline float constexpr normalizeF32(quint8 value) noexcept { return 1.0f * value / 0xffu; }
inline float constexpr normalizeF32(quint16 value) noexcept { return 1.0f * value / 0xffffu; }
inline float constexpr bounded(float value) noexcept { return qBound(0.0f, value, 1.0f); }
inline float constexpr boundedSigned(float value) noexcept { return qBound(-1.0f, value, 1.0f); }

} // namespace Private

// qHelpers

inline constexpr float qRed(RgbaFloat32 color) noexcept { return color.red(); }
inline constexpr float qGreen(RgbaFloat32 color) noexcept { return color.green(); }
inline constexpr float qBlue(RgbaFloat32 color) noexcept { return color.blue(); }
inline constexpr float qAlpha(RgbaFloat32 color) noexcept { return color.alpha(); }

// constructors

inline constexpr RgbaFloat32 rgbaFloat32(float red, float green, float blue, float alpha = 1.0f) noexcept
{
    return {red, green, blue, alpha};
}

// QRgba

inline constexpr RgbaFloat32 rgbaFloat32(QRgb rgba) noexcept
{
    return rgbaFloat32(
            Private::normalizeF32(quint8(qRed(rgba))),
            Private::normalizeF32(quint8(qGreen(rgba))),
            Private::normalizeF32(quint8(qBlue(rgba))),
            Private::normalizeF32(quint8(qAlpha(rgba)))
    );
}

inline constexpr QRgb qRgba(RgbaFloat32 rgba) noexcept
{
    return qRgba(
            0xffu * Private::bounded(rgba.red()),
            0xffu * Private::bounded(rgba.green()),
            0xffu * Private::bounded(rgba.blue()),
            0xffu * Private::bounded(rgba.alpha()));
}

// Rgba32Signed

inline constexpr RgbaFloat32 rgbaFloat32(Rgba32Signed rgba) noexcept
{
    return {
            Private::normalizeF32(rgba.red()),
            Private::normalizeF32(rgba.green()),
            Private::normalizeF32(rgba.blue()),
            Private::normalizeF32(rgba.alpha())
    };
}

inline constexpr Rgba32Signed rgba32Signed(RgbaFloat32 rgba) noexcept
{
    // TODO (abbapoh): we do not cover -128 value for now, should be fixed
    return rgba32Signed(
            0x7f * Private::boundedSigned(rgba.red()),
            0x7f * Private::boundedSigned(rgba.green()),
            0x7f * Private::boundedSigned(rgba.blue()),
            0x7f * Private::boundedSigned(rgba.alpha())
    );
}

// QRgba64

inline constexpr RgbaFloat32 rgbaFloat32(QRgba64 rgba) noexcept
{
    return {
            Private::normalizeF32(quint16(qRed(rgba))),
            Private::normalizeF32(quint16(qGreen(rgba))),
            Private::normalizeF32(quint16(qBlue(rgba))),
            Private::normalizeF32(quint16(qAlpha(rgba)))
    };
}

inline constexpr QRgba64 qRgba64(RgbaFloat32 rgba) noexcept
{
    return qRgba64(
            0xffffu * rgba.red(),
            0xffffu * rgba.green(),
            0xffffu * rgba.blue(),
            0xffffu * rgba.alpha());
}

// RgbaFloat16

inline constexpr RgbaFloat32 rgbaFloat32(Rgba64Float rgba) noexcept
{
    return { qRed(rgba), qGreen(rgba), qBlue(rgba), qAlpha(rgba) };
}

inline constexpr Rgba64Float rgbaFloat16(RgbaFloat32 rgba) noexcept
{
    using Float = HalfFloat;
    return { Float(qRed(rgba)), Float(qGreen(rgba)), Float(qBlue(rgba)), Float(qAlpha(rgba)) };
}

#endif // RGBAFLOAT32_H
