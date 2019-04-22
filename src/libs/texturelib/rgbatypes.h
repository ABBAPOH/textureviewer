#ifndef RGBATYPES_H
#define RGBATYPES_H

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

#include <HalfFloat>

// Forward-declare types

template<typename T> class RgbaGeneric;

class Rgba32Signed;
//using Rgba32Unsigned = QRgb;
using Rgba64Float = RgbaGeneric<HalfFloat>;
class Rgba64Signed;
//using Rgba64Unsigned = QRgba64;
using Rgba128Float = RgbaGeneric<float>;
using Rgba128Signed = RgbaGeneric<qint32>;
using Rgba128 = RgbaGeneric<quint32>;

// Traits

namespace Private {

template<typename T> struct IsColor : public std::false_type {};
template<> struct IsColor<Rgba32Signed> : public std::true_type {};
template<> struct IsColor<QRgb> : public std::true_type {};
template<> struct IsColor<Rgba64Float> : public std::true_type {};
template<> struct IsColor<Rgba64Signed> : public std::true_type {};
template<> struct IsColor<QRgba64> : public std::true_type {};
template<> struct IsColor<Rgba128Float> : public std::true_type {};
template<> struct IsColor<Rgba128Signed> : public std::true_type {};
template<> struct IsColor<Rgba128> : public std::true_type {};

template<typename T> constexpr bool isColor_v = IsColor<T>::value;

template<typename T> struct IsColorChannel : public std::false_type {};
template<> struct IsColorChannel<qint8> : public std::true_type {};
template<> struct IsColorChannel<quint8> : public std::true_type {};
template<> struct IsColorChannel<qint16> : public std::true_type {};
template<> struct IsColorChannel<quint16> : public std::true_type {};
template<> struct IsColorChannel<qint32> : public std::true_type {};
template<> struct IsColorChannel<quint32> : public std::true_type {};
template<> struct IsColorChannel<HalfFloat> : public std::true_type {};
template<> struct IsColorChannel<float> : public std::true_type {};
template<> struct IsColorChannel<double> : public std::true_type {};

template<typename T> constexpr bool isColorChannel_v = IsColorChannel<T>::value;

template<typename T>
struct ColorChannelTraitsBase
{
    using DataType = T;
    using RgbaType = RgbaGeneric<T>;

    static DataType max() { return std::numeric_limits<DataType>::max(); }
    static DataType min() { return std::numeric_limits<DataType>::min(); }
};

template<typename T, std::enable_if_t<isColorChannel_v<T>, int> = 0>
struct ColorChannelTraits : public ColorChannelTraitsBase<T> {};

template<> struct ColorChannelTraits<qint8> : public ColorChannelTraitsBase<qint8>
{ using RgbaType = Rgba32Signed; };

template<> struct ColorChannelTraits<quint8> : public ColorChannelTraitsBase<quint8>
{ using RgbaType = QRgb; };

template<> struct ColorChannelTraits<qint16> : public ColorChannelTraitsBase<qint16>
{ using RgbaType = Rgba64Signed; };

template<> struct ColorChannelTraits<quint16> : public ColorChannelTraitsBase<quint16>
{ using RgbaType = QRgba64; };

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

template<typename T, std::enable_if_t<isColor_v<T>, int> = 0>
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

// Types

// Rgba32Signed

class Rgba32Signed
{
    quint32 m_rgba {0};

    // Make sure that the representation always has the order: red green blue alpha, independent
    // of byte order. This way, vector operations that assume 4 8-bit values see the correct ones.
    enum Shifts {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        RedShift = 24,
        GreenShift = 16,
        BlueShift = 8,
        AlphaShift = 0
#else // little endian:
        RedShift = 0,
        GreenShift = 8,
        BlueShift = 16,
        AlphaShift = 24
#endif
    };

public:
    using DataType = qint8;

    constexpr Rgba32Signed() noexcept = default;
    explicit constexpr Rgba32Signed(quint32 c) noexcept : m_rgba(c) {}
    constexpr Rgba32Signed(qint8 red, qint8 green, qint8 blue, qint8 alpha = 127) noexcept
    {
        m_rgba = quint32(quint8(red))   << RedShift
               | quint32(quint8(green)) << GreenShift
               | quint32(quint8(blue))  << BlueShift
               | quint32(quint8(alpha)) << AlphaShift;
    }

    constexpr Rgba32Signed &operator=(quint32 rgba) noexcept
    {
        m_rgba = rgba;
        return *this;
    }

    constexpr operator quint32() const noexcept { return m_rgba; }

    constexpr qint8 red()   const noexcept { return qint8(m_rgba >> RedShift);   }
    constexpr qint8 green() const noexcept { return qint8(m_rgba >> GreenShift); }
    constexpr qint8 blue()  const noexcept { return qint8(m_rgba >> BlueShift);  }
    constexpr qint8 alpha() const noexcept { return qint8(m_rgba >> AlphaShift); }

    constexpr void setRed(qint8 red) noexcept
    {
        m_rgba = (m_rgba & ~(0xffu << RedShift))   | (quint32(quint8(red))   << RedShift);
    }

    constexpr void setGreen(qint8 green) noexcept
    {
        m_rgba = (m_rgba & ~(0xffu << GreenShift)) | (quint32(quint8(green)) << GreenShift);

    }

    constexpr void setBlue(qint8 blue) noexcept
    {
        m_rgba = (m_rgba & ~(0xffu << BlueShift))  | (quint32(quint8(blue))  << BlueShift);
    }

