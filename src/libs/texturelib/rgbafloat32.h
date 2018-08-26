#ifndef RGBAFLOAT32_H
#define RGBAFLOAT32_H

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

    constexpr QRgb toRgba32() const noexcept
    {
        return qRgba(
                0xffu * bounded(red()),
                0xffu * bounded(green()),
                0xffu * bounded(blue()),
                0xffu * bounded(alpha()));
    }

    static constexpr RgbaFloat32 fromRgba32(QRgb rgba) noexcept
    {
        return RgbaFloat32(
                normalize(quint8(qRed(rgba))),
                normalize(quint8(qGreen(rgba))),
                normalize(quint8(qBlue(rgba))),
                normalize(quint8(qAlpha(rgba))));
    }

    constexpr QRgba64 toRgba64() const noexcept
    {
        return qRgba64(0xffffu * red(), 0xffffu * green(), 0xffffu * blue(), 0xffffu * alpha());
    }

    static constexpr RgbaFloat32 fromRgba64(QRgba64 rgba) noexcept
    {
        return RgbaFloat32(
                normalize(quint16(qRed(rgba))),
                normalize(quint16(qGreen(rgba))),
                normalize(quint16(qBlue(rgba))),
                normalize(quint16(qAlpha(rgba))));
    }

private:
    static float constexpr normalize(quint8 value) noexcept { return 1.0f * value / 0xffu; }
    static float constexpr normalize(quint16 value) noexcept { return 1.0f * value / 0xffffu; }
    static float constexpr bounded(float value) noexcept { return qBound(0.0f, value, 1.0f); }

    float m_red {0.0};
    float m_green {0.0};
    float m_blue {0.0};
    float m_alpha {0.0};
};

#endif // RGBAFLOAT32_H
