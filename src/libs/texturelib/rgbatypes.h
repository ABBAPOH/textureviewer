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

template<typename T> struct IsColorBase : public std::false_type {};
template<> struct IsColorBase<Rgba32Signed> : public std::true_type {};
template<> struct IsColorBase<QRgb> : public std::true_type {};
template<> struct IsColorBase<Rgba64Float> : public std::true_type {};
template<> struct IsColorBase<Rgba64Signed> : public std::true_type {};
template<> struct IsColorBase<QRgba64> : public std::true_type {};
template<> struct IsColorBase<Rgba128Float> : public std::true_type {};
template<> struct IsColorBase<Rgba128Signed> : public std::true_type {};
template<> struct IsColorBase<Rgba128> : public std::true_type {};

template<typename T> struct IsColor : public IsColorBase<std::decay_t<T>> {};
template<typename T> constexpr bool isColor_v = IsColor<T>::value;

template<typename T> struct IsColorChannelBase : public std::false_type {};
template<> struct IsColorChannelBase<qint8> : public std::true_type {};
template<> struct IsColorChannelBase<quint8> : public std::true_type {};
template<> struct IsColorChannelBase<qint16> : public std::true_type {};
template<> struct IsColorChannelBase<quint16> : public std::true_type {};
template<> struct IsColorChannelBase<qint32> : public std::true_type {};
template<> struct IsColorChannelBase<quint32> : public std::true_type {};
template<> struct IsColorChannelBase<HalfFloat> : public std::true_type {};
template<> struct IsColorChannelBase<float> : public std::true_type {};

template<typename T> struct IsColorChannel : public IsColorChannelBase<std::remove_cv_t<T>> {};
template<typename T> constexpr bool isColorChannel_v = IsColorChannel<T>::value;

template<typename T, typename = std::enable_if_t<isColorChannel_v<T>>>
struct ColorChannelLimits
{
    static T max() { return std::numeric_limits<T>::max(); }
    static T min() { return std::numeric_limits<T>::min(); }
};

template<>
struct ColorChannelLimits<HalfFloat>
{
    static HalfFloat max() { return HalfFloat(+1.0f); }
    static HalfFloat min() { return HalfFloat(-1.0f); }
};

template<>
struct ColorChannelLimits<float>
{
    static float max() { return +1.0f; }
    static float min() { return -1.0f; }
};

template<typename T, typename = std::enable_if_t<isColorChannel_v<T>>>
struct RgbaFromColorChannel { using Type = RgbaGeneric<T>; };
template<> struct RgbaFromColorChannel<qint8> { using Type = Rgba32Signed; };
template<> struct RgbaFromColorChannel<quint8> { using Type = QRgb; };
template<> struct RgbaFromColorChannel<qint16> { using Type = Rgba64Signed; };
template<> struct RgbaFromColorChannel<quint16> { using Type = QRgba64; };

template<typename T> using RgbaFromColorChannel_t = typename RgbaFromColorChannel<T>::Type;

template<typename T, typename = std::enable_if_t<isColor_v<T>>>
struct RgbaToColorChannel { using Type = typename T::DataType; };

template<> struct RgbaToColorChannel<QRgb> { using Type = quint8; };
template<> struct RgbaToColorChannel<QRgba64> { using Type = quint16; };

template<typename T> using rgbaToColorChannel_t = typename RgbaToColorChannel<T>::Type;

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

    explicit constexpr operator quint32() const noexcept { return m_rgba; }

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

constexpr bool operator==(Rgba32Signed lhs, Rgba32Signed rhs)
{ return quint32(lhs) == quint32(rhs); }

constexpr bool operator!=(Rgba32Signed lhs, Rgba32Signed rhs)
{ return quint32(lhs) != quint32(rhs); }

constexpr bool operator<(Rgba32Signed lhs, Rgba32Signed rhs)
{ return quint32(lhs) < quint32(rhs); }

constexpr bool operator>(Rgba32Signed lhs, Rgba32Signed rhs)
{ return quint32(lhs) > quint32(rhs); }

