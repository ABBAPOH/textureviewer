#ifndef RGBAGENERIC_H
#define RGBAGENERIC_H

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

#include <QtGlobal>

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

    constexpr DataType red()   const noexcept { return m_red;   }
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

// helpers

namespace Private {

template<typename T>
struct RgbaCreateHelper
{
    using Src = T;
    using Dst = RgbaGeneric<T>;
    static constexpr Dst create(Src r, Src g, Src b, Src a) noexcept { return {r, g, b, a}; }
};

template<>
struct RgbaCreateHelper<quint8>
{
    using Src = quint8;
    using Dst = QRgb;
    static constexpr Dst create(Src r, Src g, Src b, Src a) noexcept { return qRgba(r, g, b, a); }
};

template<>
struct RgbaCreateHelper<quint16>
{
    using Src = quint16;
    using Dst = QRgba64;
    static constexpr Dst create(Src r, Src g, Src b, Src a) noexcept { return qRgba64(r, g, b, a); }
};

template<typename T>
struct RgbaChannelTypeHelper
{
    using Type = typename T::DataType;
};

template<>
struct RgbaChannelTypeHelper<QRgb>
{
    using Type = quint8;
};

template<>
struct RgbaChannelTypeHelper<QRgba64>
{
    using Type = quint16;
};

template<typename T>
constexpr typename RgbaCreateHelper<T>::Dst createRgba(
        T r, T g, T b, T a = RgbaLimits<T>::max()) noexcept
{
    return RgbaCreateHelper<T>::create(r, g, b, a);
}

template<typename Dst, typename Src>
inline constexpr Dst normalize(Src src)
{
    src = qBound(Src(0), src, RgbaLimits<Src>::max());

    if (!std::numeric_limits<Dst>::is_integer && std::numeric_limits<Src>::is_integer)
        return Dst(RgbaLimits<Dst>::max() * src / RgbaLimits<Src>::max());
    else if (std::numeric_limits<Dst>::is_integer && !std::numeric_limits<Src>::is_integer)
        return Dst(RgbaLimits<Dst>::max() * src / RgbaLimits<Src>::max() + 0.5);
    else if (std::numeric_limits<Dst>::is_integer && std::numeric_limits<Src>::is_integer)
        return Dst(RgbaLimits<Dst>::max() * (1.0 * src / RgbaLimits<Src>::max()));
    else if (!std::numeric_limits<Dst>::is_integer && !std::numeric_limits<Src>::is_integer)
        return Dst(RgbaLimits<Dst>::max() * src / RgbaLimits<Src>::max());
}

template<typename Dst, typename Src>
constexpr typename RgbaCreateHelper<Dst>::Dst convertRgba(typename RgbaCreateHelper<Src>::Dst src) noexcept
{
    return createRgba<Dst>(
            normalize<Dst, Src>(qRed(src)),
            normalize<Dst, Src>(qGreen(src)),
            normalize<Dst, Src>(qBlue(src)),
            normalize<Dst, Src>(qAlpha(src))
    );
}

static_assert(std::is_same<QRgb, decltype(createRgba(quint8(), quint8(), quint8(), quint8()))>::value, "Not same");
static_assert(std::is_same<QRgba64, decltype(createRgba(quint16(), quint16(), quint16()))>::value, "Not same");

} // namespace Private

#endif // RGBAGENERIC_H
