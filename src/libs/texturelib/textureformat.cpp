#include "textureformat.h"

/*!
  \namespace Details
  Helper namespace with a Qt Meta Object.
*/

/*!
  \enum Details::TextureFormat

  This enum describes the format of the Texture.

  \var Details::TextureFormat Details::Invalid
  Invalid format.
  \var Details::TextureFormat Details::A8_Unorm
  A8_Unorm format
  \var Details::TextureFormat Details::L8_Unorm
  L8_Unorm format
  \var Details::TextureFormat Details::R8_Snorm
  R8_Snorm format
  \var Details::TextureFormat Details::R8_Unorm
  R8_Unorm format
  \var Details::TextureFormat Details::R8_Sint
  R8_Sint format
  \var Details::TextureFormat Details::R8_Uint
  R8_Uint format
  \var Details::TextureFormat Details::LA8_Unorm
  LA8_Unorm format
  \var Details::TextureFormat Details::R16_Snorm
  R16_Snorm format
  \var Details::TextureFormat Details::R16_Unorm
  R16_Unorm format
  \var Details::TextureFormat Details::R16_Sint
  R16_Sint format
  \var Details::TextureFormat Details::R16_Uint
  R16_Uint format
  \var Details::TextureFormat Details::R16_Float
  R16_Float format
  \var Details::TextureFormat Details::RG8_Snorm
  RG8_Snorm format
  \var Details::TextureFormat Details::RG8_Unorm
  RG8_Unorm format
  \var Details::TextureFormat Details::RG8_Sint
  RG8_Sint format
  \var Details::TextureFormat Details::RG8_Uint
  RG8_Uint format
  \var Details::TextureFormat Details::RGB8_Unorm
  RGB8_Unorm format
  \var Details::TextureFormat Details::BGR8_Unorm
  BGR8_Unorm format
  \var Details::TextureFormat Details::R32_Sint
  R32_Sint format
  \var Details::TextureFormat Details::R32_Uint
  R32_Uint format
  \var Details::TextureFormat Details::R32_Float
  R32_Float format
  \var Details::TextureFormat Details::RG16_Snorm
  RG16_Snorm format
  \var Details::TextureFormat Details::RG16_Unorm
  RG16_Unorm format
  \var Details::TextureFormat Details::RG16_Sint
  RG16_Sint format
  \var Details::TextureFormat Details::RG16_Uint
  RG16_Uint format
  \var Details::TextureFormat Details::RG16_Float
  RG16_Float format
  \var Details::TextureFormat Details::RGBA8_Snorm
  RGBA8_Snorm format
  \var Details::TextureFormat Details::RGBA8_Unorm
  RGBA8_Unorm format
  \var Details::TextureFormat Details::RGBA8_Sint
  RGBA8_Sint format
  \var Details::TextureFormat Details::RGBA8_Uint
  RGBA8_Uint format
  \var Details::TextureFormat Details::RGBA8_Srgb
  RGBA8_Srgb format
  \var Details::TextureFormat Details::BGRA8_Unorm
  BGRA8_Unorm format
  \var Details::TextureFormat Details::BGRA8_Srgb
  BGRA8_Srgb format
  \var Details::TextureFormat Details::ABGR8_Unorm
  ABGR8_Unorm format
  \var Details::TextureFormat Details::RGBX8_Unorm
  RGBX8_Unorm format
  \var Details::TextureFormat Details::BGRX8_Unorm
  BGRX8_Unorm format
  \var Details::TextureFormat Details::BGRX8_Srgb
  BGRX8_Srgb format
  \var Details::TextureFormat Details::RGBA16_Snorm
  RGBA16_Snorm format
  \var Details::TextureFormat Details::RGBA16_Unorm
  RGBA16_Unorm format
  \var Details::TextureFormat Details::RGBA16_Sint
  RGBA16_Sint format
  \var Details::TextureFormat Details::RGBA16_Uint
  RGBA16_Uint format
  \var Details::TextureFormat Details::RGBA16_Float
  RGBA16_Float format
  \var Details::TextureFormat Details::RG32_Sint
  RG32_Sint format
  \var Details::TextureFormat Details::RG32_Uint
  RG32_Uint format
  \var Details::TextureFormat Details::RG32_Float
  RG32_Float format
  \var Details::TextureFormat Details::RGB32_Sint
  RGB32_Sint format
  \var Details::TextureFormat Details::RGB32_Uint
  RGB32_Uint format
  \var Details::TextureFormat Details::RGB32_Float
  RGB32_Float format
  \var Details::TextureFormat Details::RGBA32_Sint
  RGBA32_Sint format
  \var Details::TextureFormat Details::RGBA32_Uint
  RGBA32_Uint format
  \var Details::TextureFormat Details::RGBA32_Float
  RGBA32_Float format
  \var Details::TextureFormat Details::BGR565_Unorm
  BGR565_Unorm format
  \var Details::TextureFormat Details::RGB565_Unorm
  RGB565_Unorm format
  \var Details::TextureFormat Details::BGRA4_Unorm
  BGRA4_Unorm format
  \var Details::TextureFormat Details::BGRX4_Unorm
  BGRX4_Unorm format
  \var Details::TextureFormat Details::BGRA5551_Unorm
  BGRA5551_Unorm format
  \var Details::TextureFormat Details::BGRX5551_Unorm
  BGRX5551_Unorm format
  \var Details::TextureFormat Details::RGB332_Unorm
  RGB332_Unorm format
  \var Details::TextureFormat Details::Bc1Rgb_Unorm
  Bc1Rgb_Unorm compressed format
  \var Details::TextureFormat Details::Bc1Rgb_Srgb
  Bc1Rgb_Srgb compressed format
  \var Details::TextureFormat Details::Bc1Rgba_Unorm
  Bc1Rgba_Unorm compressed format
  \var Details::TextureFormat Details::Bc1Rgba_Srgb
  Bc1Rgba_Srgb compressed format
  \var Details::TextureFormat Details::Bc2_Unorm
  Bc2_Unorm compressed format
  \var Details::TextureFormat Details::Bc2_Srgb
  Bc2_Srgb compressed format
  \var Details::TextureFormat Details::Bc3_Unorm
  Bc3_Unorm compressed format
  \var Details::TextureFormat Details::Bc3_Srgb
  Bc3_Srgb compressed format
  \var Details::TextureFormat Details::Bc4_Snorm
  Bc4_Snorm compressed format
  \var Details::TextureFormat Details::Bc4_Unorm
  Bc4_Unorm compressed format
  \var Details::TextureFormat Details::Bc4_Unorm
  Bc4_Unorm compressed format
  \var Details::TextureFormat Details::Bc5_Snorm
  Bc5_Snorm compressed format
  \var Details::TextureFormat Details::Bc6HUF16
  Bc6HUF16 compressed format
  \var Details::TextureFormat Details::Bc6HSF16
  Bc6HSF16 compressed format
  \var Details::TextureFormat Details::Bc7_Unorm
  Bc7_Unorm compressed format
  \var Details::TextureFormat Details::Bc7_Srgb
  Bc7_Srgb compressed format
  \var Details::TextureFormat Details::RXGB
  RXGB compressed format
  \var Details::TextureFormat Details::RG_ATI2N_UNorm
  RG_ATI2N_UNorm compressed format
  \var Details::TextureFormat Details::RGB8_ETC1
  RGB8_ETC1 compressed format
  \var Details::TextureFormat Details::RGB8_ETC2
  RGB8_ETC2 compressed format
  \var Details::TextureFormat Details::RGBA8_ETC2_EAC
  RGBA8_ETC2_EAC compressed format
  \var Details::TextureFormat Details::RGB8_PunchThrough_Alpha1_ETC2
  RGB8_PunchThrough_Alpha1_ETC2 compressed format
  \var Details::TextureFormat Details::R11_EAC_UNorm
  R11_EAC_UNorm compressed format
  \var Details::TextureFormat Details::RG11_EAC_UNorm
  RG11_EAC_UNorm compressed format
  \var Details::TextureFormat Details::R11_EAC_SNorm
  R11_EAC_SNorm compressed format
  \var Details::TextureFormat Details::RG11_EAC_SNorm
  RG11_EAC_SNorm compressed format
  \var Details::TextureFormat Details::FormatsCount
  Total number of the format
*/
