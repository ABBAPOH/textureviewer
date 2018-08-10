#ifndef ENUMS_H
#define ENUMS_H

#include <QtCore/qglobal.h>

enum class DDSFourCC : quint32 {
    A16B16G16R16         = 36,
    V8U8                 = 60,
    Q16W16V16U16         = 110,

    R16F                 = 111,
    G16R16F              = 112,
    A16B16G16R16F        = 113,

    R32F                 = 114,
    G32R32F              = 115,
    A32B32G32R32F        = 116,
    CxV8U8               = 117,

    UYVY                 = 0x59565955, // "UYVY"
    R8G8B8G8             = 0x47424752, // "RGBG"
    YUY2                 = 0x32595559, // "YUY2"
    G8R8G8B8             = 0x42475247, // "GRGB"
    DXT1                 = 0x31545844, // "DXT1"
    DXT2                 = 0x32545844, // "DXT2"
    DXT3                 = 0x33545844, // "DXT3"
    DXT4                 = 0x34545844, // "DXT4"
    DXT5                 = 0x35545844, // "DXT5"
    RXGB                 = 0x42475852, // "RXGB"
    ATI2                 = 0x32495441, // "ATI2"
    Multi2ARGB8          = 0x3154454d, // "MET1"
    DX10                 = 0x30315844, // "DX10"
};

enum class DDSFlag : quint32 {
    Caps        = 0x000001,
    Height      = 0x000002,
    Width       = 0x000004,
    Pitch       = 0x000008,
    PixelFormat = 0x001000,
    MipmapCount = 0x020000,
    LinearSize  = 0x080000,
    Depth       = 0x800000
};

Q_DECLARE_FLAGS(DDSFlags, DDSFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DDSFlags)

enum class DDSPixelFormatFlag : quint32 {
    None            = 0x00000000,
    AlphaPixels     = 0x00000001,
    Alpha           = 0x00000002,
    FourCC          = 0x00000004,
    PaletteIndexed4 = 0x00000008,
    PaletteIndexed8 = 0x00000020,
    RGB             = 0x00000040,
    YUV             = 0x00000200,
    Luminance       = 0x00020000,
    Normal          = 0x00080000,
    RGBA = AlphaPixels | RGB,
    LA = AlphaPixels | Luminance
};
Q_DECLARE_FLAGS(DDSPixelFormatFlags, DDSPixelFormatFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DDSPixelFormatFlags)

enum class DDSCapsFlag : quint32 {
    Complex = 0x000008,
    Texture = 0x001000,
    Mipmap  = 0x400000
};

Q_DECLARE_FLAGS(DDSCapsFlags, DDSCapsFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DDSCapsFlags)

enum class DDSCaps2Flag : quint32 {
    CubeMap          = 0x0200,
    CubeMapPositiveX = 0x0400,
    CubeMapNegativeX = 0x0800,
    CubeMapPositiveY = 0x1000,
    CubeMapNegativeY = 0x2000,
    CubeMapPositiveZ = 0x4000,
    CubeMapNegativeZ = 0x8000,
    Volume           = 0x200000
};

Q_DECLARE_FLAGS(DDSCaps2Flags, DDSCaps2Flag)
Q_DECLARE_OPERATORS_FOR_FLAGS(DDSCaps2Flags)

