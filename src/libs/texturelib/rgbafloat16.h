#ifndef RGBAFLOAT16_H
#define RGBAFLOAT16_H

#include <TextureLib/Rgba32Signed>

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

#include <HalfFloat>

class RgbaFloat16
{
public:
    constexpr RgbaFloat16() noexcept = default;
    constexpr RgbaFloat16(HalfFloat red, HalfFloat green, HalfFloat blue, HalfFloat alpha = HalfFloat(1.0)) noexcept
        : m_red(red)
        , m_green(green)
        , m_blue(blue)
        , m_alpha(alpha)
    {}

    constexpr HalfFloat red()   const noexcept { return m_red;  }
    constexpr HalfFloat green() const noexcept { return m_green; }
    constexpr HalfFloat blue()  const noexcept { return m_blue;  }
    constexpr HalfFloat alpha() const noexcept { return m_alpha; }

    constexpr void setRed  (HalfFloat red)   noexcept { m_red   = red;   }
    constexpr void setGreen(HalfFloat green) noexcept { m_green = green; }
    constexpr void setBlue (HalfFloat blue)  noexcept { m_blue  = blue;  }
    constexpr void setAlpha(HalfFloat alpha) noexcept { m_alpha = alpha; }

private:
    HalfFloat m_red;
    HalfFloat m_green;
    HalfFloat m_blue;
    HalfFloat m_alpha;
};

namespace Private {

inline HalfFloat constexpr normalizeF16(qint8 value) noexcept { return HalfFloat(1.0f * value / 0x7f); }
inline HalfFloat constexpr normalizeF16(quint8 value) noexcept { return HalfFloat(1.0f * value / 0xffu); }
inline HalfFloat constexpr normalizeF16(quint16 value) noexcept { return HalfFloat(1.0f * value / 0xffffu); }
inline HalfFloat constexpr bounded(HalfFloat value) noexcept { return qBound(HalfFloat(0.0f), value, HalfFloat(1.0f)); }
inline HalfFloat constexpr boundedSigned(HalfFloat value) noexcept { return qBound(HalfFloat(-1.0f), value, HalfFloat(1.0f)); }

} // namespace Private

// qHelpers

inline constexpr HalfFloat qRed(RgbaFloat16 color) noexcept { return color.red(); }
inline constexpr HalfFloat qGreen(RgbaFloat16 color) noexcept { return color.green(); }
inline constexpr HalfFloat qBlue(RgbaFloat16 color) noexcept { return color.blue(); }
inline constexpr HalfFloat qAlpha(RgbaFloat16 color) noexcept { return color.alpha(); }

// constructors

inline constexpr RgbaFloat16 rgbaFloat16(HalfFloat red, HalfFloat green, HalfFloat blue, HalfFloat alpha = HalfFloat(1.0)) noexcept
{
    return {red, green, blue, alpha};
}

// QRgba

inline constexpr RgbaFloat16 rgbaFloat16(QRgb rgba) noexcept
{
    return {
            Private::normalizeF16(quint8(qRed(rgba))),
            Private::normalizeF16(quint8(qGreen(rgba))),
            Private::normalizeF16(quint8(qBlue(rgba))),
            Private::normalizeF16(quint8(qAlpha(rgba)))
    };
}

inline constexpr QRgb qRgba(RgbaFloat16 rgba) noexcept
{
    return qRgba(
            0xffu * Private::bounded(rgba.red()),
            0xffu * Private::bounded(rgba.green()),
            0xffu * Private::bounded(rgba.blue()),
            0xffu * Private::bounded(rgba.alpha()));
}

// Rgba32Signed

inline constexpr RgbaFloat16 rgbaFloat16(Rgba32Signed rgba) noexcept
{
    return {
            Private::normalizeF16(rgba.red()),
            Private::normalizeF16(rgba.green()),
            Private::normalizeF16(rgba.blue()),
            Private::normalizeF16(rgba.alpha())
    };
}

inline constexpr Rgba32Signed rgba32Signed(RgbaFloat16 rgba) noexcept
{
    // TODO (abbapoh): we do not cover -128 value for now, should be fixed
    return {
            qint8(0x7f * Private::boundedSigned(rgba.red())),
            qint8(0x7f * Private::boundedSigned(rgba.green())),
            qint8(0x7f * Private::boundedSigned(rgba.blue())),
            qint8(0x7f * Private::boundedSigned(rgba.alpha()))
    };
}

// QRgba64

inline constexpr RgbaFloat16 rgbaFloat16(QRgba64 rgba) noexcept
{
    return {
            Private::normalizeF16(quint16(qRed(rgba))),
            Private::normalizeF16(quint16(qGreen(rgba))),
            Private::normalizeF16(quint16(qBlue(rgba))),
            Private::normalizeF16(quint16(qAlpha(rgba)))
    };
}

inline constexpr QRgba64 qRgba64(RgbaFloat16 rgba) noexcept
{
    return qRgba64(
            0xffffu * rgba.red(),
            0xffffu * rgba.green(),
            0xffffu * rgba.blue(),
            0xffffu * rgba.alpha()
    );
}

#endif // RGBAFLOAT16_H
