#ifndef RGBA128FLOAT_H
#define RGBA128FLOAT_H

#include <TextureLib/Rgba32Signed>
#include <TextureLib/Rgba64Float>
#include <TextureLib/RgbaGeneric>

#include <QtGui/QRgb>
#include <QtGui/QRgba64>

using Rgba128Float = RgbaGeneric<float>;

inline constexpr Rgba128Float rgba128Float(float r, float g, float b, float a = 1.0f) noexcept
{
    return Private::createRgba<float>(r, g, b, a);
}

template<typename T>
inline constexpr Rgba128Float rgba128Float(T rgba) noexcept
{
    return Private::convertRgba<float, typename Private::RgbaTraits<T>::DataType>(rgba);
}

using Rgba128Signed = RgbaGeneric<qint32>;

inline constexpr Rgba128Signed rgba128Signed(qint32 r, qint32 g, qint32 b, qint32 a = 0x7fffffff) noexcept
{
    return Private::createRgba<qint32>(r, g, b, a);
}

template<typename T>
inline constexpr Rgba128Signed rgba128Signed(T rgba) noexcept
{
    return Private::convertRgba<qint32, typename Private::RgbaTraits<T>::DataType>(rgba);
}

using Rgba128 = RgbaGeneric<quint32>;

inline constexpr Rgba128 rgba128(quint32 r, quint32 g, quint32 b, quint32 a = 0xffffffff) noexcept
{
    return Private::createRgba<quint32>(r, g, b, a);
}

template<typename T>
inline constexpr Rgba128 rgba128(T rgba) noexcept
{
    return Private::convertRgba<quint32, typename Private::RgbaTraits<T>::DataType>(rgba);
}

#endif // RGBA128FLOAT_H
