#ifndef RGBA128FLOAT_H
#define RGBA128FLOAT_H

#include <TextureLib/Rgba32Signed>
#include <TextureLib/Rgba64Float>
#include <TextureLib/RgbaGeneric>

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

using Rgba128Float = RgbaGeneric<float>;

namespace Private {

inline float constexpr normalizeF32(qint8 value) noexcept { return 1.0f * value / 0x7f; }
inline float constexpr normalizeF32(quint8 value) noexcept { return 1.0f * value / 0xffu; }
inline float constexpr normalizeF32(quint16 value) noexcept { return 1.0f * value / 0xffffu; }
inline float constexpr bounded(float value) noexcept { return qBound(0.0f, value, 1.0f); }
inline float constexpr boundedSigned(float value) noexcept { return qBound(-1.0f, value, 1.0f); }

} // namespace Private

// constructors

inline constexpr Rgba128Float rgba128Float(float r, float g, float b, float a = 1.0f) noexcept
{
    return createRgba<float>(r, g, b, a);
}

// QRgba

inline constexpr Rgba128Float rgba128Float(QRgb rgba) noexcept
{
    return rgba128Float(
            Private::normalizeF32(quint8(qRed(rgba))),
            Private::normalizeF32(quint8(qGreen(rgba))),
            Private::normalizeF32(quint8(qBlue(rgba))),
            Private::normalizeF32(quint8(qAlpha(rgba)))
    );
}

inline constexpr QRgb qRgba(Rgba128Float rgba) noexcept
{
    return qRgba(
            0xffu * Private::bounded(rgba.red()),
            0xffu * Private::bounded(rgba.green()),
            0xffu * Private::bounded(rgba.blue()),
            0xffu * Private::bounded(rgba.alpha()));
}

// Rgba32Signed

inline constexpr Rgba128Float rgba128Float(Rgba32Signed rgba) noexcept
{
    return {
            Private::normalizeF32(rgba.red()),
            Private::normalizeF32(rgba.green()),
            Private::normalizeF32(rgba.blue()),
            Private::normalizeF32(rgba.alpha())
    };
}

inline constexpr Rgba32Signed rgba32Signed(Rgba128Float rgba) noexcept
{
    // TODO (abbapoh): we do not cover -128 value for now, should be fixed
    return rgba32Signed(
            0x7f * Private::boundedSigned(rgba.red()),
            0x7f * Private::boundedSigned(rgba.green()),
            0x7f * Private::boundedSigned(rgba.blue()),
            0x7f * Private::boundedSigned(rgba.alpha())
    );
}

// QRgba64

inline constexpr Rgba128Float rgba128Float(QRgba64 rgba) noexcept
{
    return {
            Private::normalizeF32(quint16(qRed(rgba))),
            Private::normalizeF32(quint16(qGreen(rgba))),
            Private::normalizeF32(quint16(qBlue(rgba))),
            Private::normalizeF32(quint16(qAlpha(rgba)))
    };
}

inline constexpr QRgba64 qRgba64(Rgba128Float rgba) noexcept
{
    return qRgba64(
            0xffffu * rgba.red(),
            0xffffu * rgba.green(),
            0xffffu * rgba.blue(),
            0xffffu * rgba.alpha());
}

// RgbaFloat16

inline constexpr Rgba128Float rgba128Float(Rgba64Float rgba) noexcept
{
    return { qRed(rgba), qGreen(rgba), qBlue(rgba), qAlpha(rgba) };
}

inline constexpr Rgba64Float rgba64Float(Rgba128Float rgba) noexcept
{
    using Float = HalfFloat;
    return { Float(qRed(rgba)), Float(qGreen(rgba)), Float(qBlue(rgba)), Float(qAlpha(rgba)) };
}

#endif // RGBA128FLOAT_H
