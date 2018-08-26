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
    explicit constexpr Rgba32Signed(qint8 red, qint8 green, qint8 blue, qint8 alpha = 127) noexcept
    {
        m_rgba = quint32(quint8(red))   << RedShift
               | quint32(quint8(green)) << GreenShift
               | quint32(quint8(blue))  << BlueShift
               | quint32(quint8(alpha)) << AlphaShift;
    }

    Rgba32Signed operator=(quint32 rgba)
    {
        m_rgba = rgba;
        return *this;
    }

    constexpr qint8 red()   const noexcept { return qint8(m_rgba >> RedShift);   }
    constexpr qint8 green() const noexcept { return qint8(m_rgba >> GreenShift); }
    constexpr qint8 blue()  const noexcept { return qint8(m_rgba >> BlueShift);  }
    constexpr qint8 alpha() const noexcept { return qint8(m_rgba >> AlphaShift); }
};

#endif // RGBA32SIGNED_H
