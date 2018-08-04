/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2013 Ivan Komissarov.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the DDS plugin in the Qt ImageFormats module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ddshandler.h"

#include "ddsheader.h"

#include <TextureLib/Texture>

#include <QtCore/QDebug>
#include <QtCore/QtMath>

#include <gsl/span>

namespace {

enum Colors {
    Red = 0,
    Green,
    Blue,
    Alpha,
    ColorCount
};

enum DXTVersions {
    One = 1,
    Two = 2,
    Three = 3,
    Four = 4,
    Five = 5,
    RXGB = 6
};

// All magic numbers are little-endian as long as dds format has little
// endian byte order
constexpr quint32 ddsMagic = 0x20534444; // "DDS "
constexpr quint32 dx10Magic = 0x30315844; // "DX10"

constexpr qint64 headerSize = 128;
constexpr quint32 ddsSize = 124; // headerSize without magic
constexpr quint32 pixelFormatSize = 32;

struct FaceOffset
{
    int x, y;
};

constexpr DDSCaps2Flag faceFlags[6] = {
    DDSCaps2Flag::CubeMapPositiveX,
    DDSCaps2Flag::CubeMapNegativeX,
    DDSCaps2Flag::CubeMapPositiveY,
    DDSCaps2Flag::CubeMapNegativeY,
    DDSCaps2Flag::CubeMapPositiveZ,
    DDSCaps2Flag::CubeMapNegativeZ
};

struct FormatInfo
{
    DDSFormat format {DDSFormat::Unknown};
    DDSPixelFormatFlags flags;
    quint32 bitCount {0};
    quint32 rBitMask {0};
    quint32 gBitMask {0};
    quint32 bBitMask {0};
    quint32 aBitMask {0};
};

constexpr FormatInfo formatInfos[] = {
    { DDSFormat::Unknown,                            {},  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { DDSFormat::A8R8G8B8,    DDSPixelFormatFlag::RGBA, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 },
    { DDSFormat::X8R8G8B8,    DDSPixelFormatFlag::RGB,  32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 },
    { DDSFormat::A2B10G10R10, DDSPixelFormatFlag::RGBA, 32, 0x000003ff, 0x0000fc00, 0x3ff00000, 0xc0000000 },
    { DDSFormat::A8B8G8R8,    DDSPixelFormatFlag::RGBA, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 },
    { DDSFormat::X8B8G8R8,    DDSPixelFormatFlag::RGB,  32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000 },
    { DDSFormat::G16R16,      DDSPixelFormatFlag::RGBA, 32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 },
    { DDSFormat::G16R16,      DDSPixelFormatFlag::RGB,  32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 },
    { DDSFormat::A2R10G10B10, DDSPixelFormatFlag::RGBA, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000 },

    { DDSFormat::R8G8B8,      DDSPixelFormatFlag::RGB,  24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 },

    { DDSFormat::R5G6B5,      DDSPixelFormatFlag::RGB,  16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 },
    { DDSFormat::X1R5G5B5,    DDSPixelFormatFlag::RGB,  16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00000000 },
    { DDSFormat::A1R5G5B5,    DDSPixelFormatFlag::RGBA, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000 },
    { DDSFormat::A4R4G4B4,    DDSPixelFormatFlag::RGBA, 16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000 },
    { DDSFormat::A8R3G3B2,    DDSPixelFormatFlag::RGBA, 16, 0x000000e0, 0x0000001c, 0x00000003, 0x0000ff00 },
    { DDSFormat::X4R4G4B4,    DDSPixelFormatFlag::RGB,  16, 0x00000f00, 0x000000f0, 0x0000000f, 0x00000000 },
    { DDSFormat::A8L8,        DDSPixelFormatFlag::LA,   16, 0x000000ff, 0x00000000, 0x00000000, 0x0000ff00 },
    { DDSFormat::L16,   DDSPixelFormatFlag::Luminance,  16, 0x0000ffff, 0x00000000, 0x00000000, 0x00000000 },

    { DDSFormat::R3G3B2,      DDSPixelFormatFlag::RGB,  8,  0x000000e0, 0x0000001c, 0x00000003, 0x00000000 },
    { DDSFormat::A8,        DDSPixelFormatFlag::Alpha,  8,  0x00000000, 0x00000000, 0x00000000, 0x000000ff },
    { DDSFormat::L8,    DDSPixelFormatFlag::Luminance,  8,  0x000000ff, 0x00000000, 0x00000000, 0x00000000 },
    { DDSFormat::A4L4,        DDSPixelFormatFlag::LA,   8,  0x0000000f, 0x00000000, 0x00000000, 0x000000f0 },

    { DDSFormat::V8U8,        DDSPixelFormatFlag::Normal, 16, 0x000000ff, 0x0000ff00, 0x00000000, 0x00000000 },
    { DDSFormat::L6V5U5,                                0, 16, 0x0000001f, 0x000003e0, 0x0000fc00, 0x00000000 },
    { DDSFormat::X8L8V8U8,                              0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000 },
    { DDSFormat::Q8W8V8U8,    DDSPixelFormatFlag::Normal, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 },
    { DDSFormat::V16U16,      DDSPixelFormatFlag::Normal, 32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 },
    { DDSFormat::A2W10V10U10, DDSPixelFormatFlag::Normal, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000 }
};

constexpr DDSFormat knownFourCCs[] = {
    DDSFormat::A16B16G16R16,
    DDSFormat::V8U8,
    DDSFormat::UYVY,
    DDSFormat::R8G8B8G8,
    DDSFormat::YUY2,
    DDSFormat::G8R8G8B8,
    DDSFormat::DXT1,
    DDSFormat::DXT2,
    DDSFormat::DXT3,
    DDSFormat::DXT4,
    DDSFormat::DXT5,
    DDSFormat::RXGB,
    DDSFormat::ATI2,
    DDSFormat::Q16W16V16U16,
    DDSFormat::R16F,
    DDSFormat::G16R16F,
    DDSFormat::A16B16G16R16F,
    DDSFormat::R32F,
    DDSFormat::G32R32F,
    DDSFormat::A32B32G32R32F,
    DDSFormat::CxV8U8,
    DDSFormat::DX10
};

struct DXGIFormatInfo
{
    DXGIFormat dxgiFormat {DXGIFormat::UNKNOWN};
    TextureFormat textureFormat {TextureFormat::Invalid};
};

constexpr DXGIFormatInfo dxgiFormatInfos[size_t(DXGIFormat::FormatCount)] = {
    { DXGIFormat::UNKNOWN, TextureFormat::Invalid },
    { DXGIFormat::R32G32B32A32_TYPELESS },
    { DXGIFormat::R32G32B32A32_FLOAT },
    { DXGIFormat::R32G32B32A32_UINT },
    { DXGIFormat::R32G32B32A32_SINT },
    { DXGIFormat::R32G32B32_TYPELESS },
    { DXGIFormat::R32G32B32_FLOAT },
    { DXGIFormat::R32G32B32_UINT },
    { DXGIFormat::R32G32B32_SINT },
    { DXGIFormat::R16G16B16A16_TYPELESS },
    { DXGIFormat::R16G16B16A16_FLOAT },
    { DXGIFormat::R16G16B16A16_UNORM },
    { DXGIFormat::R16G16B16A16_UINT },
    { DXGIFormat::R16G16B16A16_SNORM },
    { DXGIFormat::R16G16B16A16_SINT },
    { DXGIFormat::R32G32_TYPELESS},
    { DXGIFormat::R32G32_FLOAT },
    { DXGIFormat::R32G32_UINT },
    { DXGIFormat::R32G32_SINT },
    { DXGIFormat::R32G8X24_TYPELESS },
    { DXGIFormat::D32_FLOAT_S8X24_UINT },
    { DXGIFormat::R32_FLOAT_X8X24_TYPELESS },
    { DXGIFormat::X32_TYPELESS_G8X24_UINT },
    { DXGIFormat::R10G10B10A2_TYPELESS },
    { DXGIFormat::R10G10B10A2_UNORM },
    { DXGIFormat::R10G10B10A2_UINT },
    { DXGIFormat::R11G11B10_FLOAT },

    { DXGIFormat::R8G8B8A8_TYPELESS,   TextureFormat::Invalid    },
    { DXGIFormat::R8G8B8A8_UNORM,      TextureFormat::RGBA8Unorm },
    { DXGIFormat::R8G8B8A8_UNORM_SRGB, TextureFormat::RGBA8Srgb  },
    { DXGIFormat::R8G8B8A8_UINT,       TextureFormat::RGBA8Uint  },
    { DXGIFormat::R8G8B8A8_SNORM,      TextureFormat::RGBA8Snorm },
    { DXGIFormat::R8G8B8A8_SINT,       TextureFormat::RGBA8Sint  },

    { DXGIFormat::R16G16_TYPELESS},
    { DXGIFormat::R16G16_FLOAT },
    { DXGIFormat::R16G16_UNORM },
    { DXGIFormat::R16G16_UINT },
    { DXGIFormat::R16G16_SNORM },
    { DXGIFormat::R16G16_SINT },
    { DXGIFormat::R32_TYPELESS },
    { DXGIFormat::D32_FLOAT  },
    { DXGIFormat::R32_FLOAT },
    { DXGIFormat::R32_UINT },
    { DXGIFormat::R32_SINT },
    { DXGIFormat::R24G8_TYPELESS },
    { DXGIFormat::D24_UNORM_S8_UINT  },
    { DXGIFormat::R24_UNORM_X8_TYPELESS },
    { DXGIFormat::X24_TYPELESS_G8_UINT },

    { DXGIFormat::R8G8_TYPELESS, TextureFormat::Invalid  },
    { DXGIFormat::R8G8_UNORM,    TextureFormat::RG8Unorm },
    { DXGIFormat::R8G8_UINT,     TextureFormat::RG8Uint  },
    { DXGIFormat::R8G8_SNORM,    TextureFormat::RG8Snorm },
    { DXGIFormat::R8G8_SINT,     TextureFormat::RG8Sint  },

    { DXGIFormat::R16_TYPELESS, TextureFormat::Invalid  },
    { DXGIFormat::R16_FLOAT,    TextureFormat::R16F     },
    { DXGIFormat::D16_UNORM },
    { DXGIFormat::R16_UNORM,    TextureFormat::R16Unorm },
    { DXGIFormat::R16_UINT,     TextureFormat::R16Uint  },
    { DXGIFormat::R16_SNORM,    TextureFormat::R16Snorm },
    { DXGIFormat::R16_SINT,     TextureFormat::R16Sint  },

    { DXGIFormat::R8_TYPELESS, TextureFormat::Invalid },
    { DXGIFormat::R8_UNORM,    TextureFormat::R8Unorm },
    { DXGIFormat::R8_UINT,     TextureFormat::R8Uint  },
    { DXGIFormat::R8_SNORM,    TextureFormat::R8Snorm },
    { DXGIFormat::R8_SINT,     TextureFormat::R8Sint  },

    { DXGIFormat::A8_UNORM },
    { DXGIFormat::R1_UNORM },
    { DXGIFormat::R9G9B9E5_SHAREDEXP },
    { DXGIFormat::R8G8_B8G8_UNORM },
    { DXGIFormat::G8R8_G8B8_UNORM },

    { DXGIFormat::BC1_TYPELESS,   TextureFormat::Invalid     },
    { DXGIFormat::BC1_UNORM,      TextureFormat::Bc1RgbUnorm },
    { DXGIFormat::BC1_UNORM_SRGB, TextureFormat::Bc1RgbSrgb  },
    { DXGIFormat::BC2_TYPELESS,   TextureFormat::Invalid     },
    { DXGIFormat::BC2_UNORM,      TextureFormat::Bc2Unorm    },
    { DXGIFormat::BC2_UNORM_SRGB, TextureFormat::Bc2Srgb     },
    { DXGIFormat::BC3_TYPELESS,   TextureFormat::Invalid     },
    { DXGIFormat::BC3_UNORM,      TextureFormat::Bc3Unorm    },
    { DXGIFormat::BC3_UNORM_SRGB, TextureFormat::Bc3Srgb     },
    { DXGIFormat::BC4_TYPELESS,   TextureFormat::Invalid     },
    { DXGIFormat::BC4_UNORM,      TextureFormat::Bc4Unorm    },
    { DXGIFormat::BC4_SNORM,      TextureFormat::Bc4Snorm    },
    { DXGIFormat::BC5_TYPELESS,   TextureFormat::Invalid     },
    { DXGIFormat::BC5_UNORM,      TextureFormat::Bc5Unorm    },
    { DXGIFormat::BC5_SNORM,      TextureFormat::Bc5Snorm    },

    { DXGIFormat::B5G6R5_UNORM },
    { DXGIFormat::B5G5R5A1_UNORM },
    { DXGIFormat::B8G8R8A8_UNORM },
    { DXGIFormat::B8G8R8X8_UNORM },
    { DXGIFormat::R10G10B10_XR_BIAS_A2_UNORM },
    { DXGIFormat::B8G8R8A8_TYPELESS },
    { DXGIFormat::B8G8R8A8_UNORM_SRGB },
    { DXGIFormat::B8G8R8X8_TYPELESS },
    { DXGIFormat::B8G8R8X8_UNORM_SRGB },

    { DXGIFormat::BC6H_TYPELESS,  TextureFormat::Invalid  },
    { DXGIFormat::BC6H_UF16,      TextureFormat::Bc6HUF16 },
    { DXGIFormat::BC6H_SF16,      TextureFormat::Bc6HSF16 },
    { DXGIFormat::BC7_TYPELESS,   TextureFormat::Invalid  },
    { DXGIFormat::BC7_UNORM,      TextureFormat::Bc7Unorm },
    { DXGIFormat::BC7_UNORM_SRGB, TextureFormat::Bc7Srgb  },

    { DXGIFormat::AYUV },
    { DXGIFormat::Y410 },
    { DXGIFormat::Y416 },
    { DXGIFormat::NV12 },
    { DXGIFormat::P010 },
    { DXGIFormat::P016 },
    { DXGIFormat::_420_OPAQUE },
    { DXGIFormat::YUY2 },
    { DXGIFormat::Y210 },
    { DXGIFormat::Y216 },
    { DXGIFormat::NV11 },
    { DXGIFormat::AI44 },
    { DXGIFormat::IA44 },
    { DXGIFormat::P8 },
    { DXGIFormat::A8P8 },
    { DXGIFormat::B4G4R4A4_UNORM },
    { DXGIFormat::P208 },
    { DXGIFormat::V208 },
    { DXGIFormat::V408 },
    { DXGIFormat::FORCE_UINT },
};

bool isCubeMap(const DDSHeader &dds)
{
    return (dds.caps2 & DDSCaps2Flag::CubeMap) != 0;
}

bool isVolumeMap(const DDSHeader &dds)
{
    return (dds.caps2 & DDSCaps2Flag::Volume) != 0;
}

DDSFormat getFormat(const DDSHeader &dds)
{
    const DDSPixelFormat &format = dds.pixelFormat;
    if (format.flags & DDSPixelFormatFlag::PaletteIndexed4) {
        return DDSFormat::P4;
    } else if (format.flags & DDSPixelFormatFlag::PaletteIndexed8) {
        return DDSFormat::P8;
    } else if (format.flags & DDSPixelFormatFlag::FourCC) {
            for (const auto fourCC: gsl::span<const DDSFormat>(knownFourCCs)) {
            if (DDSFormat(dds.pixelFormat.fourCC) == fourCC)
                return fourCC;
        }
    } else {
        for (const auto &info: gsl::span<const FormatInfo>(formatInfos)) {
            if ((format.flags & info.flags) == info.flags &&
                format.rgbBitCount == info.bitCount &&
                format.rBitMask == info.rBitMask &&
                format.bBitMask == info.bBitMask &&
                format.bBitMask == info.bBitMask &&
                format.aBitMask == info.aBitMask) {
                return info.format;
            }
        }
    }

    return DDSFormat::Unknown;
}

const FormatInfo &getFormatInfo(DDSFormat format)
{
    for (const auto &info: gsl::span<const FormatInfo>(formatInfos)) {
        if (info.format == format)
            return info;
    }
    return formatInfos[0];
}

constexpr TextureFormat convertFormat(DXGIFormat format)
{
    if (size_t(format) >= size_t(DXGIFormat::FormatCount))
        return TextureFormat::Invalid;
    return gsl::at(dxgiFormatInfos, size_t(format)).textureFormat;
}

constexpr TextureFormat convertFormat(DDSFormat format, DXGIFormat format2)
{
    if (format == DDSFormat::DX10) {
        return convertFormat(format2);
    }

    switch (format) {
    case DDSFormat::R8G8B8: return TextureFormat::BGR8Unorm;
    case DDSFormat::A8R8G8B8: return TextureFormat::BGRA8Unorm;
    case DDSFormat::A8B8G8R8: return TextureFormat::RGBA8Unorm;
    case DDSFormat::X8B8G8R8: return TextureFormat::RGBX8Unorm;
    case DDSFormat::X8R8G8B8: return TextureFormat::BGRX8Unorm;
    case DDSFormat::R5G6B5: return TextureFormat::BGR565Unorm;
    case DDSFormat::A4R4G4B4: return TextureFormat::BGRA4Unorm;
    case DDSFormat::X4R4G4B4: return TextureFormat::BGRX4Unorm;
    case DDSFormat::A1R5G5B5: return TextureFormat::BGRA5551Unorm;
    case DDSFormat::X1R5G5B5: return TextureFormat::BGRX5551Unorm;
    case DDSFormat::R3G3B2: return TextureFormat::RGB332Unorm;
    case DDSFormat::A8L8: return TextureFormat::LA8Unorm;
    case DDSFormat::A8: return TextureFormat::A8Unorm;
    case DDSFormat::L8: return TextureFormat::L8Unorm;
    case DDSFormat::DXT1: return TextureFormat::Bc1RgbUnorm;
    case DDSFormat::DXT3: return TextureFormat::Bc2Unorm;
    case DDSFormat::DXT5: return TextureFormat::Bc3Unorm;
    case DDSFormat::RXGB: return TextureFormat::RXGB;
    case DDSFormat::ATI2: return TextureFormat::RG_ATI2N_UNorm;
    case DDSFormat::A16B16G16R16F: return TextureFormat::RGBA16F;
    default: return TextureFormat::Invalid;
    }
}

constexpr DDSFormat convertFormat(TextureFormat format)
{
    switch (format) {
    case TextureFormat::BGR8Unorm: return DDSFormat::R8G8B8;
    case TextureFormat::BGRA8Unorm: return DDSFormat::A8R8G8B8;
    case TextureFormat::RGBX8Unorm: return DDSFormat::X8B8G8R8;
    case TextureFormat::BGRX8Unorm: return DDSFormat::X8R8G8B8;
    case TextureFormat::BGR565Unorm: return DDSFormat::R5G6B5;
    case TextureFormat::BGRA4Unorm: return DDSFormat::A4R4G4B4;
    case TextureFormat::BGRX4Unorm: return DDSFormat::X4R4G4B4;
    case TextureFormat::BGRA5551Unorm: return DDSFormat::A1R5G5B5;
    case TextureFormat::BGRX5551Unorm: return DDSFormat::X1R5G5B5;
    case TextureFormat::RGB332Unorm: return DDSFormat::R3G3B2;
    case TextureFormat::LA8Unorm: return DDSFormat::A8L8;
    case TextureFormat::A8Unorm: return DDSFormat::A8;
    case TextureFormat::L8Unorm: return DDSFormat::L8;
    case TextureFormat::Bc1RgbUnorm: return DDSFormat::DXT1;
    case TextureFormat::Bc2Unorm: return DDSFormat::DXT3;
    case TextureFormat::Bc3Unorm: return DDSFormat::DXT5;
    case TextureFormat::RXGB: return DDSFormat::RXGB;
    case TextureFormat::RG_ATI2N_UNorm: return DDSFormat::ATI2;
    case TextureFormat::RGBA16F: return DDSFormat::A16B16G16R16F;
    default: return DDSFormat::Unknown;
    }
}

bool verifyHeader(const DDSHeader &dds)
{
    const auto flags = dds.flags;
    const auto requiredFlags = DDSFlag::Caps | DDSFlag::Height
            | DDSFlag::Width | DDSFlag::PixelFormat;
    if ((flags & requiredFlags) != requiredFlags) {
        qCWarning(ddshandler) << "Wrong dds.flags - not all required flags present. "
                                 "Actual flags :" << flags;
        return false;
    }

    if (dds.size != ddsSize) {
        qCWarning(ddshandler) << "Wrong dds.size: actual =" << dds.size
                              << "expected =" << ddsSize;
        return false;
    }

    if (dds.pixelFormat.size != pixelFormatSize) {
        qCWarning(ddshandler) << "Wrong dds.pixelFormat.size: actual =" << dds.pixelFormat.size
                              << "expected =" << pixelFormatSize;
        return false;
    }

    if (dds.width > INT_MAX || dds.height > INT_MAX) {
        qCWarning(ddshandler) << "Can't read image with w/h bigger than INT_MAX";
        return false;
    }

    return true;
}

} // namespace

bool DDSHandler::read(Texture &texture)
{
    if (device()->peek(4) != QByteArrayLiteral("DDS "))
        return false;

    DDSHeader header;
    DDSFormat format {DDSFormat::Unknown};
    DDSHeaderDX10 header10;

    {
        QDataStream s(device().get());
        s.setByteOrder(QDataStream::LittleEndian);
        s >> header;
        if (header.pixelFormat.fourCC == dx10Magic)
            s >> header10;

        if (s.status() != QDataStream::Ok) {
            qCWarning(ddshandler) << "Can't read header: data stream status =" << s.status();
            return false;
        }
    }

    if (!verifyHeader(header))
        return false;

    format = getFormat(header);

    const auto ulayers = std::max(1u, header10.arraySize);
    const auto ulevels = std::max(1u, header.mipMapCount);
    const auto cubeMap = isCubeMap(header);
    const auto faces = cubeMap ? 6 : 1;

    if (isVolumeMap(header)) {
        qCWarning(ddshandler) << "Reading cubemaps is not supported (yet)";
        return false;
    }

    const auto textureFormat = convertFormat(DDSFormat(format), DXGIFormat(header10.dxgiFormat));
    if (textureFormat == TextureFormat::Invalid) {
        if (header.pixelFormat.fourCC == dx10Magic) {
            qCWarning(ddshandler) << QStringLiteral("Unsupported dxgi format 0x%1")
                                     .arg(quint32(format), 0, 16);
        } else {
            qCWarning(ddshandler) << QStringLiteral("Unsupported format")
                                     .arg(quint32(format), 0, 16);
        }
        return false;
    }

    auto result = Texture::create(
                textureFormat,
                {int(header.width), int(header.height)},
                cubeMap ? Texture::IsCubemap::Yes : Texture::IsCubemap::No,
                ulevels, ulayers);

    if (result.isNull()) {
        qCWarning(ddshandler) << "Can't create texture";
        return false;
    }

    const auto pitch = Texture::calculateBytesPerLine(textureFormat, int(header.width));

    if (pitch != header.pitchOrLinearSize) {
        qCDebug(ddshandler) << "Computed pitch differs from the actual pitch"
                            << pitch << "!=" << header.pitchOrLinearSize;
    }

    for (int layer = 0; layer < int(ulayers); ++layer) {
        for (int face = 0; face < faces; ++face) {
            if (cubeMap && !(header.caps2 & (faceFlags[face]))) {
                continue;
            }

            for (int level = 0; level < int(ulevels); ++level) {
                const auto data = result.imageData({Texture::Side(face), level, layer});
                const auto read = device()->read(reinterpret_cast<char *>(data.data()), data.size());
                if (read != data.size()) {
                    qCWarning(ddshandler) << "Can't read from file:" << device()->errorString();
                    return false;
                }
            }
        }
    }

    texture = std::move(result);

    return true;
}

bool DDSHandler::write(const Texture &texture)
{
    const auto format = convertFormat(texture.format());
    if (format == DDSFormat::Unknown) {
        qCWarning(ddshandler) << "unsupported format" << int(texture.format());
        return false;
    }

    if (texture.layers() > 1) {
        qCWarning(ddshandler) << "Writing layers are not supported";
        return false;
    }

    if (texture.depth() > 1) {
        qCWarning(ddshandler) << "Writing volume maps are not supported";
        return false;
    }

    if (texture.faces() > 1) {
        qCWarning(ddshandler) << "Writing cube maps are not supported";
        return false;
    }

    const auto copy = texture.convert(Texture::Alignment::Byte);

    QDataStream s(device().get());
    s.setByteOrder(QDataStream::LittleEndian);

    DDSHeader dds;
    // Filling header
    dds.magic = ddsMagic;
    dds.size = 124;
    dds.flags = DDSFlag::Caps | DDSFlag::Height |
                DDSFlag::Width | DDSFlag::PixelFormat;
    dds.height = quint32(copy.height());
    dds.width = quint32(copy.width());
    dds.depth = 0;
    dds.mipMapCount = quint32(copy.levels() > 1 ? copy.levels() : 0);
    for (int i = 0; i < DDSHeader::ReservedCount; i++)
        dds.reserved1[i] = 0;
    dds.caps = DDSCapsFlag::Texture;
    if (copy.levels() > 1)
        dds.caps |= DDSCapsFlag::Mipmap;
    dds.caps2 = 0;
    dds.caps3 = 0;
    dds.caps4 = 0;
    dds.reserved2 = 0;

    const auto &info = getFormatInfo(format);
    if (info.format == DDSFormat::Unknown) {
        dds.pixelFormat.fourCC = quint32(format);
        // TODO: do we need flag RGB and aplha?
        dds.pixelFormat.flags = DDSPixelFormatFlag::FourCC;
    } else {
        dds.pixelFormat.fourCC = 0;
        dds.pixelFormat.flags = info.flags;
    }

    // Filling pixelformat
    dds.pixelFormat.size = pixelFormatSize;
    dds.pixelFormat.rgbBitCount = info.bitCount;
    dds.pixelFormat.aBitMask = info.aBitMask;
    dds.pixelFormat.rBitMask = info.rBitMask;
    dds.pixelFormat.gBitMask = info.gBitMask;
    dds.pixelFormat.bBitMask = info.bBitMask;

    dds.pitchOrLinearSize =
            quint32(Texture::calculateBytesPerLine(copy.format(), copy.width()));

    s << dds;

    for (int layer = 0; layer < copy.layers(); ++layer) {
        for (int face = 0; face < copy.faces(); ++face) {
            for (int level = 0; level < copy.levels(); ++level) {
                const auto data = copy.imageData({Texture::Side(face), level, layer});
                const auto writen = device()->write(reinterpret_cast<const char *>(data.data()), data.size());
                if (writen != data.size()) {
                    qCWarning(ddshandler) << "Can't write to device:" << device()->errorString();
                    return false;
                }
            }
        }
    }

    return true;
}

Q_LOGGING_CATEGORY(ddshandler, "plugins.textureformats.ddshandler")
