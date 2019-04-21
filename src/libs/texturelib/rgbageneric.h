#ifndef RGBAGENERIC_H
#define RGBAGENERIC_H

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

#include <QtGlobal>

#include <HalfFloat>

template<typename T> class RgbaGeneric;

namespace Private {

template<typename T>
struct ColorChannelTraitsBase
{
    using DataType = T;
    using RgbaType = RgbaGeneric<T>;

    static DataType max() { return std::numeric_limits<DataType>::max(); }
    static DataType min() { return std::numeric_limits<DataType>::min(); }
};

template<typename T>
struct ColorChannelTraits : public ColorChannelTraitsBase<T>
{
};

template<>
struct ColorChannelTraits<quint8> : public ColorChannelTraitsBase<quint8>
{
    using RgbaType = QRgb;
};

template<>
struct ColorChannelTraits<quint16> : public ColorChannelTraitsBase<quint16>
{
    using RgbaType = QRgba64;
};

template<>
struct ColorChannelTraits<float> : public ColorChannelTraitsBase<float>
{
    static DataType max() { return +1.0f; }
    static DataType min() { return -1.0f; }
};

template<>
struct ColorChannelTraits<HalfFloat> : public ColorChannelTraitsBase<HalfFloat>
{
    static DataType max() { return HalfFloat(+1.0f); }
    static DataType min() { return HalfFloat(-1.0f); }
};

template<typename T>
struct RgbaTraits
{
    using DataType = typename T::DataType;
    using RgbaType = T;
};

template<>
struct RgbaTraits<QRgb>
{
    using DataType = quint8;
    using RgbaType = QRgb;
};

template<>
struct RgbaTraits<QRgba64>
{
    using DataType = quint16;
    using RgbaType = QRgba64;
};

} // namespace Private

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
            DataType alpha = Private::ColorChannelTraits<DataType>::max()) noexcept
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

// getHelpers

template<typename T>
constexpr typename Private::RgbaTraits<T>::DataType getRed  (T color) noexcept { return color.red();   }
template<typename T>
constexpr typename Private::RgbaTraits<T>::DataType getGreen(T color) noexcept { return color.green(); }
template<typename T>
constexpr typename Private::RgbaTraits<T>::DataType getBlue (T color) noexcept { return color.blue();  }
template<typename T>
constexpr typename Private::RgbaTraits<T>::DataType getAlpha(T color) noexcept { return color.alpha(); }

inline constexpr quint8 getRed  (QRgb color) noexcept { return quint8(qRed(color));   }
inline constexpr quint8 getGreen(QRgb color) noexcept { return quint8(qGreen(color)); }
inline constexpr quint8 getBlue (QRgb color) noexcept { return quint8(qBlue(color));  }
inline constexpr quint8 getAlpha(QRgb color) noexcept { return quint8(qAlpha(color)); }

// helpers

namespace Private {

template<typename T>
constexpr typename ColorChannelTraits<T>::RgbaType createRgbaHelper(
        T r, T g, T b, T a) noexcept
{
    return {r, g, b, a};
}

template<>
constexpr typename ColorChannelTraits<quint8>::RgbaType createRgbaHelper<quint8>(
        quint8 r, quint8 g, quint8 b, quint8 a) noexcept
{
    return qRgba(r, g, b, a);
}

template<>
constexpr typename ColorChannelTraits<quint16>::RgbaType createRgbaHelper<quint16>(
        quint16 r, quint16 g, quint16 b, quint16 a) noexcept
{
    return qRgba64(r, g, b, a);
}

template<typename T>
constexpr typename ColorChannelTraits<T>::RgbaType createRgba(
        T r, T g, T b, T a = ColorChannelTraits<T>::max()) noexcept
{
    return createRgbaHelper<T>(r, g, b, a);
}

template<typename Dst, typename Src>
inline constexpr Dst normalize(Src src)
{
    src = qBound(Src(0), src, ColorChannelTraits<Src>::max());

    if (!std::numeric_limits<Dst>::is_integer && std::numeric_limits<Src>::is_integer)
        return Dst(ColorChannelTraits<Dst>::max() * src / ColorChannelTraits<Src>::max());
    else if (std::numeric_limits<Dst>::is_integer && !std::numeric_limits<Src>::is_integer)
        return Dst(ColorChannelTraits<Dst>::max() * src / ColorChannelTraits<Src>::max() + 0.5);
    else if (std::numeric_limits<Dst>::is_integer && std::numeric_limits<Src>::is_integer)
        return Dst(ColorChannelTraits<Dst>::max() * (1.0 * src / ColorChannelTraits<Src>::max()));
    else if (!std::numeric_limits<Dst>::is_integer && !std::numeric_limits<Src>::is_integer)
        return Dst(ColorChannelTraits<Dst>::max() * src / ColorChannelTraits<Src>::max());
}

template<typename Dst, typename Src>
constexpr Dst convertRgba(Src src) noexcept
{
    using SrcChannel = typename Private::RgbaTraits<Src>::DataType;
    using DstChannel = typename Private::RgbaTraits<Dst>::DataType;
    return createRgba<DstChannel>(
            normalize<DstChannel, SrcChannel>(getRed(src)),
            normalize<DstChannel, SrcChannel>(getGreen(src)),
            normalize<DstChannel, SrcChannel>(getBlue(src)),
            normalize<DstChannel, SrcChannel>(getAlpha(src))
    );
}

static_assert(std::is_same<QRgb, decltype(createRgba(quint8(), quint8(), quint8(), quint8()))>::value, "Not same");
static_assert(std::is_same<QRgba64, decltype(createRgba(quint16(), quint16(), quint16()))>::value, "Not same");

} // namespace Private

template<typename T>
constexpr bool operator==(const RgbaGeneric<T> &lhs, const RgbaGeneric<T> &rhs)
{
    return lhs.red() == rhs.red()
            && lhs.green() == rhs.green()
            && lhs.blue() == rhs.blue()
            && lhs.alpha() == rhs.alpha();
}

template<typename T>
constexpr bool operator!=(const RgbaGeneric<T> &lhs, const RgbaGeneric<T> &rhs)
{ return !(lhs == rhs); }

// Qt color types

template<typename T>
inline constexpr QRgb qRgba(T rgba) noexcept
{
    return Private::convertRgba<QRgb, T>(rgba);
}

template<typename T>
inline constexpr QRgba64 qRgba64(T rgba) noexcept
{
    return Private::convertRgba<QRgba64, T>(rgba);
}

#endif // RGBAGENERIC_H
