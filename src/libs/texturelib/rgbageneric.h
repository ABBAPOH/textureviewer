#ifndef RGBAGENERIC_H
#define RGBAGENERIC_H

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

#include <HalfFloat>

template<typename T>
struct RgbaLimits
{
    using DataType = T;
    static DataType max() { return std::numeric_limits<DataType>::max(); }
    static DataType min() { return std::numeric_limits<DataType>::min(); }
};

template<>
struct RgbaLimits<float>
{
    using DataType = float;
    static DataType max() { return +1.0f; }
    static DataType min() { return -1.0f; }
};

template<>
struct RgbaLimits<HalfFloat>
{
    using DataType = HalfFloat;
    static DataType max() { return HalfFloat(+1.0f); }
    static DataType min() { return HalfFloat(-1.0f); }
};

template<typename T>
class RgbaGeneric
{
public:
    using DataType = T;

    constexpr RgbaGeneric() noexcept = default;
    constexpr RgbaGeneric(
            DataType red,
            DataType green,
            DataType blue,
            DataType alpha = RgbaLimits<DataType>::max()) noexcept
        : m_red(red)
        , m_green(green)
        , m_blue(blue)
        , m_alpha(alpha)
    {}

    constexpr DataType red()   const noexcept { return m_red;  }
    constexpr DataType green() const noexcept { return m_green; }
    constexpr DataType blue()  const noexcept { return m_blue;  }
    constexpr DataType alpha() const noexcept { return m_alpha; }

    constexpr void setRed  (DataType red)   noexcept { m_red   = red;   }
    constexpr void setGreen(DataType green) noexcept { m_green = green; }
    constexpr void setBlue (DataType blue)  noexcept { m_blue  = blue;  }
    constexpr void setAlpha(DataType alpha) noexcept { m_alpha = alpha; }

private:
    DataType m_red;
    DataType m_green;
    DataType m_blue;
    DataType m_alpha;
};

#endif // RGBAGENERIC_H