constexpr bool operator<=(Rgba32Signed lhs, Rgba32Signed rhs)
{ return quint32(lhs) <= quint32(rhs); }

constexpr bool operator>=(Rgba32Signed lhs, Rgba32Signed rhs)
{ return quint32(lhs) >= quint32(rhs); }

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

    explicit constexpr operator quint64() const noexcept { return m_rgba; }

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

constexpr bool operator==(Rgba64Signed lhs, Rgba64Signed rhs)
{ return quint64(lhs) == quint64(rhs); }

constexpr bool operator!=(Rgba64Signed lhs, Rgba64Signed rhs)
{ return quint64(lhs) != quint64(rhs); }

constexpr bool operator<(Rgba64Signed lhs, Rgba64Signed rhs)
{ return quint64(lhs) < quint64(rhs); }

constexpr bool operator>(Rgba64Signed lhs, Rgba64Signed rhs)
{ return quint64(lhs) > quint64(rhs); }

constexpr bool operator<=(Rgba64Signed lhs, Rgba64Signed rhs)
{ return quint64(lhs) <= quint64(rhs); }

constexpr bool operator>=(Rgba64Signed lhs, Rgba64Signed rhs)
{ return quint64(lhs) >= quint64(rhs); }

// Rgba64UnSigned

constexpr bool operator==(QRgba64 lhs, QRgba64 rhs)
{ return quint64(lhs) == quint64(rhs); }

constexpr bool operator!=(QRgba64 lhs, QRgba64 rhs)
{ return quint64(lhs) != quint64(rhs); }

constexpr bool operator<(QRgba64 lhs, QRgba64 rhs)
{ return quint64(lhs) < quint64(rhs); }

constexpr bool operator>(QRgba64 lhs, QRgba64 rhs)
{ return quint64(lhs) > quint64(rhs); }

constexpr bool operator<=(QRgba64 lhs, QRgba64 rhs)
{ return quint64(lhs) <= quint64(rhs); }

constexpr bool operator>=(QRgba64 lhs, QRgba64 rhs)
{ return quint64(lhs) >= quint64(rhs); }

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
            DataType alpha = Private::ColorChannelLimits<DataType>::max()) noexcept
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

    template<typename U>
    friend constexpr bool operator<(const RgbaGeneric<U> &lhs, const RgbaGeneric<U> &rhs) noexcept;
};

template<typename T>
constexpr bool operator==(const RgbaGeneric<T> &lhs, const RgbaGeneric<T> &rhs) noexcept
{
    return lhs.red() == rhs.red()
            && lhs.green() == rhs.green()
            && lhs.blue() == rhs.blue()
            && lhs.alpha() == rhs.alpha();
}

template<typename T>
constexpr bool operator!=(const RgbaGeneric<T> &lhs, const RgbaGeneric<T> &rhs) noexcept
{ return !(lhs == rhs); }

template<typename T>
constexpr bool operator<(const RgbaGeneric<T> &lhs, const RgbaGeneric<T> &rhs) noexcept
{
    return std::tie(lhs.m_red, lhs.m_green, lhs.m_blue, lhs.m_alpha)
            < std::tie(rhs.m_red, rhs.m_green, rhs.m_blue, rhs.m_alpha);
}

template<typename T>
constexpr bool operator>(const RgbaGeneric<T> &lhs, const RgbaGeneric<T> &rhs) noexcept
{
    return rhs < lhs;
}

template<typename T>
constexpr bool operator<=(const RgbaGeneric<T> &lhs, const RgbaGeneric<T> &rhs) noexcept
{
    return !(lhs > rhs);
}

template<typename T>
constexpr bool operator>=(const RgbaGeneric<T> &lhs, const RgbaGeneric<T> &rhs) noexcept
{
    return !(lhs < rhs);
}

// Helpers

// get* helpers