enum class DXGIFormat : quint32 {
    UNKNOWN                     ,
    R32G32B32A32_TYPELESS       ,
    R32G32B32A32_FLOAT          ,
    R32G32B32A32_UINT           ,
    R32G32B32A32_SINT           ,
    R32G32B32_TYPELESS          ,
    R32G32B32_FLOAT             ,
    R32G32B32_UINT              ,
    R32G32B32_SINT              ,
    R16G16B16A16_TYPELESS       ,
    R16G16B16A16_FLOAT          ,
    R16G16B16A16_UNORM          ,
    R16G16B16A16_UINT           ,
    R16G16B16A16_SNORM          ,
    R16G16B16A16_SINT           ,
    R32G32_TYPELESS             ,
    R32G32_FLOAT                ,
    R32G32_UINT                 ,
    R32G32_SINT                 ,
    R32G8X24_TYPELESS           ,
    D32_FLOAT_S8X24_UINT        ,
    R32_FLOAT_X8X24_TYPELESS    ,
    X32_TYPELESS_G8X24_UINT     ,
    R10G10B10A2_TYPELESS        ,
    R10G10B10A2_UNORM           ,
    R10G10B10A2_UINT            ,
    R11G11B10_FLOAT             ,
    R8G8B8A8_TYPELESS           ,
    R8G8B8A8_UNORM              ,
    R8G8B8A8_UNORM_SRGB         ,
    R8G8B8A8_UINT               ,
    R8G8B8A8_SNORM              ,
    R8G8B8A8_SINT               ,
    R16G16_TYPELESS             ,
    R16G16_FLOAT                ,
    R16G16_UNORM                ,
    R16G16_UINT                 ,
    R16G16_SNORM                ,
    R16G16_SINT                 ,
    R32_TYPELESS                ,
    D32_FLOAT                   ,
    R32_FLOAT                   ,
    R32_UINT                    ,
    R32_SINT                    ,
    R24G8_TYPELESS              ,
    D24_UNORM_S8_UINT           ,
    R24_UNORM_X8_TYPELESS       ,
    X24_TYPELESS_G8_UINT        ,
    R8G8_TYPELESS               ,
    R8G8_UNORM                  ,
    R8G8_UINT                   ,
    R8G8_SNORM                  ,
    R8G8_SINT                   ,
    R16_TYPELESS                ,
    R16_FLOAT                   ,
    D16_UNORM                   ,
    R16_UNORM                   ,
    R16_UINT                    ,
    R16_SNORM                   ,
    R16_SINT                    ,
    R8_TYPELESS                 ,
    R8_UNORM                    ,
    R8_UINT                     ,
    R8_SNORM                    ,
    R8_SINT                     ,
    A8_UNORM                    ,
    R1_UNORM                    ,
    R9G9B9E5_SHAREDEXP          ,
    R8G8_B8G8_UNORM             ,
    G8R8_G8B8_UNORM             ,
    BC1_TYPELESS                ,
    BC1_UNORM                   ,
    BC1_UNORM_SRGB              ,
    BC2_TYPELESS                ,
    BC2_UNORM                   ,
    BC2_UNORM_SRGB              ,
    BC3_TYPELESS                ,
    BC3_UNORM                   ,
    BC3_UNORM_SRGB              ,
    BC4_TYPELESS                ,
    BC4_UNORM                   ,
    BC4_SNORM                   ,
    BC5_TYPELESS                ,
    BC5_UNORM                   ,
    BC5_SNORM                   ,
    B5G6R5_UNORM                ,
    B5G5R5A1_UNORM              ,
    B8G8R8A8_UNORM              ,
    B8G8R8X8_UNORM              ,
    R10G10B10_XR_BIAS_A2_UNORM  ,
    B8G8R8A8_TYPELESS           ,
    B8G8R8A8_UNORM_SRGB         ,
    B8G8R8X8_TYPELESS           ,
    B8G8R8X8_UNORM_SRGB         ,
    BC6H_TYPELESS               ,
    BC6H_UF16                   ,
    BC6H_SF16                   ,
    BC7_TYPELESS                ,
    BC7_UNORM                   ,
    BC7_UNORM_SRGB              ,
    AYUV                        ,
    Y410                        ,
    Y416                        ,
    NV12                        ,
    P010                        ,
    P016                        ,
    _420_OPAQUE                  ,
    YUY2                        ,
    Y210                        ,
    Y216                        ,
    NV11                        ,
    AI44                        ,
    IA44                        ,
    P8                          ,
    A8P8                        ,
    B4G4R4A4_UNORM              ,
    P208                        ,
    V208                        ,
    V408                        ,
    FORCE_UINT                  ,
    FormatCount // should be the last
};

#endif // ENUMS_H
