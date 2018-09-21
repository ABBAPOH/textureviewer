#ifndef RGBA32SIGNED_H
#define RGBA32SIGNED_H

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

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
    constexpr Rgba32Signed() noexcept = default;
    explicit constexpr Rgba32Signed(quint32 c) noexcept : m_rgba(c) {}
    constexpr Rgba32Signed(qint8 red, qint8 green, qint8 blue, qint8 alpha = 127) noexcept
    {
        m_rgba = quint32(quint8(red))   << RedShift
               | quint32(quint8(green)) << GreenShift
               | quint32(quint8(blue))  << BlueShift
               | quint32(quint8(alpha)) << AlphaShift;
    }

    Rgba32Signed &operator=(quint32 rgba)
    {
        m_rgba = rgba;
        return *this;
    }

    constexpr qint8 red()   const noexcept { return qint8(m_rgba >> RedShift);   }
    constexpr qint8 green() const noexcept { return qint8(m_rgba >> GreenShift); }
    constexpr qint8 blue()  const noexcept { return qint8(m_rgba >> BlueShift);  }
    constexpr qint8 alpha() const noexcept { return qint8(m_rgba >> AlphaShift); }

    constexpr void setRed(qint8 red)
    {
        m_rgba = (m_rgba & ~(0xffu << RedShift))   | (quint32(quint8(red))   << RedShift);
    }

    constexpr void setGreen(qint8 green)
    {
        m_rgba = (m_rgba & ~(0xffu << GreenShift)) | (quint32(quint8(green)) << GreenShift);

    }

    constexpr void setBlue(qint8 blue)
    {
        m_rgba = (m_rgba & ~(0xffu << BlueShift))  | (quint32(quint8(blue))  << BlueShift);
    }

    constexpr void setAlpha(qint8 alpha)
    {
        m_rgba = (m_rgba & ~(0xffu << AlphaShift)) | (quint32(quint8(alpha)) << AlphaShift);
    }
};

// qHelpers

inline constexpr qint8 qRed  (Rgba32Signed color) noexcept { return color.red();   }
inline constexpr qint8 qGreen(Rgba32Signed color) noexcept { return color.green(); }
inline constexpr qint8 qBlue (Rgba32Signed color) noexcept { return color.blue();  }
inline constexpr qint8 qAlpha(Rgba32Signed color) noexcept { return color.alpha(); }

// TODO (abbapoh) : we can't enable this overload because QRgb is quint32 too (=
// I hope this will be fixed in Qt6
//inline constexpr Rgba32Signed rgba32Signed(quint32 value) noexcept
//{
//    return Rgba32Signed(value);
//}

inline constexpr Rgba32Signed rgba32Signed(qint8 r, qint8 g, qint8 b, qint8 a = 127) noexcept
{
    return {r, g, b, a};
}

// QRgb convertions

inline constexpr Rgba32Signed rgba32Signed(QRgb rgba) noexcept
{
    return {qint8(qRed(rgba)   >> 1u),
            qint8(qGreen(rgba) >> 1u),
            qint8(qBlue(rgba)  >> 1u),
            qint8(qAlpha(rgba) >> 1u)};
}

inline constexpr QRgb qRgba(Rgba32Signed rgba)
{
    return qRgba(
            quint8(std::max<qint8>(0, rgba.red()))   << 1u,
            quint8(std::max<qint8>(0, rgba.green())) << 1u,
            quint8(std::max<qint8>(0, rgba.blue()))  << 1u,
            quint8(std::max<qint8>(0, rgba.alpha())) << 1u);
}

// QRgba64 convertions

inline constexpr Rgba32Signed rgba32Signed(QRgba64 rgba) noexcept
{
    return {qint8(qRed(rgba)   >> 9u),
            qint8(qGreen(rgba) >> 9u),
            qint8(qBlue(rgba)  >> 9u),
            qint8(qAlpha(rgba) >> 9u)};
}

inline constexpr QRgba64 qRgba64(Rgba32Signed rgba)
{
    return qRgba64(
            quint16(std::max<qint8>(0, rgba.red()))   << 9u,
            quint16(std::max<qint8>(0, rgba.green())) << 9u,
            quint16(std::max<qint8>(0, rgba.blue()))  << 9u,
            quint16(std::max<qint8>(0, rgba.alpha())) << 9u);
}

#endif // RGBA32SIGNED_H
