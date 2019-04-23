#ifndef TEXTUREFORMAT_H
#define TEXTUREFORMAT_H

#include "texturelib_global.h"

#include <UtilsLib/ToStringHelpers>

#include <QtCore/QMetaType>
#include <QtCore/QStringView>

namespace Details {

TEXTURELIB_EXPORT Q_NAMESPACE

#ifdef DOXY
enum class TextureFormat {
#else
enum class TEXTURELIB_EXPORT TextureFormat: quint32 {
#endif
    Invalid = 0,

    // 8bit
    A8_Unorm,
    L8_Unorm,

    R8_Snorm,
    R8_Unorm,
    R8_Sint,
    R8_Uint,

    // 16 bit
    LA8_Unorm,

    R16_Snorm,
    R16_Unorm,
    R16_Sint,
    R16_Uint,
    R16_Float,

    RG8_Snorm,
    RG8_Unorm,
    RG8_Sint,
    RG8_Uint,

    // 24bit
    RGB8_Unorm,
    BGR8_Unorm,

    // 32bit
    R32_Sint,
    R32_Uint,
    R32_Float,

    RG16_Snorm,
    RG16_Unorm,
    RG16_Sint,
    RG16_Uint,
    RG16_Float,

    RGBA8_Snorm,
    RGBA8_Unorm,
    RGBA8_Sint,
    RGBA8_Uint,
    RGBA8_Srgb,

    BGRA8_Unorm,
    BGRA8_Srgb,
    ABGR8_Unorm,
    RGBX8_Unorm,
    BGRX8_Unorm,
    BGRX8_Srgb,

    // 64bit
    RGBA16_Snorm,
    RGBA16_Unorm,
    RGBA16_Sint,
    RGBA16_Uint,
    RGBA16_Float,

    RG32_Sint,
    RG32_Uint,
    RG32_Float,

    // 96bit
    RGB32_Sint,
    RGB32_Uint,
    RGB32_Float,

    // 128bit
    RGBA32_Sint,
    RGBA32_Uint,
    RGBA32_Float,

    // packed formats
    BGR565_Unorm,
    RGB565_Unorm,
    // RGBA4Unorm,
    BGRA4_Unorm,
    // RGBX4Unorm,
    BGRX4_Unorm,
    BGRA5551_Unorm,
    BGRX5551_Unorm,
    RGB332_Unorm,

    // compressed
    Bc1Rgb_Unorm,
    Bc1Rgb_Srgb,
    Bc1Rgba_Unorm,
    Bc1Rgba_Srgb,
    Bc2_Unorm,
    Bc2_Srgb,
    Bc3_Unorm,
    Bc3_Srgb,
    Bc4_Snorm,
    Bc4_Unorm,
    Bc5_Unorm,
    Bc5_Snorm,
    Bc6HUF16,
    Bc6HSF16,
    Bc7_Unorm,
    Bc7_Srgb,

    RXGB,
    RG_ATI2N_UNorm,
    RGB8_ETC1,
    RGB8_ETC2,
    RGBA8_ETC2_EAC,
    RGB8_PunchThrough_Alpha1_ETC2,
    R11_EAC_UNorm,
    RG11_EAC_UNorm,
    R11_EAC_SNorm,
    RG11_EAC_SNorm,

    FormatsCount // should be the last
};

Q_ENUM_NS(TextureFormat)

} // namespace Details

using TextureFormat = Details::TextureFormat;

#endif // TEXTUREFORMAT_H
