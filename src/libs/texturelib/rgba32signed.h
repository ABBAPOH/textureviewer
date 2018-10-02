#ifndef RGBA32SIGNED_H
#define RGBA32SIGNED_H

#include <TextureLib/RgbaGeneric>

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

namespace Private {

template<>
struct ColorChannelTraits<qint8> : public ColorChannelTraitsBase<qint8>
{
    using RgbaType = Rgba32Signed;
};

} // namespace Private

inline constexpr Rgba32Signed rgba32Signed(qint8 r, qint8 g, qint8 b, qint8 a = 127) noexcept
{
    return {r, g, b, a};
}

// Generic

template<typename T>
inline constexpr Rgba32Signed rgba32Signed(T rgba) noexcept
{
    return Private::convertRgba<qint8, typename Private::RgbaTraits<T>::DataType>(rgba);
}

#endif // RGBA32SIGNED_H
