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

#include <QtCore/qdebug.h>
#include <QtCore/qmath.h>

#include <QtGui/qimage.h>

#include "ddsheader.h"

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
static const quint32 ddsMagic = 0x20534444; // "DDS "
static const quint32 dx10Magic = 0x30315844; // "DX10"

static const qint64 headerSize = 128;
static const quint32 ddsSize = 124; // headerSize without magic
static const quint32 pixelFormatSize = 32;

struct FaceOffset
{
    int x, y;
};

constexpr static const DDSCaps2Flag faceFlags[6] = {
    DDSCaps2Flag::CubeMapPositiveX,
    DDSCaps2Flag::CubeMapNegativeX,
    DDSCaps2Flag::CubeMapPositiveY,
    DDSCaps2Flag::CubeMapNegativeY,
    DDSCaps2Flag::CubeMapPositiveZ,
    DDSCaps2Flag::CubeMapNegativeZ
};

struct FormatInfo
{
    DDSFormat format;
    DDSPixelFormatFlags flags;
    quint32 bitCount;
    quint32 rBitMask;
    quint32 gBitMask;
    quint32 bBitMask;
    quint32 aBitMask;
};

static constexpr const FormatInfo formatInfos[] = {
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

static constexpr const DDSFormat knownFourCCs[] = {
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

static inline bool isCubeMap(const DDSHeader &dds)
{
    return (dds.caps2 & DDSCaps2Flag::CubeMap) != 0;
}

static inline bool isVolumeMap(const DDSHeader &dds)
{
    return (dds.caps2 & DDSCaps2Flag::Volume) != 0;
}

static DDSFormat getFormat(const DDSHeader &dds)
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

static const FormatInfo &getFormatInfo(DDSFormat format)
{
    for (const auto &info: gsl::span<const FormatInfo>(formatInfos)) {
        if (info.format == format)
            return info;
    }
    return formatInfos[0];
}

DDSHandler::DDSHandler() :
    m_format(DDSFormat::A8R8G8B8)
{
}

QByteArray DDSHandler::name() const
{
    return QByteArrayLiteral("dds");
}

static Texture::Format convertFormat(DDSFormat format, DXGIFormat format2)
{
    if (format == DDSFormat::DX10) {
        switch (format2) {
        case DXGIFormat::BC1_UNORM: return Texture::Format::DXT1;
        case DXGIFormat::BC1_UNORM_SRGB: return Texture::Format::BC1_UNorm_SRGB;
        case DXGIFormat::BC2_UNORM: return Texture::Format::DXT3;
        case DXGIFormat::BC2_UNORM_SRGB: return Texture::Format::BC2_UNorm_SRGB;
        case DXGIFormat::BC3_UNORM: return Texture::Format::DXT5;
        case DXGIFormat::BC3_UNORM_SRGB: return Texture::Format::BC3_UNorm_SRGB;
        case DXGIFormat::BC6H_UF16: return Texture::Format::BC6H_UF16;
        case DXGIFormat::BC7_UNORM: return Texture::Format::BC7_UNORM;
        case DXGIFormat::BC7_UNORM_SRGB: return Texture::Format::BC7_UNORM_SRGB;
        default:
            break;
        }
        return Texture::Format::Invalid;
    }

    switch (format) {
    case DDSFormat::R8G8B8: return Texture::Format::BGR_888;
    case DDSFormat::A8R8G8B8: return Texture::Format::BGRA_8888;
    case DDSFormat::A8B8G8R8: return Texture::Format::RGBA_8888;
    case DDSFormat::X8B8G8R8: return Texture::Format::RGBX_8888;
    case DDSFormat::X8R8G8B8: return Texture::Format::BGRX_8888;
    case DDSFormat::R5G6B5: return Texture::Format::BGR_565;
    case DDSFormat::A4R4G4B4: return Texture::Format::BGRA_4444;
    case DDSFormat::X4R4G4B4: return Texture::Format::BGRX_4444;
    case DDSFormat::A1R5G5B5: return Texture::Format::BGRA_5551;
    case DDSFormat::X1R5G5B5: return Texture::Format::BGRX_5551;
    case DDSFormat::R3G3B2: return Texture::Format::RGB_332;
    case DDSFormat::A8L8: return Texture::Format::LA88;
    case DDSFormat::A8: return Texture::Format::A8;
    case DDSFormat::L8: return Texture::Format::L8;
    case DDSFormat::A2B10G10R10: return Texture::Format::RGBA_10101002_Rev;
    case DDSFormat::A2R10G10B10: return Texture::Format::BGRA_10101002_Rev;
    case DDSFormat::DXT1: return Texture::Format::DXT1;
    case DDSFormat::DXT3: return Texture::Format::DXT3;
    case DDSFormat::DXT5: return Texture::Format::DXT5;
    case DDSFormat::RXGB: return Texture::Format::RXGB;
    case DDSFormat::ATI2: return Texture::Format::RG_ATI2N_UNorm;
    case DDSFormat::A16B16G16R16F: return Texture::Format::RGBA_16161616F;
    default: return Texture::Format::Invalid;
    }
}

static DDSFormat convertFormat(Texture::Format format)
{
    switch (format) {
    case Texture::Format::BGR_888: return DDSFormat::R8G8B8;
    case Texture::Format::BGRA_8888: return DDSFormat::A8R8G8B8;
    case Texture::Format::RGBX_8888: return DDSFormat::X8B8G8R8;
    case Texture::Format::BGRX_8888: return DDSFormat::X8R8G8B8;
    case Texture::Format::BGR_565: return DDSFormat::R5G6B5;
    case Texture::Format::BGRA_4444: return DDSFormat::A4R4G4B4;
    case Texture::Format::BGRX_4444: return DDSFormat::X4R4G4B4;
    case Texture::Format::BGRA_5551: return DDSFormat::A1R5G5B5;
    case Texture::Format::BGRX_5551: return DDSFormat::X1R5G5B5;
    case Texture::Format::RGB_332: return DDSFormat::R3G3B2;
    case Texture::Format::LA88: return DDSFormat::A8L8;
    case Texture::Format::A8: return DDSFormat::A8;
    case Texture::Format::L8: return DDSFormat::L8;
    case Texture::Format::RGBA_10101002_Rev: return DDSFormat::A2B10G10R10;
    case Texture::Format::BGRA_10101002_Rev: return DDSFormat::A2R10G10B10;
    case Texture::Format::DXT1: return DDSFormat::DXT1;
    case Texture::Format::DXT3: return DDSFormat::DXT3;
    case Texture::Format::DXT5: return DDSFormat::DXT5;
    case Texture::Format::RXGB: return DDSFormat::RXGB;
    case Texture::Format::RG_ATI2N_UNorm: return DDSFormat::ATI2;
    case Texture::Format::RGBA_16161616F: return DDSFormat::A16B16G16R16F;
    default: return DDSFormat::Unknown;
    }
}

bool DDSHandler::read(Texture &texture)
{
    if (!canRead(device()))
        return false;

    if (!doScan())
        return false;

    const auto ulayers = std::max(1u, m_header10.arraySize);
    const auto ulevels = std::max(1u, m_header.mipMapCount);
    const auto cubeMap = isCubeMap(m_header);
    const auto faces = cubeMap ? 6 : 1;

    if (isVolumeMap(m_header)) {
        qCWarning(ddshandler) << "Reading cubemaps is not supported (yet)";
        return false;
    }

    const auto textureFormat = convertFormat(DDSFormat(m_format), DXGIFormat(m_header10.dxgiFormat));
    if (textureFormat == Texture::Format::Invalid) {
        qCWarning(ddshandler) << "Unsupported format" << quint32(m_format);
        return false;
    }

    auto result = Texture::create(
                textureFormat,
                {int(m_header.width), int(m_header.height)},
                cubeMap ? Texture::IsCubemap::Yes : Texture::IsCubemap::No,
                ulevels, ulayers);

    if (result.isNull()) {
        qCWarning(ddshandler) << "Can't create texture";
        return false;
    }

    const auto pitch = Texture::calculateBytesPerLine(textureFormat, int(m_header.width));

    if (pitch != m_header.pitchOrLinearSize) {
        qCDebug(ddshandler) << "Computed pitch differs from the actual pitch"
                            << pitch << "!=" << m_header.pitchOrLinearSize;
    }

    if (!device()->seek(headerSize)) {
        qCWarning(ddshandler) << "Can't seek to mipmap";
        return false;
    }

    for (int layer = 0; layer < int(ulayers); ++layer) {
        for (int face = 0; face < faces; ++face) {
            if (cubeMap && !(m_header.caps2 & (faceFlags[face]))) {
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

    qSwap(texture, result);

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

    if (texture.alignment() != Texture::Alignment::Byte) {
        qCWarning(ddshandler) << "Only byte alignment is supported";
        return false;
    }

    QDataStream s(device().get());
    s.setByteOrder(QDataStream::LittleEndian);

    DDSHeader dds;
    // Filling header
    dds.magic = ddsMagic;
    dds.size = 124;
    dds.flags = DDSFlag::Caps | DDSFlag::Height |
                DDSFlag::Width | DDSFlag::PixelFormat;
    dds.height = quint32(texture.height());
    dds.width = quint32(texture.width());
    dds.depth = 0;
    dds.mipMapCount = quint32(texture.levels() > 1 ? texture.levels() : 0);
    for (int i = 0; i < DDSHeader::ReservedCount; i++)
        dds.reserved1[i] = 0;
    dds.caps = DDSCapsFlag::Texture;
    if (texture.levels() > 1)
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
            quint32(Texture::calculateBytesPerLine(texture.format(), texture.width()));

    s << dds;

    for (int layer = 0; layer < texture.layers(); ++layer) {
        for (int face = 0; face < texture.faces(); ++face) {
            for (int level = 0; level < texture.levels(); ++level) {
                const auto data = texture.imageData({Texture::Side(face), level, layer});
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

bool DDSHandler::canRead(QIODevicePointer device)
{
    if (!device) {
        qCWarning(ddshandler) << "DDSHandler: canRead() called with no device";
        return false;
    }

    if (device->isSequential()) {
        qCWarning(ddshandler) << "DDSHandler: Sequential devices are not supported";
        return false;
    }

    return device->peek(4) == QByteArrayLiteral("DDS ");
}

bool DDSHandler::doScan()
{
    m_format = DDSFormat::Unknown;

//    qint64 oldPos = device()->pos();
//    device()->seek(0);

    QDataStream s(device().get());
    s.setByteOrder(QDataStream::LittleEndian);
    s >> m_header;
    if (m_header.pixelFormat.fourCC == dx10Magic)
        s >> m_header10;

//    device()->seek(oldPos);

    if (s.status() != QDataStream::Ok)
        return false;

    if (!verifyHeader(m_header))
        return false;

    this->m_format = getFormat(m_header);

    return true;
}

bool DDSHandler::verifyHeader(const DDSHeader &dds) const
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

Q_LOGGING_CATEGORY(ddshandler, "pluigns.textureformats.ddshandler")
