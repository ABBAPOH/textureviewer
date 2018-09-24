#ifndef RGBA64FLOAT_H
#define RGBA64FLOAT_H

#include <TextureLib/Rgba32Signed>
#include <TextureLib/RgbaGeneric>

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

using Rgba64Float = RgbaGeneric<HalfFloat>;

namespace Private {

inline HalfFloat constexpr normalizeF16(qint8 value) noexcept { return HalfFloat(1.0f * value / 0x7f); }
inline HalfFloat constexpr normalizeF16(quint8 value) noexcept { return HalfFloat(1.0f * value / 0xffu); }
inline HalfFloat constexpr normalizeF16(quint16 value) noexcept { return HalfFloat(1.0f * value / 0xffffu); }
inline HalfFloat constexpr bounded(HalfFloat value) noexcept { return qBound(HalfFloat(0.0f), value, HalfFloat(1.0f)); }
inline HalfFloat constexpr boundedSigned(HalfFloat value) noexcept { return qBound(HalfFloat(-1.0f), value, HalfFloat(1.0f)); }

} // namespace Private

// constructors

inline constexpr Rgba64Float rgba64Float(HalfFloat r, HalfFloat g, HalfFloat b, HalfFloat a = HalfFloat(1.0)) noexcept
{
    return createRgba<HalfFloat>(r, g, b, a);
}

// QRgba

inline constexpr Rgba64Float rgba64Float(QRgb rgba) noexcept
{
    return {
            Private::normalizeF16(quint8(qRed(rgba))),
            Private::normalizeF16(quint8(qGreen(rgba))),
            Private::normalizeF16(quint8(qBlue(rgba))),
            Private::normalizeF16(quint8(qAlpha(rgba)))
    };
}

inline constexpr QRgb qRgba(Rgba64Float rgba) noexcept
{
    return qRgba(
            quint8(0xffu * Private::bounded(rgba.red()) + 0.5f),
            quint8(0xffu * Private::bounded(rgba.green()) + 0.5f),
            quint8(0xffu * Private::bounded(rgba.blue()) + 0.5f),
            quint8(0xffu * Private::bounded(rgba.alpha()) + 0.5f));
}

// Rgba32Signed

inline constexpr Rgba64Float rgba64Float(Rgba32Signed rgba) noexcept
{
    return {
            Private::normalizeF16(rgba.red()),
            Private::normalizeF16(rgba.green()),
            Private::normalizeF16(rgba.blue()),
            Private::normalizeF16(rgba.alpha())
    };
}

inline constexpr Rgba32Signed rgba32Signed(Rgba64Float rgba) noexcept
{
    // TODO (abbapoh): we do not cover -128 value for now, should be fixed
    return {
            qint8(0x7f * Private::boundedSigned(rgba.red())),
            qint8(0x7f * Private::boundedSigned(rgba.green())),
            qint8(0x7f * Private::boundedSigned(rgba.blue())),
            qint8(0x7f * Private::boundedSigned(rgba.alpha()))
    };
}

// QRgba64

inline constexpr Rgba64Float rgba64Float(QRgba64 rgba) noexcept
{
    return {
            Private::normalizeF16(quint16(qRed(rgba))),
            Private::normalizeF16(quint16(qGreen(rgba))),
            Private::normalizeF16(quint16(qBlue(rgba))),
            Private::normalizeF16(quint16(qAlpha(rgba)))
    };
}

inline constexpr QRgba64 qRgba64(Rgba64Float rgba) noexcept
{
    return qRgba64(
            0xffffu * rgba.red(),
            0xffffu * rgba.green(),
            0xffffu * rgba.blue(),
            0xffffu * rgba.alpha()
    );
}

#endif // RGBA64FLOAT_H
