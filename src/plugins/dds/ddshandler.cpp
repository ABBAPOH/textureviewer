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

constexpr auto maxInt = std::numeric_limits<int>::max();

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
    TextureFormat format {TextureFormat::Invalid};
    DDSPixelFormatFlags flags;
    quint32 bitCount {0};
    quint32 rBitMask {0};
    quint32 gBitMask {0};
    quint32 bBitMask {0};
    quint32 aBitMask {0};
};

constexpr FormatInfo formatInfos[] = {
    { TextureFormat::Invalid,                              {},  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { TextureFormat::BGRA8_Unorm,    DDSPixelFormatFlag::RGBA, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 },
    { TextureFormat::BGRX8_Unorm,    DDSPixelFormatFlag::RGB,  32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 },
    { TextureFormat::RGBA8_Unorm,    DDSPixelFormatFlag::RGBA, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 },
    { TextureFormat::RGBX8_Unorm,    DDSPixelFormatFlag::RGB,  32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000 },

    { TextureFormat::BGR8_Unorm,     DDSPixelFormatFlag::RGB,  24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 },

    { TextureFormat::BGR565_Unorm,   DDSPixelFormatFlag::RGB,  16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 },
    { TextureFormat::BGRX5551_Unorm, DDSPixelFormatFlag::RGB,  16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00000000 },
    { TextureFormat::BGRA5551_Unorm, DDSPixelFormatFlag::RGBA, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000 },
    { TextureFormat::BGRA4_Unorm,    DDSPixelFormatFlag::RGBA, 16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000 },
    { TextureFormat::BGRX4_Unorm,    DDSPixelFormatFlag::RGB,  16, 0x00000f00, 0x000000f0, 0x0000000f, 0x00000000 },
    { TextureFormat::LA8_Unorm,      DDSPixelFormatFlag::LA,   16, 0x000000ff, 0x00000000, 0x00000000, 0x0000ff00 },

    { TextureFormat::RGB332_Unorm,   DDSPixelFormatFlag::RGB,   8, 0x000000e0, 0x0000001c, 0x00000003, 0x00000000 },
    { TextureFormat::A8_Unorm,       DDSPixelFormatFlag::Alpha, 8, 0x00000000, 0x00000000, 0x00000000, 0x000000ff },
    { TextureFormat::L8_Unorm,   DDSPixelFormatFlag::Luminance, 8, 0x000000ff, 0x00000000, 0x00000000, 0x00000000 },
};

struct FourCCInfo
{
    DDSFourCC fourCC {};
    TextureFormat format {TextureFormat::Invalid};
};

constexpr FourCCInfo fourCCInfos[] = {
    { DDSFourCC::A16B16G16R16, TextureFormat::RGBA16_Unorm },
//    { DDSFourCC::V8U8, TextureFormat::Invalid },
//    { DDSFourCC::Q16W16V16U16, TextureFormat::Invalid },
    { DDSFourCC::R16F, TextureFormat::R16_Float },
    { DDSFourCC::G16R16F, TextureFormat::RG16_Float },
    { DDSFourCC::A16B16G16R16F, TextureFormat::RGBA16_Float },
    { DDSFourCC::R32F, TextureFormat::R32_Float },
    { DDSFourCC::G32R32F, TextureFormat::RG32_Float },
    { DDSFourCC::A32B32G32R32F, TextureFormat::RGBA32_Float },
//   { DDSFourCC::CxV8U8, TextureFormat::Invalid },
//   { DDSFourCC::UYVY, TextureFormat::Invalid },
//   { DDSFourCC::R8G8B8G8, TextureFormat::Invalid },
//   { DDSFourCC::YUY2, TextureFormat::Invalid },
//   { DDSFourCC::G8R8G8B8, TextureFormat::Invalid },
    { DDSFourCC::DXT1, TextureFormat::Bc1Rgb_Unorm },
//   { DDSFourCC::DXT2, TextureFormat::Invalid },
    { DDSFourCC::DXT3, TextureFormat::Bc2_Unorm },
//   { DDSFourCC::DXT4, TextureFormat::Invalid },
    { DDSFourCC::DXT5, TextureFormat::Bc3_Unorm },
    { DDSFourCC::RXGB, TextureFormat::RXGB },
    { DDSFourCC::ATI2, TextureFormat::RXGB },
};

struct DXGIFormatInfo
{
    DXGIFormat dxgiFormat {DXGIFormat::UNKNOWN};
    TextureFormat textureFormat {TextureFormat::Invalid};
};

constexpr DXGIFormatInfo dxgiFormatInfos[size_t(DXGIFormat::FormatCount)] = {
    { DXGIFormat::UNKNOWN, TextureFormat::Invalid },

    { DXGIFormat::R32G32B32A32_TYPELESS, TextureFormat::Invalid      },
    { DXGIFormat::R32G32B32A32_FLOAT,    TextureFormat::RGBA32_Float },
    { DXGIFormat::R32G32B32A32_UINT,     TextureFormat::RGBA32_Uint  },
    { DXGIFormat::R32G32B32A32_SINT,     TextureFormat::RGBA32_Uint  },

    { DXGIFormat::R32G32B32_TYPELESS, TextureFormat::Invalid     },
    { DXGIFormat::R32G32B32_FLOAT,    TextureFormat::RGB32_Float },
    { DXGIFormat::R32G32B32_UINT,     TextureFormat::RGB32_Uint  },
    { DXGIFormat::R32G32B32_SINT,     TextureFormat::RGB32_Sint  },

    { DXGIFormat::R16G16B16A16_TYPELESS, TextureFormat::Invalid      },
    { DXGIFormat::R16G16B16A16_FLOAT,    TextureFormat::RGBA16_Float },
    { DXGIFormat::R16G16B16A16_UNORM,    TextureFormat::RGBA16_Unorm },
    { DXGIFormat::R16G16B16A16_UINT,     TextureFormat::RGBA16_Uint  },
    { DXGIFormat::R16G16B16A16_SNORM,    TextureFormat::RGBA16_Snorm },
    { DXGIFormat::R16G16B16A16_SINT,     TextureFormat::RGBA16_Sint  },

    { DXGIFormat::R32G32_TYPELESS, TextureFormat::Invalid    },
    { DXGIFormat::R32G32_FLOAT,    TextureFormat::RG32_Float },
    { DXGIFormat::R32G32_UINT,     TextureFormat::RG32_Uint  },
    { DXGIFormat::R32G32_SINT,     TextureFormat::RG32_Sint  },

    { DXGIFormat::R32G8X24_TYPELESS },
    { DXGIFormat::D32_FLOAT_S8X24_UINT },
    { DXGIFormat::R32_FLOAT_X8X24_TYPELESS },
    { DXGIFormat::X32_TYPELESS_G8X24_UINT },
    { DXGIFormat::R10G10B10A2_TYPELESS },
    { DXGIFormat::R10G10B10A2_UNORM },
    { DXGIFormat::R10G10B10A2_UINT },
    { DXGIFormat::R11G11B10_FLOAT },

    { DXGIFormat::R8G8B8A8_TYPELESS,   TextureFormat::Invalid     },
    { DXGIFormat::R8G8B8A8_UNORM,      TextureFormat::RGBA8_Unorm },
    { DXGIFormat::R8G8B8A8_UNORM_SRGB, TextureFormat::RGBA8_Srgb  },
    { DXGIFormat::R8G8B8A8_UINT,       TextureFormat::RGBA8_Uint  },
    { DXGIFormat::R8G8B8A8_SNORM,      TextureFormat::RGBA8_Snorm },
    { DXGIFormat::R8G8B8A8_SINT,       TextureFormat::RGBA8_Sint  },

    { DXGIFormat::R16G16_TYPELESS, TextureFormat::Invalid    },
    { DXGIFormat::R16G16_FLOAT,    TextureFormat::RG16_Float },
    { DXGIFormat::R16G16_UNORM,    TextureFormat::RG16_Unorm },
    { DXGIFormat::R16G16_UINT,     TextureFormat::RG16_Uint  },
    { DXGIFormat::R16G16_SNORM,    TextureFormat::RG16_Snorm },
    { DXGIFormat::R16G16_SINT,     TextureFormat::RG16_Sint  },

    { DXGIFormat::R32_TYPELESS, TextureFormat::Invalid   },
    { DXGIFormat::D32_FLOAT  },
    { DXGIFormat::R32_FLOAT,    TextureFormat::R32_Float },
    { DXGIFormat::R32_UINT,     TextureFormat::R32_Uint  },
    { DXGIFormat::R32_SINT,     TextureFormat::R32_Sint  },

    { DXGIFormat::R24G8_TYPELESS },
    { DXGIFormat::D24_UNORM_S8_UINT  },
    { DXGIFormat::R24_UNORM_X8_TYPELESS },
    { DXGIFormat::X24_TYPELESS_G8_UINT },

    { DXGIFormat::R8G8_TYPELESS, TextureFormat::Invalid   },
    { DXGIFormat::R8G8_UNORM,    TextureFormat::RG8_Unorm },
    { DXGIFormat::R8G8_UINT,     TextureFormat::RG8_Uint  },
    { DXGIFormat::R8G8_SNORM,    TextureFormat::RG8_Snorm },
    { DXGIFormat::R8G8_SINT,     TextureFormat::RG8_Sint  },

    { DXGIFormat::R16_TYPELESS, TextureFormat::Invalid   },
    { DXGIFormat::R16_FLOAT,    TextureFormat::R16_Float },
    { DXGIFormat::D16_UNORM },
    { DXGIFormat::R16_UNORM,    TextureFormat::R16_Unorm },
    { DXGIFormat::R16_UINT,     TextureFormat::R16_Uint  },
    { DXGIFormat::R16_SNORM,    TextureFormat::R16_Snorm },
    { DXGIFormat::R16_SINT,     TextureFormat::R16_Sint  },

    { DXGIFormat::R8_TYPELESS, TextureFormat::Invalid  },
    { DXGIFormat::R8_UNORM,    TextureFormat::R8_Unorm },
    { DXGIFormat::R8_UINT,     TextureFormat::R8_Uint  },
    { DXGIFormat::R8_SNORM,    TextureFormat::R8_Snorm },
    { DXGIFormat::R8_SINT,     TextureFormat::R8_Sint  },

    { DXGIFormat::A8_UNORM },
    { DXGIFormat::R1_UNORM },
    { DXGIFormat::R9G9B9E5_SHAREDEXP },
    { DXGIFormat::R8G8_B8G8_UNORM },
    { DXGIFormat::G8R8_G8B8_UNORM },

    { DXGIFormat::BC1_TYPELESS,   TextureFormat::Invalid      },
    { DXGIFormat::BC1_UNORM,      TextureFormat::Bc1Rgb_Unorm },
    { DXGIFormat::BC1_UNORM_SRGB, TextureFormat::Bc1Rgb_Srgb  },
    { DXGIFormat::BC2_TYPELESS,   TextureFormat::Invalid      },
    { DXGIFormat::BC2_UNORM,      TextureFormat::Bc2_Unorm    },
    { DXGIFormat::BC2_UNORM_SRGB, TextureFormat::Bc2_Srgb     },
    { DXGIFormat::BC3_TYPELESS,   TextureFormat::Invalid      },
    { DXGIFormat::BC3_UNORM,      TextureFormat::Bc3_Unorm    },
    { DXGIFormat::BC3_UNORM_SRGB, TextureFormat::Bc3_Srgb     },
    { DXGIFormat::BC4_TYPELESS,   TextureFormat::Invalid      },
    { DXGIFormat::BC4_UNORM,      TextureFormat::Bc4_Unorm    },
    { DXGIFormat::BC4_SNORM,      TextureFormat::Bc4_Snorm    },
    { DXGIFormat::BC5_TYPELESS,   TextureFormat::Invalid      },
    { DXGIFormat::BC5_UNORM,      TextureFormat::Bc5_Unorm    },
    { DXGIFormat::BC5_SNORM,      TextureFormat::Bc5_Snorm    },

    { DXGIFormat::B5G6R5_UNORM,   TextureFormat::BGR565_Unorm   },
    { DXGIFormat::B5G5R5A1_UNORM, TextureFormat::BGRA5551_Unorm },

    { DXGIFormat::B8G8R8A8_UNORM, TextureFormat::BGRA8_Unorm },
    { DXGIFormat::B8G8R8X8_UNORM, TextureFormat::BGRX8_Unorm },

    { DXGIFormat::R10G10B10_XR_BIAS_A2_UNORM },

    { DXGIFormat::B8G8R8A8_TYPELESS,   TextureFormat::Invalid    },
    { DXGIFormat::B8G8R8A8_UNORM_SRGB, TextureFormat::BGRA8_Srgb },
    { DXGIFormat::B8G8R8X8_TYPELESS,   TextureFormat::Invalid    },
    { DXGIFormat::B8G8R8X8_UNORM_SRGB, TextureFormat::BGRX8_Srgb },

    { DXGIFormat::BC6H_TYPELESS,  TextureFormat::Invalid  },
    { DXGIFormat::BC6H_UF16,      TextureFormat::Bc6HUF16 },
    { DXGIFormat::BC6H_SF16,      TextureFormat::Bc6HSF16 },
    { DXGIFormat::BC7_TYPELESS,   TextureFormat::Invalid  },
    { DXGIFormat::BC7_UNORM,      TextureFormat::Bc7_Unorm },
    { DXGIFormat::BC7_UNORM_SRGB, TextureFormat::Bc7_Srgb  },

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

constexpr bool checkFormatPositions()
{
    int position = 0;
    // Use a variable to compile with msvc. It can't build because rvalue is not constexpr
    const auto array = gsl::span<const DXGIFormatInfo>(dxgiFormatInfos);
    for (const auto &format: array) {
        if (format.dxgiFormat != DXGIFormat(position++))
            return false;
    }
    return true;
}
static_assert (checkFormatPositions(), "Incorrect format position in dxgiFormatInfos array");

bool isDX10(const DDSHeader &header)
{
    return DDSFourCC(header.pixelFormat.fourCC) == DDSFourCC::DX10;
}

bool isCubeMap(const DDSHeader &dds)
{
    return (dds.caps2 & DDSCaps2Flag::CubeMap) != 0;
}

bool isVolumeMap(const DDSHeader &dds)
{
    return (dds.caps2 & DDSCaps2Flag::Volume) != 0;
}

constexpr TextureFormat convertFormat(DXGIFormat format)
{
    return gsl::at(dxgiFormatInfos, qsizetype(format)).textureFormat;
}

constexpr DXGIFormat convertFormat(TextureFormat format)
{
    // silence msvc
    const auto infos = gsl::span<const DXGIFormatInfo>(dxgiFormatInfos);
    for (const auto info: infos) {
        if (info.textureFormat == format)
            return info.dxgiFormat;
    }
    return DXGIFormat::UNKNOWN;
}

TextureFormat getFormat(const DDSHeader &dds, const DDSHeaderDX10 &dds10)
{
    const DDSPixelFormat &format = dds.pixelFormat;
    if (format.flags & DDSPixelFormatFlag::PaletteIndexed4) {
        qCWarning(ddshandler()) << "PaletteIndexed4 format is not supported";
        return TextureFormat::Invalid;
    } else if (format.flags & DDSPixelFormatFlag::PaletteIndexed8) {
        qCWarning(ddshandler()) << "PaletteIndexed8 format is not supported";
        return TextureFormat::Invalid;
    } else if (format.flags & DDSPixelFormatFlag::FourCC) {
        if (isDX10(dds)) {
            auto result = convertFormat(DXGIFormat(dds10.dxgiFormat));
            if (result == TextureFormat::Invalid) {
                // ok, we know that format but don't support it
                qCWarning(ddshandler()) << QStringLiteral("Unsupported dxgi format: %1")
                                           .arg(dds10.dxgiFormat, 0, 16);
            }
            return result;
        }

        for (const auto info: gsl::span<const FourCCInfo>(fourCCInfos)) {
            if (DDSFourCC(dds.pixelFormat.fourCC) == info.fourCC)
                return info.format;
        }
        qCWarning(ddshandler()) << QStringLiteral("Unknown fourCC: %1")
                                   .arg(dds.pixelFormat.fourCC, 0, 16);
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
        qCWarning(ddshandler()) << QStringLiteral("Unknown pixel format");
    }

    return TextureFormat::Invalid;
}

const FormatInfo &getFormatInfo(TextureFormat format)
{
    for (const auto &info: gsl::span<const FormatInfo>(formatInfos)) {
        if (info.format == format)
            return info;
    }
    return formatInfos[0];
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

    if (dds.size != DDSHeader::ddsSize) {
        qCWarning(ddshandler) << "Wrong dds.size: actual =" << dds.size
                              << "expected =" << DDSHeader::ddsSize;
        return false;
    }

    if (dds.pixelFormat.size != DDSPixelFormat::pixelFormatSize) {
        qCWarning(ddshandler) << "Wrong dds.pixelFormat.size: actual =" << dds.pixelFormat.size
                              << "expected =" << DDSPixelFormat::pixelFormatSize;
        return false;
    }

    if (dds.width > maxInt) {
        qCWarning(ddshandler) << "Width is too big" << dds.width;
        return false;
    }

    if (dds.height > maxInt) {
        qCWarning(ddshandler) << "Height is too big" << dds.height;
        return false;
    }

    if (dds.depth > maxInt) {
        qCWarning(ddshandler) << "Depth is too big" << dds.depth;
        return false;
    }

    return true;
}

bool verifyHeaderDX10(const DDSHeaderDX10 &header)
{
    if (header.dxgiFormat >= quint32(DXGIFormat::FormatCount)) {
        qCWarning(ddshandler) << QStringLiteral("Invalid dxgiFormat: 0x%1").
                                 arg(quint32(header.dxgiFormat), 0, 16);
        return false;
    }

    if (header.arraySize > maxInt) {
        qCWarning(ddshandler) << "Can't read image with arraySize bigger than maxInt";
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
    DDSHeaderDX10 header10;

    {
        QDataStream s(device().get());
        s.setByteOrder(QDataStream::LittleEndian);
        s >> header;
        if (isDX10(header))
            s >> header10;

        if (s.status() != QDataStream::Ok) {
            qCWarning(ddshandler) << "Can't read header: data stream status =" << s.status();
            return false;
        }
    }

    if (!verifyHeader(header))
        return false;

    if (isDX10(header) && !verifyHeaderDX10(header10))
        return false;

    const auto udepth = isVolumeMap(header) ? std::max(1u, header.depth) : 1u;
    const auto ulayers = std::max(1u, header10.arraySize);
    const auto ulevels = std::max(1u, header.mipMapCount);
    const auto cubeMap = isCubeMap(header);
    const auto faces = cubeMap ? 6 : 1;

    const auto textureFormat = getFormat(header, header10);
    if (textureFormat == TextureFormat::Invalid)
        return false;

    auto result = Texture(
                textureFormat,
                {int(header.width), int(header.height), int(udepth)},
                {Texture::IsCubemap(cubeMap), int(ulevels), int(ulayers)});

    if (result.isNull()) {
        qCWarning(ddshandler) << "Can't create texture";
        return false;
    }

    const auto pitch = Texture::calculateBytesPerLine(textureFormat, int(header.width));

    if (header.pitchOrLinearSize && pitch != header.pitchOrLinearSize) {
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
    DDSHeaderDX10 dds10;
    // Filling header
    dds.flags = DDSFlag::Caps | DDSFlag::Height |
                DDSFlag::Width | DDSFlag::PixelFormat;
    dds.height = quint32(copy.height());
    dds.width = quint32(copy.width());
    dds.depth = 0;
    dds.mipMapCount = quint32(copy.levels() > 1 ? copy.levels() : 0);
    dds.caps = DDSCapsFlag::Texture;
    if (copy.levels() > 1)
        dds.caps |= DDSCapsFlag::Mipmap;

    // TODO (abbapoh): Invert priority to almost always write DX10 files
    const auto &info = getFormatInfo(texture.format());
    if (info.format == TextureFormat::Invalid) {
        const auto format = convertFormat(texture.format());
        if (format == DXGIFormat::UNKNOWN) {
            qCWarning(ddshandler()) << "Unsupported format" << texture.format();
            return false;
        }
        dds.pixelFormat.fourCC = quint32(DDSFourCC::DX10);
        // TODO: do we need flag RGB and aplha?
        dds.pixelFormat.flags = DDSPixelFormatFlag::FourCC;

        dds10.dxgiFormat = quint32(format);
        dds10.arraySize = texture.layers();
    } else {
        dds.pixelFormat.fourCC = 0;
        dds.pixelFormat.flags = info.flags;
        // Filling pixelformat
        dds.pixelFormat.rgbBitCount = info.bitCount;
        dds.pixelFormat.aBitMask = info.aBitMask;
        dds.pixelFormat.rBitMask = info.rBitMask;
        dds.pixelFormat.gBitMask = info.gBitMask;
        dds.pixelFormat.bBitMask = info.bBitMask;
    }

    dds.pitchOrLinearSize =
            quint32(Texture::calculateBytesPerLine(copy.format(), copy.width()));

    s << dds;

    if (isDX10(dds))
        s << dds10;

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
