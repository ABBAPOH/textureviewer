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