    constexpr void setAlpha(qint8 alpha) noexcept
    {
        m_rgba = (m_rgba & ~(0xffu << AlphaShift)) | (quint32(quint8(alpha)) << AlphaShift);
    }
};

constexpr bool operator ==(Rgba32Signed lhs, Rgba32Signed rhs)
{
    return quint32(lhs) == quint32(rhs);
}

constexpr bool operator !=(Rgba32Signed lhs, Rgba32Signed rhs)
{
    return !(lhs == rhs);
}

// Rgba64Signed

class Rgba64Signed
{
    quint64 m_rgba {0};

    // Make sure that the representation always has the order: red green blue alpha, independent
    // of byte order. This way, vector operations that assume 4 8-bit values see the correct ones.
    enum Shifts {
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
        RedShift = 48,
        GreenShift = 32,
        BlueShift = 16,
        AlphaShift = 0
#else // little endian:
        RedShift = 0,
        GreenShift = 16,
        BlueShift = 32,
        AlphaShift = 48
#endif
    };

public:
    using DataType = qint16;

    constexpr Rgba64Signed() noexcept = default;
    explicit constexpr Rgba64Signed(quint64 c) noexcept : m_rgba(c) {}
    constexpr Rgba64Signed(qint16 red, qint16 green, qint16 blue, qint16 alpha = 127) noexcept
    {
        m_rgba = quint64(quint16(red))   << RedShift
               | quint64(quint16(green)) << GreenShift
               | quint64(quint16(blue))  << BlueShift
               | quint64(quint16(alpha)) << AlphaShift;
    }

    constexpr Rgba64Signed &operator=(quint64 rgba) noexcept
    {
        m_rgba = rgba;
        return *this;
    }

    constexpr operator quint64() const noexcept { return m_rgba; }

    constexpr qint16 red()   const noexcept { return qint16(m_rgba >> RedShift);   }
    constexpr qint16 green() const noexcept { return qint16(m_rgba >> GreenShift); }
    constexpr qint16 blue()  const noexcept { return qint16(m_rgba >> BlueShift);  }
    constexpr qint16 alpha() const noexcept { return qint16(m_rgba >> AlphaShift); }

    constexpr void setRed(qint16 red) noexcept
    {
        m_rgba = (m_rgba & ~(0xffffull << RedShift))   | (quint64(quint16(red))   << RedShift);
    }

    constexpr void setGreen(qint16 green) noexcept
    {
        m_rgba = (m_rgba & ~(0xffffull << GreenShift)) | (quint64(quint16(green)) << GreenShift);
    }

    constexpr void setBlue(qint16 blue) noexcept
    {
        m_rgba = (m_rgba & ~(0xffffull << BlueShift))  | (quint64(quint16(blue))  << BlueShift);
    }

    constexpr void setAlpha(qint16 alpha) noexcept
    {
        m_rgba = (m_rgba & ~(0xffffull << AlphaShift)) | (quint64(quint16(alpha)) << AlphaShift);
    }
};

constexpr bool operator ==(Rgba64Signed lhs, Rgba64Signed rhs)
{
    return quint32(lhs) == quint32(rhs);
}

constexpr bool operator !=(Rgba64Signed lhs, Rgba64Signed rhs)
{
    return !(lhs == rhs);
}

// RgbaGeneric

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

// Helpers

// get* helpers

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

// Constructors

inline constexpr Rgba32Signed rgba32Signed(qint8 r, qint8 g, qint8 b, qint8 a = 127) noexcept
{
    return {r, g, b, a};
}

template<typename T>
inline constexpr Rgba32Signed rgba32Signed(T rgba) noexcept
{
    return Private::convertRgba<Rgba32Signed, T>(rgba);
}

template<typename T>
inline constexpr QRgb qRgba(T rgba) noexcept
{
    return Private::convertRgba<QRgb, T>(rgba);
}

inline constexpr Rgba64Float rgba64Float(HalfFloat r, HalfFloat g, HalfFloat b, HalfFloat a = HalfFloat(1.0)) noexcept
{
    return Private::createRgba<HalfFloat>(r, g, b, a);
}

template<typename T>
inline constexpr Rgba64Float rgba64Float(T rgba) noexcept
{
    return Private::convertRgba<Rgba64Float, T>(rgba);
}

inline constexpr Rgba64Signed rgba64Signed(qint16 r, qint16 g, qint16 b, qint16 a = 0x7fff) noexcept
{
    return {r, g, b, a};
}

template<typename T>
inline constexpr Rgba64Signed rgba64Signed(T rgba) noexcept
{
    return Private::convertRgba<Rgba64Signed, T>(rgba);
}

template<typename T>
inline constexpr QRgba64 qRgba64(T rgba) noexcept
{
    return Private::convertRgba<QRgba64, T>(rgba);
}

inline constexpr Rgba128Float rgba128Float(float r, float g, float b, float a = 1.0f) noexcept
{
    return Private::createRgba<float>(r, g, b, a);
}

template<typename T>
inline constexpr Rgba128Float rgba128Float(T rgba) noexcept
{
    return Private::convertRgba<Rgba128Float, T>(rgba);
}

inline constexpr Rgba128Signed rgba128Signed(qint32 r, qint32 g, qint32 b, qint32 a = 0x7fffffff) noexcept
{
    return Private::createRgba<qint32>(r, g, b, a);
}

template<typename T>
inline constexpr Rgba128Signed rgba128Signed(T rgba) noexcept
{
    return Private::convertRgba<Rgba128Signed, T>(rgba);
}

inline constexpr Rgba128 rgba128(quint32 r, quint32 g, quint32 b, quint32 a = 0xffffffff) noexcept
{
    return Private::createRgba<quint32>(r, g, b, a);
}

template<typename T>
inline constexpr Rgba128 rgba128(T rgba) noexcept
{
    return Private::convertRgba<Rgba128, T>(rgba);
}

#endif // RGBATYPES_H
