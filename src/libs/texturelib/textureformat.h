#ifndef TEXTUREFORMAT_H
#define TEXTUREFORMAT_H

#include <QtCore/QMetaType>

enum class TextureFormat {
    Invalid = 0,

    // 8bit
    A8Unorm,
    L8Unorm,

    R8Snorm,
    R8Unorm,
    R8Sint,
    R8Uint,

    // 16 bit
    LA8Unorm,

    R16Snorm,
    R16Unorm,
    R16Sint,
    R16Uint,
    R16F,

    RG8Snorm,
    RG8Unorm,
    RG8Sint,
    RG8Uint,

    // 24bit
    RGB8Unorm,
    BGR8Unorm,
    // 32bit
    RGBA8Unorm,
    BGRA8Unorm,
    ABGR8Unorm,
    RGBX8Unorm,
    BGRX8Unorm,
    // 64bit
    RGBA16Unorm,
    RGBA16F,

    // packed formats
    BGR565Unorm,
    RGB565Unorm,
    // RGBA4Unorm,
    BGRA4Unorm,
    // RGBX4Unorm,
    BGRX4Unorm,
    BGRA5551Unorm,
    BGRX5551Unorm,
    RGB332Unorm,

    // compressed
    Bc1RgbUnorm,
    Bc1RgbSrgb,
    Bc1RgbaUnorm,
    Bc1RgbaSrgb,
    Bc2Unorm,
    Bc2Srgb,
    Bc3Unorm,
    Bc3Srgb,
    Bc4Snorm,
    Bc4Unorm,
    Bc5Unorm,
    Bc5Snorm,
    Bc6HUF16,
    Bc6HSF16,
    Bc7Unorm,
    Bc7Srgb,

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

Q_DECLARE_METATYPE(TextureFormat)

QString toQString(TextureFormat format);

#endif // TEXTUREFORMAT_H