template<typename T, typename = std::enable_if_t<Private::isColor_v<T>>>
constexpr Private::rgbaToColorChannel_t<T> getRed  (T color) noexcept { return color.red();   }

template<typename T, typename = std::enable_if_t<Private::isColor_v<T>>>
constexpr Private::rgbaToColorChannel_t<T> getGreen(T color) noexcept { return color.green(); }

template<typename T, typename = std::enable_if_t<Private::isColor_v<T>>>
constexpr Private::rgbaToColorChannel_t<T> getBlue (T color) noexcept { return color.blue();  }

template<typename T, typename = std::enable_if_t<Private::isColor_v<T>>>
constexpr Private::rgbaToColorChannel_t<T> getAlpha(T color) noexcept { return color.alpha(); }

inline constexpr quint8 getRed  (QRgb color) noexcept { return quint8(qRed(color));   }
inline constexpr quint8 getGreen(QRgb color) noexcept { return quint8(qGreen(color)); }
inline constexpr quint8 getBlue (QRgb color) noexcept { return quint8(qBlue(color));  }
inline constexpr quint8 getAlpha(QRgb color) noexcept { return quint8(qAlpha(color)); }

namespace Private {

template<typename T, typename = std::enable_if_t<isColorChannel_v<T>>>
constexpr RgbaFromColorChannel_t<T> createRgbaHelper(T r, T g, T b, T a) noexcept
{ return {r, g, b, a}; }

template<>
constexpr RgbaFromColorChannel_t<quint8> createRgbaHelper<quint8>(
        quint8 r, quint8 g, quint8 b, quint8 a) noexcept
{ return qRgba(r, g, b, a); }

template<>
constexpr RgbaFromColorChannel_t<quint16> createRgbaHelper<quint16>(
        quint16 r, quint16 g, quint16 b, quint16 a) noexcept
{ return qRgba64(r, g, b, a); }

template<typename T>
constexpr RgbaFromColorChannel_t<T> createRgba(
        T r, T g, T b, T a = ColorChannelLimits<T>::max()) noexcept
{ return createRgbaHelper<T>(r, g, b, a); }

template<typename T, typename = std::enable_if_t<isColorChannel_v<T>>>
constexpr bool is_float_v = !std::is_integral_v<T>;

template<typename Dst, typename Src,
         typename = std::enable_if_t<isColorChannel_v<Src> && isColorChannel_v<Dst>>>
inline constexpr Dst convertChannel(Src src)
{
    if constexpr (std::is_same_v<std::decay_t<Src>, std::decay_t<Dst>>)
            return src;
    const auto maxSrc = ColorChannelLimits<Src>::max();
    const auto maxDst = ColorChannelLimits<Dst>::max();

    // floats can be out of [-1, 1] range
    src = qBound(ColorChannelLimits<Src>::min(), src, maxSrc);
    if constexpr (is_float_v<Dst>) {
        return Dst(maxDst * src / maxSrc);
    } else {
        if constexpr (is_float_v<Src>)
            return Dst(maxDst * src / maxSrc + 0.5);
        else
            return Dst(maxDst * (1.0 * src / maxSrc));
    }
}

template<typename Dst, typename Src,
         typename = std::enable_if_t<isColor_v<Src> && isColor_v<Dst>>>
constexpr Dst convertRgba(Src src) noexcept
{
    using SrcChannel = Private::rgbaToColorChannel_t<Src>;
    using DstChannel = Private::rgbaToColorChannel_t<Dst>;
    return createRgba<DstChannel>(
            convertChannel<DstChannel, SrcChannel>(getRed(src)),
            convertChannel<DstChannel, SrcChannel>(getGreen(src)),
            convertChannel<DstChannel, SrcChannel>(getBlue(src)),
            convertChannel<DstChannel, SrcChannel>(getAlpha(src))
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

inline constexpr Rgba64Float rgba64Float(float r, float g, float b, float a = 1.0f) noexcept
{
    return rgba64Float(HalfFloat(r), HalfFloat(g), HalfFloat(b), HalfFloat(a));
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
