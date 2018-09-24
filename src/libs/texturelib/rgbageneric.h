#ifndef RGBAGENERIC_H
#define RGBAGENERIC_H

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

    constexpr RgbaGeneric() noexcept {}
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
    DataType m_red {0};
    DataType m_green {0};
    DataType m_blue {0};
    DataType m_alpha {0};
};

// qHelpers

template<typename T> constexpr T qRed  (RgbaGeneric<T> color) noexcept { return color.red();   }
template<typename T> constexpr T qGreen(RgbaGeneric<T> color) noexcept { return color.green(); }
template<typename T> constexpr T qBlue (RgbaGeneric<T> color) noexcept { return color.blue();  }
template<typename T> constexpr T qAlpha(RgbaGeneric<T> color) noexcept { return color.alpha(); }

template<typename T> constexpr RgbaGeneric<T> createRgba(T r, T g, T b, T a = RgbaLimits<T>::max()) noexcept
{
    return {r, g, b, a};
}

#endif // RGBAGENERIC_H
