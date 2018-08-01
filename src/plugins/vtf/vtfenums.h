#ifndef VTFENUMS_H
#define VTFENUMS_H

#include <qglobal.h>

constexpr quint32 paddedHeaderSize = 80;
constexpr quint32 maxResourcesCount = 32;

enum class VTFImageFormat : quint32
{
    None = 0xffffffff,
    RGBA_8888 = 0,
    ABGR_8888,
    RGB_888,
    BGR_888,
    RGB_565,
    I8,
    IA88,
    P8,
    A8,
    RGB_888_BlueScreen,
    BGR_888_BlueScreen,
    ARGB_8888,
    BGRA_8888,
    DXT1,
    DXT3,
    DXT5,
    BGRX_8888,
    BGR_565,
    BGRX_5551,
    BGRA_4444,
    DXT1_ONEBITALPHA,
    BGRA_5551,
    UV88,
    UVWQ_8888,
    RGBA_16161616F,
    RGBA_16161616,
    UVLX_8888,
    FormatCount
};

inline constexpr VTFImageFormat vtfFormat(quint32 value) noexcept
{
    if (value >= quint32(VTFImageFormat::FormatCount))
        return VTFImageFormat::None;

    return VTFImageFormat(value);
}

enum class VTFFlag : quint32
{
    PointSampling = 0x00000001,
    TrilinearSampling = 0x00000002,
    ClampS = 0x00000004,
    ClampT = 0x00000008,
    AnisotropicSampling = 0x00000010,
    HintDXT5 = 0x00000020,
    NoCompress = 0x00000040,
    NormalMap = 0x00000080,
    NoMipmaps = 0x00000100,
    NoLevelOfDetail = 0x00000200,
    NoMinimumMipmap = 0x00000400,
    Procedural = 0x00000800,
    OneBitAlpha = 0x00001000,
    EightBitAlpha = 0x00002000,
    EnvironmentMap = 0x00004000,
    RenderTarget = 0x00008000,
    DepthRenderTarget = 0x00010000,
    NoDebugOverride = 0x00020000,
    SingleCopy = 0x00040000,
    PreSRGB = 0x00080000,
    NoDepthBuffer = 0x00800000,
    NiceFiltered = 0x01000000,
    ClampU = 0x02000000,
    VertexTexture = 0x04000000,
    SSBump = 0x08000000,
    Border = 0x20000000
};

Q_DECLARE_FLAGS(VTFFlags, VTFFlag);

inline constexpr quint32 makeVtfResourceType(quint8 a, quint8 b, quint8 c, quint8 d = 0) noexcept
{
    return quint32(a) | quint32(b << 8) | quint32(c << 16) | quint32(d << 24);
}

enum class VTFResourceType : quint32
{
    LegacyLowResolutionImage = makeVtfResourceType(0x01, 0, 0),
    LegacyImage = makeVtfResourceType(0x30, 0, 0),
    Sheet = makeVtfResourceType(0x10, 0, 0),
    CRC = makeVtfResourceType('C', 'R', 'C', 0x02),
    TextureLodSettings = makeVtfResourceType('L', 'O', 'D', 0x02),
    TextureSettingsEx = makeVtfResourceType('T', 'S', 'O', 0x02),
    KeyValueData = makeVtfResourceType('K', 'V', 'D'),
};

#endif // VTFENUMS_H
