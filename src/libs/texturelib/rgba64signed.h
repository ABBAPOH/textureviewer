#ifndef RGBA64SIGNED_H
#define RGBA64SIGNED_H

#include <TextureLib/RgbaGeneric>

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

namespace Private {

template<>
struct ColorChannelTraits<qint16> : public ColorChannelTraitsBase<qint16>
{
    using RgbaType = Rgba64Signed;
};

} // namespace Private

inline constexpr Rgba64Signed rgba64Signed(qint16 r, qint16 g, qint16 b, qint16 a = 0x7fff) noexcept
{
    return {r, g, b, a};
}

template<typename T>
inline constexpr Rgba64Signed rgba64Signed(T rgba) noexcept
{
    return Private::convertRgba<Rgba64Signed, T>(rgba);
}

#endif // RGBA64SIGNED_H
