#include "textureformatinfo.h"

#include <QtCore/QMetaEnum>

constexpr TextureFormatInfo formats[] = {
    {},

    // 8bit
    { TextureFormat::A8_Unorm, 8, 0, QOpenGLTexture::AlphaFormat,     QOpenGLTexture::Alpha,     QOpenGLTexture::UInt8 },
    { TextureFormat::L8_Unorm, 8, 0, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, QOpenGLTexture::UInt8 },

    { TextureFormat::R8_Snorm, 8, 0, QOpenGLTexture::R8_SNorm, QOpenGLTexture::Red,         QOpenGLTexture::Int8  },
    { TextureFormat::R8_Unorm, 8, 0, QOpenGLTexture::R8_UNorm, QOpenGLTexture::Red,         QOpenGLTexture::UInt8 },
    { TextureFormat::R8_Sint,  8, 0, QOpenGLTexture::R8I,      QOpenGLTexture::Red_Integer, QOpenGLTexture::Int8  },
    { TextureFormat::R8_Uint,  8, 0, QOpenGLTexture::R8U,      QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt8 },

    // 16bit
    { TextureFormat::LA8_Unorm, 16, 0, QOpenGLTexture::LuminanceAlphaFormat, QOpenGLTexture::LuminanceAlpha, QOpenGLTexture::UInt8 },

    { TextureFormat::R16_Snorm, 16, 0, QOpenGLTexture::R16_SNorm, QOpenGLTexture::Red,         QOpenGLTexture::Int16   },
    { TextureFormat::R16_Unorm, 16, 0, QOpenGLTexture::R16_UNorm, QOpenGLTexture::Red,         QOpenGLTexture::UInt16  },
    { TextureFormat::R16_Sint,  16, 0, QOpenGLTexture::R16I,      QOpenGLTexture::Red_Integer, QOpenGLTexture::Int16   },
    { TextureFormat::R16_Uint,  16, 0, QOpenGLTexture::R16U,      QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt16  },
    { TextureFormat::R16_Float, 16, 0, QOpenGLTexture::R16F,      QOpenGLTexture::Red,         QOpenGLTexture::Float16 },

    { TextureFormat::RG8_Snorm, 16, 0, QOpenGLTexture::RG8_SNorm, QOpenGLTexture::RG,         QOpenGLTexture::Int8  },
    { TextureFormat::RG8_Unorm, 16, 0, QOpenGLTexture::RG8_UNorm, QOpenGLTexture::RG,         QOpenGLTexture::UInt8 },
    { TextureFormat::RG8_Sint,  16, 0, QOpenGLTexture::RG16I,     QOpenGLTexture::RG_Integer, QOpenGLTexture::Int8  },
    { TextureFormat::RG8_Uint,  16, 0, QOpenGLTexture::RG16U,     QOpenGLTexture::RG_Integer, QOpenGLTexture::UInt8 },

    // 24bit
    { TextureFormat::RGB8_Unorm, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGB, QOpenGLTexture::UInt8 },
    { TextureFormat::BGR8_Unorm, 24, 0, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGR, QOpenGLTexture::UInt8 },

    // 32bit
    { TextureFormat::R32_Sint,  32, 0, QOpenGLTexture::R32I, QOpenGLTexture::Red_Integer, QOpenGLTexture::Int32   },
    { TextureFormat::R32_Uint,  32, 0, QOpenGLTexture::R32U, QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt32  },
    { TextureFormat::R32_Float, 32, 0, QOpenGLTexture::R32F, QOpenGLTexture::Red,         QOpenGLTexture::Float32 },

    { TextureFormat::RG16_Snorm, 32, 0, QOpenGLTexture::RG16_SNorm, QOpenGLTexture::RG,         QOpenGLTexture::Int16   },
    { TextureFormat::RG16_Unorm, 32, 0, QOpenGLTexture::RG16_UNorm, QOpenGLTexture::RG,         QOpenGLTexture::UInt16  },
    { TextureFormat::RG16_Sint,  32, 0, QOpenGLTexture::RG16I,      QOpenGLTexture::RG_Integer, QOpenGLTexture::Int16   },
    { TextureFormat::RG16_Uint,  32, 0, QOpenGLTexture::RG16U,      QOpenGLTexture::RG_Integer, QOpenGLTexture::UInt16  },
    { TextureFormat::RG16_Float, 32, 0, QOpenGLTexture::RG16F,      QOpenGLTexture::RG,         QOpenGLTexture::Float16 },

    { TextureFormat::RGBA8_Snorm, 32, 0, QOpenGLTexture::RGBA8_SNorm,  QOpenGLTexture::RGBA, QOpenGLTexture::Int8  },
    { TextureFormat::RGBA8_Unorm, 32, 0, QOpenGLTexture::RGBA8_UNorm,  QOpenGLTexture::RGBA, QOpenGLTexture::UInt8 },
    { TextureFormat::RGBA8_Sint,  32, 0, QOpenGLTexture::RGBA8I,       QOpenGLTexture::RGBA, QOpenGLTexture::Int8  },
    { TextureFormat::RGBA8_Uint,  32, 0, QOpenGLTexture::RGBA8U,       QOpenGLTexture::RGBA, QOpenGLTexture::UInt8 },
    { TextureFormat::RGBA8_Srgb,  32, 0, QOpenGLTexture::SRGB8_Alpha8, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8 },

    { TextureFormat::BGRA8_Unorm, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8        },
    { TextureFormat::BGRA8_Srgb,  32, 0, QOpenGLTexture::SRGB8,       QOpenGLTexture::BGRA, QOpenGLTexture::UInt8        },
    { TextureFormat::ABGR8_Unorm, 32, 0, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGBA8 },
    { TextureFormat::RGBX8_Unorm, 32, 0, QOpenGLTexture::RGB8_UNorm,  QOpenGLTexture::RGBA, QOpenGLTexture::UInt8        },
    { TextureFormat::BGRX8_Unorm, 32, 0, QOpenGLTexture::RGB8_UNorm,  QOpenGLTexture::BGRA, QOpenGLTexture::UInt8        },
    { TextureFormat::BGRX8_Srgb,  32, 0, QOpenGLTexture::SRGB8,       QOpenGLTexture::BGRA, QOpenGLTexture::UInt8        },

    // 64bit
    { TextureFormat::RGBA16_Snorm, 64, 0, QOpenGLTexture::RGBA16_SNorm, QOpenGLTexture::RGBA, QOpenGLTexture::Int16   },
    { TextureFormat::RGBA16_Unorm, 64, 0, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt16  },
    { TextureFormat::RGBA16_Sint,  64, 0, QOpenGLTexture::RGBA16I,      QOpenGLTexture::RGBA, QOpenGLTexture::Int16   },
    { TextureFormat::RGBA16_Uint,  64, 0, QOpenGLTexture::RGBA16U,      QOpenGLTexture::RGBA, QOpenGLTexture::UInt16  },
    { TextureFormat::RGBA16_Float, 64, 0, QOpenGLTexture::RGBA16F,      QOpenGLTexture::RGBA, QOpenGLTexture::Float16 },

    { TextureFormat::RG32_Sint,  64, 0, QOpenGLTexture::RG32I, QOpenGLTexture::RG_Integer, QOpenGLTexture::Int32   },
    { TextureFormat::RG32_Uint,  64, 0, QOpenGLTexture::RG32U, QOpenGLTexture::RG_Integer, QOpenGLTexture::UInt32  },
    { TextureFormat::RG32_Float, 64, 0, QOpenGLTexture::RG32F, QOpenGLTexture::RG,         QOpenGLTexture::Float32 },

    // 96bit
    { TextureFormat::RGB32_Sint,  96, 0, QOpenGLTexture::RGB32I, QOpenGLTexture::RGB_Integer, QOpenGLTexture::Int32   },
    { TextureFormat::RGB32_Uint,  96, 0, QOpenGLTexture::RGB32U, QOpenGLTexture::RGB_Integer, QOpenGLTexture::UInt32  },
    { TextureFormat::RGB32_Float, 96, 0, QOpenGLTexture::RGB32F, QOpenGLTexture::RGB,         QOpenGLTexture::Float32 },

    // 128bit
    { TextureFormat::RGBA32_Sint,  128, 0, QOpenGLTexture::RGBA32I, QOpenGLTexture::RGBA_Integer, QOpenGLTexture::Int32   },
    { TextureFormat::RGBA32_Uint,  128, 0, QOpenGLTexture::RGBA32U, QOpenGLTexture::RGBA_Integer, QOpenGLTexture::UInt32  },
    { TextureFormat::RGBA32_Float, 128, 0, QOpenGLTexture::RGBA32F, QOpenGLTexture::RGBA,         QOpenGLTexture::Float32 },

    // packed formats
    { TextureFormat::BGR565_Unorm,   16, 0, QOpenGLTexture::R5G6B5,     QOpenGLTexture::RGB,  QOpenGLTexture::UInt16_R5G6B5     },
    { TextureFormat::RGB565_Unorm,   16, 0, QOpenGLTexture::R5G6B5,     QOpenGLTexture::RGB,  QOpenGLTexture::UInt16_R5G6B5_Rev },
    { TextureFormat::BGRA4_Unorm,    16, 0, QOpenGLTexture::RGBA4,      QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev  },
    { TextureFormat::BGRX4_Unorm,    16, 0, QOpenGLTexture::RGBFormat,  QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev  },
    { TextureFormat::BGRA5551_Unorm, 16, 0, QOpenGLTexture::RGBAFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev },
    { TextureFormat::BGRX5551_Unorm, 16, 0, QOpenGLTexture::RGBFormat,  QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev },
    { TextureFormat::RGB332_Unorm,    8, 0, QOpenGLTexture::RG3B2,      QOpenGLTexture::RGB,  QOpenGLTexture::UInt8_RG3B2       },
    // compressed
    { TextureFormat::Bc1Rgb_Unorm,  0,  8, QOpenGLTexture::RGB_DXT1              },
    { TextureFormat::Bc1Rgb_Srgb,   0,  8, QOpenGLTexture::SRGB_DXT1             },
    { TextureFormat::Bc1Rgba_Unorm, 0,  8, QOpenGLTexture::RGBA_DXT1             },
    { TextureFormat::Bc1Rgba_Srgb,  0,  8, QOpenGLTexture::SRGB_Alpha_DXT1       },
    { TextureFormat::Bc2_Unorm,     0, 16, QOpenGLTexture::RGBA_DXT3             },
    { TextureFormat::Bc2_Srgb,      0, 16, QOpenGLTexture::SRGB_Alpha_DXT3       },
    { TextureFormat::Bc3_Unorm,     0, 16, QOpenGLTexture::RGBA_DXT5             },
    { TextureFormat::Bc3_Srgb,      0, 16, QOpenGLTexture::SRGB_Alpha_DXT5       },
    { TextureFormat::Bc4_Snorm,     0,  8, QOpenGLTexture::R_ATI1N_SNorm         },
    { TextureFormat::Bc4_Unorm,     0,  8, QOpenGLTexture::R_ATI1N_UNorm         },
    { TextureFormat::Bc5_Unorm,     0, 16, QOpenGLTexture::RG_ATI2N_UNorm        },
    { TextureFormat::Bc5_Snorm,     0, 16, QOpenGLTexture::RG_ATI2N_SNorm        },
    { TextureFormat::Bc6HUF16,      0, 16, QOpenGLTexture::RGB_BP_UNSIGNED_FLOAT },
    { TextureFormat::Bc6HSF16,      0, 16, QOpenGLTexture::RGB_BP_SIGNED_FLOAT   },
    { TextureFormat::Bc7_Unorm,     0, 16, QOpenGLTexture::RGB_BP_UNorm          },
    { TextureFormat::Bc7_Srgb,      0, 16, QOpenGLTexture::SRGB_BP_UNorm         },

    {TextureFormat::RXGB, 0, 16, QOpenGLTexture::RGBA_DXT5},
    {TextureFormat::RG_ATI2N_UNorm, 0, 16, QOpenGLTexture::RG_ATI2N_UNorm},
    {TextureFormat::RGB8_ETC1, 0, 8, QOpenGLTexture::RGB8_ETC1},
    {TextureFormat::RGB8_ETC2, 0, 8, QOpenGLTexture::RGB8_ETC2},
    {TextureFormat::RGBA8_ETC2_EAC, 0, 16, QOpenGLTexture::RGBA8_ETC2_EAC},
    {TextureFormat::RGB8_PunchThrough_Alpha1_ETC2, 0, 8, QOpenGLTexture::RGB8_PunchThrough_Alpha1_ETC2},
    {TextureFormat::R11_EAC_UNorm, 0, 8, QOpenGLTexture::R11_EAC_UNorm},
    {TextureFormat::RG11_EAC_UNorm, 0, 16, QOpenGLTexture::RG11_EAC_UNorm},
    {TextureFormat::R11_EAC_SNorm, 0, 8, QOpenGLTexture::R11_EAC_SNorm},
    {TextureFormat::RG11_EAC_SNorm, 0, 16, QOpenGLTexture::RG11_EAC_SNorm},
};

static_assert (sizeof (formats) == sizeof(TextureFormatInfo) * size_t(TextureFormat::FormatsCount),
               "Some TextureFormat eniumerations are not handled in an array");

/*!
  \internal
    Formats in the table should be on the right position
*/
constexpr bool checkFormatPositions()
{
    int position = 0;
    // Use a variable to compile with msvc. It can't build because rvalue is not constexpr
    const auto array = TextureFormatInfo::TextureFormatInfos(formats);
    for (const auto &format: array) {
        if (format.format() != TextureFormat(position++))
            return false;
    }
    return true;
}

static_assert (checkFormatPositions(), "Incorrect format position in formats array");

const TextureFormatInfo &TextureFormatInfo::formatInfo(TextureFormat format) noexcept
{
    return TextureFormatInfos(formats).at(size_t(format));
}

TextureFormatInfo::TextureFormatInfos TextureFormatInfo::allFormatInfos() noexcept
{
    return {formats};
}

QString toQString(TextureFormat format)
{
    const auto en = QMetaEnum::fromType<Details::TextureFormat>();
    Q_ASSERT(en.isValid());
    return en.valueToKey(int(format));
}
