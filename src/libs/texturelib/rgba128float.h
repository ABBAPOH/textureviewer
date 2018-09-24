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
    return Private::createRgba<float>(r, g, b, a);
}

// Generic

template<typename T>
inline constexpr Rgba128Float rgba128Float(T rgba) noexcept
{
    return Private::convertRgba<float, typename Private::RgbaChannelTypeHelper<T>::Type>(rgba);
}

// QRgba

inline constexpr QRgb qRgba(Rgba128Float rgba) noexcept
{
    return qRgba(
            0xffu * Private::bounded(rgba.red()),
            0xffu * Private::bounded(rgba.green()),
            0xffu * Private::bounded(rgba.blue()),
            0xffu * Private::bounded(rgba.alpha()));
}

// QRgba64

inline constexpr QRgba64 qRgba64(Rgba128Float rgba) noexcept
{
    return qRgba64(
            0xffffu * rgba.red(),
            0xffffu * rgba.green(),
            0xffffu * rgba.blue(),
            0xffffu * rgba.alpha());
}

#endif // RGBA128FLOAT_H
