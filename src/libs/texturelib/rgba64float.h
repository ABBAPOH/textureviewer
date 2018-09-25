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
    return Private::createRgba<HalfFloat>(r, g, b, a);
}

// Generic

template<typename T>
inline constexpr Rgba64Float rgba64Float(T rgba) noexcept
{
    return Private::convertRgba<HalfFloat, typename Private::RgbaTraits<T>::DataType>(rgba);
}

#endif // RGBA64FLOAT_H
