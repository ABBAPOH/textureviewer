#ifndef RGBA64FLOAT_H
#define RGBA64FLOAT_H

#include <TextureLib/Rgba32Signed>
#include <TextureLib/RgbaGeneric>

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

using Rgba64Float = RgbaGeneric<HalfFloat>;

inline constexpr Rgba64Float rgba64Float(HalfFloat r, HalfFloat g, HalfFloat b, HalfFloat a = HalfFloat(1.0)) noexcept
{
    return Private::createRgba<HalfFloat>(r, g, b, a);
}

template<typename T>
inline constexpr Rgba64Float rgba64Float(T rgba) noexcept
{
    return Private::convertRgba<HalfFloat, typename Private::RgbaTraits<T>::DataType>(rgba);
}

#endif // RGBA64FLOAT_H
