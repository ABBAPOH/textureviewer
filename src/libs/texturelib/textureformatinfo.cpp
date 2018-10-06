#include "textureformatinfo.h"

#include <QtCore/QMetaEnum>

using Type = TextureFormatInfo::Type;

constexpr TextureFormatInfo formats[] = {
    {},

    // 8bit
    { TextureFormat::A8_Unorm, Type::UnsignedNormalized, 1, QOpenGLTexture::AlphaFormat,     QOpenGLTexture::Alpha,     QOpenGLTexture::UInt8 },
    { TextureFormat::L8_Unorm, Type::UnsignedNormalized, 1, QOpenGLTexture::LuminanceFormat, QOpenGLTexture::Luminance, QOpenGLTexture::UInt8 },

    { TextureFormat::R8_Snorm, Type::SignedNormalized,   1, QOpenGLTexture::R8_SNorm, QOpenGLTexture::Red,         QOpenGLTexture::Int8  },
    { TextureFormat::R8_Unorm, Type::UnsignedNormalized, 1, QOpenGLTexture::R8_UNorm, QOpenGLTexture::Red,         QOpenGLTexture::UInt8 },
    { TextureFormat::R8_Sint,  Type::SignedInteger,      1, QOpenGLTexture::R8I,      QOpenGLTexture::Red_Integer, QOpenGLTexture::Int8  },
    { TextureFormat::R8_Uint,  Type::UnsignedInteger,    1, QOpenGLTexture::R8U,      QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt8 },

    // 16bit
    { TextureFormat::LA8_Unorm, Type::UnsignedNormalized, 2, QOpenGLTexture::LuminanceAlphaFormat, QOpenGLTexture::LuminanceAlpha, QOpenGLTexture::UInt8 },

    { TextureFormat::R16_Snorm, Type::SignedNormalized,   2, QOpenGLTexture::R16_SNorm, QOpenGLTexture::Red,         QOpenGLTexture::Int16   },
    { TextureFormat::R16_Unorm, Type::UnsignedNormalized, 2, QOpenGLTexture::R16_UNorm, QOpenGLTexture::Red,         QOpenGLTexture::UInt16  },
    { TextureFormat::R16_Sint,  Type::SignedInteger,      2, QOpenGLTexture::R16I,      QOpenGLTexture::Red_Integer, QOpenGLTexture::Int16   },
    { TextureFormat::R16_Uint,  Type::UnsignedInteger,    2, QOpenGLTexture::R16U,      QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt16  },
    { TextureFormat::R16_Float, Type::Float,              2, QOpenGLTexture::R16F,      QOpenGLTexture::Red,         QOpenGLTexture::Float16 },

    { TextureFormat::RG8_Snorm, Type::SignedNormalized,   2, QOpenGLTexture::RG8_SNorm, QOpenGLTexture::RG,         QOpenGLTexture::Int8  },
    { TextureFormat::RG8_Unorm, Type::UnsignedNormalized, 2, QOpenGLTexture::RG8_UNorm, QOpenGLTexture::RG,         QOpenGLTexture::UInt8 },
    { TextureFormat::RG8_Sint,  Type::SignedInteger,      2, QOpenGLTexture::RG16I,     QOpenGLTexture::RG_Integer, QOpenGLTexture::Int8  },
    { TextureFormat::RG8_Uint,  Type::UnsignedInteger,    2, QOpenGLTexture::RG16U,     QOpenGLTexture::RG_Integer, QOpenGLTexture::UInt8 },

    // 24bit
    { TextureFormat::RGB8_Unorm, Type::UnsignedNormalized, 3, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::RGB, QOpenGLTexture::UInt8 },
    { TextureFormat::BGR8_Unorm, Type::UnsignedNormalized, 3, QOpenGLTexture::RGB8_UNorm, QOpenGLTexture::BGR, QOpenGLTexture::UInt8 },

    // 32bit
    { TextureFormat::R32_Sint,  Type::SignedInteger,    4, QOpenGLTexture::R32I, QOpenGLTexture::Red_Integer, QOpenGLTexture::Int32   },
    { TextureFormat::R32_Uint,  Type::UnsignedInteger,  4, QOpenGLTexture::R32U, QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt32  },
    { TextureFormat::R32_Float, Type::Float,            4, QOpenGLTexture::R32F, QOpenGLTexture::Red,         QOpenGLTexture::Float32 },

    { TextureFormat::RG16_Snorm, Type::SignedNormalized,   4, QOpenGLTexture::RG16_SNorm, QOpenGLTexture::RG,         QOpenGLTexture::Int16   },
    { TextureFormat::RG16_Unorm, Type::UnsignedNormalized, 4, QOpenGLTexture::RG16_UNorm, QOpenGLTexture::RG,         QOpenGLTexture::UInt16  },
    { TextureFormat::RG16_Sint,  Type::SignedInteger,      4, QOpenGLTexture::RG16I,      QOpenGLTexture::RG_Integer, QOpenGLTexture::Int16   },
    { TextureFormat::RG16_Uint,  Type::UnsignedInteger,    4, QOpenGLTexture::RG16U,      QOpenGLTexture::RG_Integer, QOpenGLTexture::UInt16  },
    { TextureFormat::RG16_Float, Type::Float,              4, QOpenGLTexture::RG16F,      QOpenGLTexture::RG,         QOpenGLTexture::Float16 },

    { TextureFormat::RGBA8_Snorm, Type::SignedNormalized,   4, QOpenGLTexture::RGBA8_SNorm,  QOpenGLTexture::RGBA,         QOpenGLTexture::Int8  },
    { TextureFormat::RGBA8_Unorm, Type::UnsignedNormalized, 4, QOpenGLTexture::RGBA8_UNorm,  QOpenGLTexture::RGBA,         QOpenGLTexture::UInt8 },
    { TextureFormat::RGBA8_Sint,  Type::SignedInteger,      4, QOpenGLTexture::RGBA8I,       QOpenGLTexture::RGBA_Integer, QOpenGLTexture::Int8  },
    { TextureFormat::RGBA8_Uint,  Type::UnsignedInteger,    4, QOpenGLTexture::RGBA8U,       QOpenGLTexture::RGBA_Integer, QOpenGLTexture::UInt8 },
    { TextureFormat::RGBA8_Srgb,  Type::Srgb,               4, QOpenGLTexture::SRGB8_Alpha8, QOpenGLTexture::RGBA,         QOpenGLTexture::UInt8 },

    { TextureFormat::BGRA8_Unorm, Type::UnsignedNormalized, 4, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::BGRA, QOpenGLTexture::UInt8        },
    { TextureFormat::BGRA8_Srgb,  Type::Srgb,               4, QOpenGLTexture::SRGB8,       QOpenGLTexture::BGRA, QOpenGLTexture::UInt8        },
    { TextureFormat::ABGR8_Unorm, Type::UnsignedNormalized, 4, QOpenGLTexture::RGBA8_UNorm, QOpenGLTexture::RGBA, QOpenGLTexture::UInt32_RGBA8 },
    { TextureFormat::RGBX8_Unorm, Type::UnsignedNormalized, 4, QOpenGLTexture::RGB8_UNorm,  QOpenGLTexture::RGBA, QOpenGLTexture::UInt8        },
    { TextureFormat::BGRX8_Unorm, Type::UnsignedNormalized, 4, QOpenGLTexture::RGB8_UNorm,  QOpenGLTexture::BGRA, QOpenGLTexture::UInt8        },
    { TextureFormat::BGRX8_Srgb,  Type::Srgb,               4, QOpenGLTexture::SRGB8,       QOpenGLTexture::BGRA, QOpenGLTexture::UInt8        },

    // 64bit
    { TextureFormat::RGBA16_Snorm, Type::SignedNormalized,   8, QOpenGLTexture::RGBA16_SNorm, QOpenGLTexture::RGBA,         QOpenGLTexture::Int16   },
    { TextureFormat::RGBA16_Unorm, Type::UnsignedNormalized, 8, QOpenGLTexture::RGBA16_UNorm, QOpenGLTexture::RGBA,         QOpenGLTexture::UInt16  },
    { TextureFormat::RGBA16_Sint,  Type::SignedInteger,      8, QOpenGLTexture::RGBA16I,      QOpenGLTexture::RGBA_Integer, QOpenGLTexture::Int16   },
    { TextureFormat::RGBA16_Uint,  Type::UnsignedInteger,    8, QOpenGLTexture::RGBA16U,      QOpenGLTexture::RGBA_Integer, QOpenGLTexture::UInt16  },
    { TextureFormat::RGBA16_Float, Type::Float,              8, QOpenGLTexture::RGBA16F,      QOpenGLTexture::RGBA,         QOpenGLTexture::Float16 },

    { TextureFormat::RG32_Sint,  Type::SignedInteger,    8, QOpenGLTexture::RG32I, QOpenGLTexture::RG_Integer, QOpenGLTexture::Int32   },
    { TextureFormat::RG32_Uint,  Type::UnsignedInteger,  8, QOpenGLTexture::RG32U, QOpenGLTexture::RG_Integer, QOpenGLTexture::UInt32  },
    { TextureFormat::RG32_Float, Type::Float,            8, QOpenGLTexture::RG32F, QOpenGLTexture::RG,         QOpenGLTexture::Float32 },

    // 96bit
    { TextureFormat::RGB32_Sint,  Type::SignedInteger,    12, QOpenGLTexture::RGB32I, QOpenGLTexture::RGB_Integer, QOpenGLTexture::Int32   },
    { TextureFormat::RGB32_Uint,  Type::UnsignedInteger,  12, QOpenGLTexture::RGB32U, QOpenGLTexture::RGB_Integer, QOpenGLTexture::UInt32  },
    { TextureFormat::RGB32_Float, Type::Float,            12, QOpenGLTexture::RGB32F, QOpenGLTexture::RGB,         QOpenGLTexture::Float32 },

    // 128bit
    { TextureFormat::RGBA32_Sint,  Type::SignedInteger,    16, QOpenGLTexture::RGBA32I, QOpenGLTexture::RGBA_Integer, QOpenGLTexture::Int32   },
    { TextureFormat::RGBA32_Uint,  Type::UnsignedInteger,  16, QOpenGLTexture::RGBA32U, QOpenGLTexture::RGBA_Integer, QOpenGLTexture::UInt32  },
    { TextureFormat::RGBA32_Float, Type::Float,            16, QOpenGLTexture::RGBA32F, QOpenGLTexture::RGBA,         QOpenGLTexture::Float32 },

    // packed formats
    { TextureFormat::BGR565_Unorm,   Type::UnsignedNormalized, 2, QOpenGLTexture::R5G6B5,     QOpenGLTexture::RGB,  QOpenGLTexture::UInt16_R5G6B5     },
    { TextureFormat::RGB565_Unorm,   Type::UnsignedNormalized, 2, QOpenGLTexture::R5G6B5,     QOpenGLTexture::RGB,  QOpenGLTexture::UInt16_R5G6B5_Rev },
    { TextureFormat::BGRA4_Unorm,    Type::UnsignedNormalized, 2, QOpenGLTexture::RGBA4,      QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev  },
    { TextureFormat::BGRX4_Unorm,    Type::UnsignedNormalized, 2, QOpenGLTexture::RGBFormat,  QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGBA4_Rev  },
    { TextureFormat::BGRA5551_Unorm, Type::UnsignedNormalized, 2, QOpenGLTexture::RGBAFormat, QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev },
    { TextureFormat::BGRX5551_Unorm, Type::UnsignedNormalized, 2, QOpenGLTexture::RGBFormat,  QOpenGLTexture::BGRA, QOpenGLTexture::UInt16_RGB5A1_Rev },
    { TextureFormat::RGB332_Unorm,   Type::UnsignedNormalized, 1, QOpenGLTexture::RG3B2,      QOpenGLTexture::RGB,  QOpenGLTexture::UInt8_RG3B2       },
    // compressed
    { TextureFormat::Bc1Rgb_Unorm,  Type::Compressed,  8, QOpenGLTexture::RGB_DXT1              },
    { TextureFormat::Bc1Rgb_Srgb,   Type::Compressed,  8, QOpenGLTexture::SRGB_DXT1             },
    { TextureFormat::Bc1Rgba_Unorm, Type::Compressed,  8, QOpenGLTexture::RGBA_DXT1             },
    { TextureFormat::Bc1Rgba_Srgb,  Type::Compressed,  8, QOpenGLTexture::SRGB_Alpha_DXT1       },
    { TextureFormat::Bc2_Unorm,     Type::Compressed, 16, QOpenGLTexture::RGBA_DXT3             },
    { TextureFormat::Bc2_Srgb,      Type::Compressed, 16, QOpenGLTexture::SRGB_Alpha_DXT3       },
    { TextureFormat::Bc3_Unorm,     Type::Compressed, 16, QOpenGLTexture::RGBA_DXT5             },
    { TextureFormat::Bc3_Srgb,      Type::Compressed, 16, QOpenGLTexture::SRGB_Alpha_DXT5       },
    { TextureFormat::Bc4_Snorm,     Type::Compressed,  8, QOpenGLTexture::R_ATI1N_SNorm         },
    { TextureFormat::Bc4_Unorm,     Type::Compressed,  8, QOpenGLTexture::R_ATI1N_UNorm         },
    { TextureFormat::Bc5_Unorm,     Type::Compressed, 16, QOpenGLTexture::RG_ATI2N_UNorm        },
    { TextureFormat::Bc5_Snorm,     Type::Compressed, 16, QOpenGLTexture::RG_ATI2N_SNorm        },
    { TextureFormat::Bc6HUF16,      Type::Compressed, 16, QOpenGLTexture::RGB_BP_UNSIGNED_FLOAT },
    { TextureFormat::Bc6HSF16,      Type::Compressed, 16, QOpenGLTexture::RGB_BP_SIGNED_FLOAT   },
    { TextureFormat::Bc7_Unorm,     Type::Compressed, 16, QOpenGLTexture::RGB_BP_UNorm          },
    { TextureFormat::Bc7_Srgb,      Type::Compressed, 16, QOpenGLTexture::SRGB_BP_UNorm         },

    {TextureFormat::RXGB, Type::Compressed, 16, QOpenGLTexture::RGBA_DXT5},
    {TextureFormat::RG_ATI2N_UNorm, Type::Compressed, 16, QOpenGLTexture::RG_ATI2N_UNorm},
    {TextureFormat::RGB8_ETC1, Type::Compressed, 8, QOpenGLTexture::RGB8_ETC1},
    {TextureFormat::RGB8_ETC2, Type::Compressed, 8, QOpenGLTexture::RGB8_ETC2},
    {TextureFormat::RGBA8_ETC2_EAC, Type::Compressed, 16, QOpenGLTexture::RGBA8_ETC2_EAC},
    {TextureFormat::RGB8_PunchThrough_Alpha1_ETC2, Type::Compressed, 8, QOpenGLTexture::RGB8_PunchThrough_Alpha1_ETC2},
    {TextureFormat::R11_EAC_UNorm, Type::Compressed, 8, QOpenGLTexture::R11_EAC_UNorm},
    {TextureFormat::RG11_EAC_UNorm, Type::Compressed, 16, QOpenGLTexture::RG11_EAC_UNorm},
    {TextureFormat::R11_EAC_SNorm, Type::Compressed, 8, QOpenGLTexture::R11_EAC_SNorm},
    {TextureFormat::RG11_EAC_SNorm, Type::Compressed, 16, QOpenGLTexture::RG11_EAC_SNorm},
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

/*!
  \class TextureFormatInfo
  This class describes the information about a TextureFormat
*/

/*!
  \typedef TextureFormatInfo::TextureFormatInfos

  Type alias for the gsl::span\<const TextureFormatInfo\>
*/

/*!
  \fn TextureFormatInfo::TextureFormatInfo() noexcept

  \brief Constructs a default TextureFormatInfo object.

  All members are filled with zeros.
*/

/*!
  \fn TextureFormatInfo::TextureFormatInfo(
    TextureFormat format, int bytesPerTexel, int blockSize,
    QOpenGLTexture::TextureFormat oglTextureFormat,
    QOpenGLTexture::PixelFormat oglPixelFormat,
    QOpenGLTexture::PixelType oglPixelType) noexcept

  \brief Constructs a TextureFormatInfo object with the given paramters.
*/

/*!
  \property TextureFormat TextureFormatInfo::format
  \brief This property holds a texture format.
*/

/*!
  \property TextureFormatInfo::Type TextureFormatInfo::type
  \brief This property holds the texture format type.
*/

/*!
  \property int TextureFormatInfo::size
  \brief This property holds a number of bytes per texel or block, depending on type.
*/

/*!
  \property int TextureFormatInfo::bytesPerTexel
  \brief This property holds a nubmer of bytes per texel.

  For compressed textures, this property is always 0.
*/

/*!
  \property int TextureFormatInfo::blockSize
  \brief This property holds a block size.

  For uncompressed textures, this property is always 0.
*/

/*!
  \property QOpenGLTexture::PixelFormat TextureFormatInfo::oglTextureFormat
  \brief This property holds an OpenGL texture format.
*/

/*!
  \property QOpenGLTexture::TextureFormat TextureFormatInfo::oglPixelFormat
  \brief This property holds an OpenGL pixel format.
*/

/*!
  \property QOpenGLTexture::PixelType TextureFormatInfo::oglPixelType
  \brief This property holds an OpenGL pixel type.
*/

/*!
  \brief Returns a TextureFormatInfo for the given \a format.
*/
const TextureFormatInfo &TextureFormatInfo::formatInfo(TextureFormat format) noexcept
{
    return TextureFormatInfos(formats).at(size_t(format));
}

/*!
  \brief Returns a TextureFormatInfos for all formats.
*/
TextureFormatInfo::TextureFormatInfos TextureFormatInfo::allFormatInfos() noexcept
{
    return {formats};
}

const TextureFormatInfo &findOGLFormatLinear(
        QOpenGLTexture::TextureFormat textureFormat,
        QOpenGLTexture::PixelFormat pixelFormat,
        QOpenGLTexture::PixelType pixelType) noexcept
{
    const auto compareFormats = [=](const TextureFormatInfo &other)
    {
        return other.oglTextureFormat() == textureFormat
                && other.oglPixelFormat() == pixelFormat
                && other.oglPixelType() == pixelType;
    };

    const auto formats = TextureFormatInfo::TextureFormatInfos(::formats);
    const auto it = std::find_if(formats.begin(), formats.end(), compareFormats);
    if (it != formats.end())
        return *it;
    return formats[0];
}

const TextureFormatInfo &TextureFormatInfo::findOGLFormat(
        QOpenGLTexture::TextureFormat textureFormat,
        QOpenGLTexture::PixelFormat pixelFormat,
        QOpenGLTexture::PixelType pixelType) noexcept
{
    return findOGLFormatLinear(textureFormat, pixelFormat, pixelType);
}

QString toQString(TextureFormat format)
{
    const auto en = QMetaEnum::fromType<Details::TextureFormat>();
    Q_ASSERT(en.isValid());
    return en.valueToKey(int(format));
}

TextureFormat fromQString(QStringView input)
{
    const auto me = QMetaEnum::fromType<Details::TextureFormat>();
    Q_ASSERT(me.isValid());
    bool ok = false;
    const auto result = me.keyToValue(input.toLatin1().data(), &ok);
    return ok ? TextureFormat(result) : TextureFormat::Invalid;
}
