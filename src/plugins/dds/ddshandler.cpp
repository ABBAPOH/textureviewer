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

static int faceFlags[6] = {
    DDSHeader::Caps2CubeMapPositiveX,
    DDSHeader::Caps2CubeMapNegativeX,
    DDSHeader::Caps2CubeMapPositiveY,
    DDSHeader::Caps2CubeMapNegativeY,
    DDSHeader::Caps2CubeMapPositiveZ,
    DDSHeader::Caps2CubeMapNegativeZ
};

struct FormatInfo
{
    Format format;
    quint32 flags;
    quint32 bitCount;
    quint32 rBitMask;
    quint32 gBitMask;
    quint32 bBitMask;
    quint32 aBitMask;
};

static const FormatInfo formatInfos[] = {
    { FormatUnknown,                            0,  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { FormatA8R8G8B8,    DDSPixelFormat::FlagRGBA, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 },
    { FormatX8R8G8B8,    DDSPixelFormat::FlagRGB,  32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 },
    { FormatA2B10G10R10, DDSPixelFormat::FlagRGBA, 32, 0x000003ff, 0x0000fc00, 0x3ff00000, 0xc0000000 },
    { FormatA8B8G8R8,    DDSPixelFormat::FlagRGBA, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 },
    { FormatX8B8G8R8,    DDSPixelFormat::FlagRGB,  32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000 },
    { FormatG16R16,      DDSPixelFormat::FlagRGBA, 32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 },
    { FormatG16R16,      DDSPixelFormat::FlagRGB,  32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 },
    { FormatA2R10G10B10, DDSPixelFormat::FlagRGBA, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000 },

    { FormatR8G8B8,      DDSPixelFormat::FlagRGB,  24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 },

    { FormatR5G6B5,      DDSPixelFormat::FlagRGB,  16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 },
    { FormatX1R5G5B5,    DDSPixelFormat::FlagRGB,  16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00000000 },
    { FormatA1R5G5B5,    DDSPixelFormat::FlagRGBA, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000 },
    { FormatA4R4G4B4,    DDSPixelFormat::FlagRGBA, 16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000 },
    { FormatA8R3G3B2,    DDSPixelFormat::FlagRGBA, 16, 0x000000e0, 0x0000001c, 0x00000003, 0x0000ff00 },
    { FormatX4R4G4B4,    DDSPixelFormat::FlagRGB,  16, 0x00000f00, 0x000000f0, 0x0000000f, 0x00000000 },
    { FormatA8L8,        DDSPixelFormat::FlagLA,   16, 0x000000ff, 0x00000000, 0x00000000, 0x0000ff00 },
    { FormatL16,   DDSPixelFormat::FlagLuminance,  16, 0x0000ffff, 0x00000000, 0x00000000, 0x00000000 },

    { FormatR3G3B2,      DDSPixelFormat::FlagRGB,  8,  0x000000e0, 0x0000001c, 0x00000003, 0x00000000 },
    { FormatA8,        DDSPixelFormat::FlagAlpha,  8,  0x00000000, 0x00000000, 0x00000000, 0x000000ff },
    { FormatL8,    DDSPixelFormat::FlagLuminance,  8,  0x000000ff, 0x00000000, 0x00000000, 0x00000000 },
    { FormatA4L4,        DDSPixelFormat::FlagLA,   8,  0x0000000f, 0x00000000, 0x00000000, 0x000000f0 },

    { FormatV8U8,        DDSPixelFormat::FlagNormal, 16, 0x000000ff, 0x0000ff00, 0x00000000, 0x00000000 },
    { FormatL6V5U5,                                0, 16, 0x0000001f, 0x000003e0, 0x0000fc00, 0x00000000 },
    { FormatX8L8V8U8,                              0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000 },
    { FormatQ8W8V8U8,    DDSPixelFormat::FlagNormal, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 },
    { FormatV16U16,      DDSPixelFormat::FlagNormal, 32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 },
    { FormatA2W10V10U10, DDSPixelFormat::FlagNormal, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000 }
};
static const size_t formatInfosSize = sizeof(formatInfos)/sizeof(FormatInfo);

static const Format knownFourCCs[] = {
    FormatA16B16G16R16,
    FormatV8U8,
    FormatUYVY,
    FormatR8G8B8G8,
    FormatYUY2,
    FormatG8R8G8B8,
    FormatDXT1,
    FormatDXT2,
    FormatDXT3,
    FormatDXT4,
    FormatDXT5,
    FormatRXGB,
    FormatATI2,
    FormatQ16W16V16U16,
    FormatR16F,
    FormatG16R16F,
    FormatA16B16G16R16F,
    FormatR32F,
    FormatG32R32F,
    FormatA32B32G32R32F,
    FormatCxV8U8
};
static const size_t knownFourCCsSize = sizeof(knownFourCCs)/sizeof(Format);

struct FormatName
{
    Format format;
    const char *const name;
};
static const FormatName formatNames[] = {
    { FormatUnknown,  "unknown" },

    { FormatR8G8B8,   "R8G8B8"  },
    { FormatA8R8G8B8, "A8R8G8B8" },
    { FormatX8R8G8B8, "X8R8G8B8" },
    { FormatR5G6B5,   "R5G6B5" },
    { FormatX1R5G5B5, "X1R5G5B5" },
    { FormatA1R5G5B5, "A1R5G5B5" },
    { FormatA4R4G4B4, "A4R4G4B4" },
    { FormatR3G3B2, "R3G3B2" },
    { FormatA8, "A8" },
    { FormatA8R3G3B2, "A8R3G3B2" },
    { FormatX4R4G4B4, "X4R4G4B4" },
    { FormatA2B10G10R10, "A2B10G10R10" },
    { FormatA8B8G8R8, "A8B8G8R8" },
    { FormatX8B8G8R8, "X8B8G8R8" },
    { FormatG16R16, "G16R16" },
    { FormatA2R10G10B10, "A2R10G10B10" },
    { FormatA16B16G16R16, "A16B16G16R16" },

    { FormatA8P8, "A8P8" },
    { FormatP8, "P8" },

    { FormatL8, "L8" },
    { FormatA8L8, "A8L8" },
    { FormatA4L4, "A4L4" },

    { FormatV8U8, "V8U8" },
    { FormatL6V5U5, "L6V5U5" },
    { FormatX8L8V8U8, "X8L8V8U8" },
    { FormatQ8W8V8U8, "Q8W8V8U8" },
    { FormatV16U16, "V16U16" },
    { FormatA2W10V10U10, "A2W10V10U10" },

    { FormatUYVY, "UYVY" },
    { FormatR8G8B8G8, "R8G8_B8G8" },
    { FormatYUY2, "YUY2" },
    { FormatG8R8G8B8, "G8R8_G8B8" },
    { FormatDXT1, "DXT1" },
    { FormatDXT2, "DXT2" },
    { FormatDXT3, "DXT3" },
    { FormatDXT4, "DXT4" },
    { FormatDXT5, "DXT5" },
    { FormatRXGB, "RXGB" },
    { FormatATI2, "ATI2" },

    { FormatD16Lockable, "D16Lockable" },
    { FormatD32, "D32" },
    { FormatD15S1, "D15S1" },
    { FormatD24S8, "D24S8" },
    { FormatD24X8, "D24X8" },
    { FormatD24X4S4, "D24X4S4" },
    { FormatD16, "D16" },

    { FormatD32FLockable, "D32FLockable" },
    { FormatD24FS8, "D24FS8" },

    { FormatD32Lockable, "D32Lockable" },
    { FormatS8Lockable, "S8Lockable" },

    { FormatL16, "L16" },

    { FormatVertexData, "VertexData" },
    { FormatIndex32, "Index32" },
    { FormatIndex32, "Index32" },

    { FormatQ16W16V16U16, "Q16W16V16U16" },

    { FormatMulti2ARGB8, "Multi2ARGB8" },

    { FormatR16F, "R16F" },
    { FormatG16R16F, "G16R16F" },
    { FormatA16B16G16R16F, "A16B16G16R16F" },

    { FormatR32F, "R32F" },
    { FormatG32R32F, "G32R32F" },
    { FormatA32B32G32R32F, "A32B32G32R32F" },

    { FormatCxV8U8, "CxV8U8" },

    { FormatA1, "A1" },
    { FormatA2B10G10R10_XR_BIAS, "A2B10G10R10_XR_BIAS" },
    { FormatBinaryBuffer, "BinaryBuffer" },

    { FormatP4, "P4" },
    { FormatA4P4, "A4P4" }
};
static const size_t formatNamesSize = sizeof(formatNames)/sizeof(FormatName);

static inline bool isCubeMap(const DDSHeader &dds)
{
    return (dds.caps2 & DDSHeader::Caps2CubeMap) != 0;
}

static inline bool isVolumeMap(const DDSHeader &dds)
{
    return (dds.caps2 & DDSHeader::Caps2Volume) != 0;
}

static inline QRgb yuv2rgb(quint8 Y, quint8 U, quint8 V)
{
    return qRgb(quint8(Y + 1.13983 * (V - 128)),
                quint8(Y - 0.39465 * (U - 128) - 0.58060 * (V - 128)),
                quint8(Y + 2.03211 * (U - 128)));
}

static Format getFormat(const DDSHeader &dds)
{
    const DDSPixelFormat &format = dds.pixelFormat;
    if (format.flags & DDSPixelFormat::FlagPaletteIndexed4) {
        return FormatP4;
    } else if (format.flags & DDSPixelFormat::FlagPaletteIndexed8) {
        return FormatP8;
    } else if (format.flags & DDSPixelFormat::FlagFourCC) {
        for (size_t i = 0; i < knownFourCCsSize; ++i) {
            if (dds.pixelFormat.fourCC == knownFourCCs[i])
                return knownFourCCs[i];
        }
    } else {
        for (size_t i = 0; i < formatInfosSize; ++i) {
            const FormatInfo &info = formatInfos[i];
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

    return FormatUnknown;
}

static const FormatInfo &getFormatInfo(Format format)
{
    for (size_t i = 0; i < formatInfosSize; ++i) {
        const FormatInfo &info = formatInfos[i];
        if (info.format == format)
            return info;
    }
    Q_ASSERT(false);
    return formatInfos[0];
}

// TODO: mipmapSize may incorrectly use an alignment
static qint64 mipmapSize(const DDSHeader &dds, const int format, const int level)
{
    quint32 w = dds.width/(1 << level);
    quint32 h = dds.height/(1 << level);

    switch (format) {
    case FormatR8G8B8:
    case FormatX8R8G8B8:
    case FormatR5G6B5:
    case FormatX1R5G5B5:
    case FormatX4R4G4B4:
    case FormatX8B8G8R8:
    case FormatG16R16:
    case FormatL8:
    case FormatL16:
        return w * h * dds.pixelFormat.rgbBitCount / 8;
    case FormatA8R8G8B8:
    case FormatA1R5G5B5:
    case FormatA4R4G4B4:
    case FormatA8:
    case FormatA8R3G3B2:
    case FormatA2B10G10R10:
    case FormatA8B8G8R8:
    case FormatA2R10G10B10:
    case FormatA8L8:
    case FormatA4L4:
        return w * h * dds.pixelFormat.rgbBitCount / 8;
    case FormatP8:
        return 256 + w * h * 8;
    case FormatA16B16G16R16:
        return w * h * 4 * 2;
    case FormatA8P8:
        break;
    case FormatV8U8:
    case FormatL6V5U5:
        return w * h * 2;
    case FormatX8L8V8U8:
    case FormatQ8W8V8U8:
    case FormatV16U16:
    case FormatA2W10V10U10:
        return w * h * 4;
    case FormatUYVY:
    case FormatR8G8B8G8:
    case FormatYUY2:
    case FormatG8R8G8B8:
        return w * h * 2;
    case FormatDXT1:
        return ((w + 3)/4) * ((h + 3)/4) * 8;
    case FormatDXT2:
    case FormatDXT3:
    case FormatDXT4:
    case FormatDXT5:
        return ((w + 3)/4) * ((h + 3)/4) * 16;
    case FormatD16Lockable:
    case FormatD32:
    case FormatD15S1:
    case FormatD24S8:
    case FormatD24X8:
    case FormatD24X4S4:
    case FormatD16:
    case FormatD32FLockable:
    case FormatD24FS8:
    case FormatD32Lockable:
    case FormatS8Lockable:
    case FormatVertexData:
    case FormatIndex16:
    case FormatIndex32:
        break;
    case FormatQ16W16V16U16:
        return w * h * 4 * 2;
    case FormatMulti2ARGB8:
        break;
    case FormatR16F:
        return w * h * 1 * 2;
    case FormatG16R16F:
        return w * h * 2 * 2;
    case FormatA16B16G16R16F:
        return w * h * 4 * 2;
    case FormatR32F:
        return w * h * 1 * 4;
    case FormatG32R32F:
        return w * h * 2 * 4;
    case FormatA32B32G32R32F:
        return w * h * 4 * 4;
    case FormatCxV8U8:
        return w * h * 2;
    case FormatA1:
    case FormatA2B10G10R10_XR_BIAS:
    case FormatBinaryBuffer:
    case FormatLast:
        break;
    }

    return 0;
}

static qint64 mipmapOffset(const DDSHeader &dds, const int format, const int level)
{
    qint64 result = 0;
    for (int i = 0; i < level; ++i)
        result += mipmapSize(dds, format, i);
    return result;
}

static QByteArray formatName(int format)
{
    for (size_t i = 0; i < formatNamesSize; ++i) {
        if (formatNames[i].format == format)
            return formatNames[i].name;
    }

    return formatNames[0].name;
}

static int formatByName(const QByteArray &name)
{
    const QByteArray loweredName = name.toLower();
    for (size_t i = 0; i < formatNamesSize; ++i) {
        if (QByteArray(formatNames[i].name).toLower() == loweredName)
            return formatNames[i].format;
    }

    return FormatUnknown;
}

DDSHandler::DDSHandler() :
    m_format(FormatA8R8G8B8)
{
}

QByteArray DDSHandler::name() const
{
    return QByteArrayLiteral("dds");
}

bool DDSHandler::canRead() const
{
    return canRead(device());
}

Texture::Format convertFormat(Format format)
{
    switch (format) {
    case FormatA8R8G8B8: return Texture::Format::BGRA_8888;
    case FormatR8G8B8: return Texture::Format::RGB_888;
    case FormatX8B8G8R8: return Texture::Format::RGBX_8888;
    case FormatX8R8G8B8: return Texture::Format::BGRX_8888;
    case FormatR5G6B5: return Texture::Format::RGB_565;
    case FormatX4R4G4B4: return Texture::Format::BGRX_4444;
    case FormatX1R5G5B5: return Texture::Format::BGRX_5551;
    case FormatDXT1: return Texture::Format::DXT1;
    case FormatDXT3: return Texture::Format::DXT3;
    case FormatDXT5: return Texture::Format::DXT5;
    default: return Texture::Format::Invalid;
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
    const auto faces = isCubeMap(m_header) ? 6 : 1;

    if (isCubeMap(m_header)) {
        qCWarning(ddshandler) << "Reading cubemaps is not supported (yet)";
        return false;
    } else if (isVolumeMap(m_header)) {
        qCWarning(ddshandler) << "Reading cubemaps is not supported (yet)";
        return false;
    }

    const auto textureFormat = convertFormat(Format(m_format));
    if (textureFormat == Texture::Format::Invalid) {
        qCWarning(ddshandler) << "Unsupported format" << m_format;
        return false;
    }

    auto result = Texture::create2DTexture(textureFormat, int(m_header.width), int(m_header.height), ulevels, m_header10.arraySize > 0 ? m_header10.arraySize : -1);

    if (result.isNull()) {
        qCWarning(ddshandler) << "Can't create texture";
        return false;
    }

    const auto pitch = Texture::calculateBytesPerLine(textureFormat, int(m_header.width));

    if (pitch != m_header.pitchOrLinearSize) {
        qCDebug(ddshandler) << "Computed pitch differs from the actual pitch"
                            << pitch << "!=" << m_header.pitchOrLinearSize;
    }

//    qint64 pos = headerSize + mipmapOffset(m_header, m_format, 0);
//    qint64 size = mipmapSize(m_header, m_format, 0);
//    if (headerSize + size > device()->size()) {
//        qCWarning(ddshandler) << "Texture file is too small";
//        return false;
//    }

    if (!device()->seek(headerSize)) {
        qCWarning(ddshandler) << "Can't seek to mipmap";
        return false;
    }

    for (int layer = 0; layer < int(ulayers); ++layer) {
        for (int face = 0; face < faces; ++face) {
            if (isCubeMap(m_header)
                    && !(m_header.caps2 & (faceFlags[face]))) {
                continue;
            }

            for (int level = 0; level < int(ulevels); ++level) {
                auto uwidth = std::max<quint32>(1, m_header.width >> level);
                auto uheight = std::max<quint32>(1, m_header.height >> level);
//                auto depth = std::max<quint32>(1, m_header.depth >> level);
                const auto pitch = Texture::calculateBytesPerLine(textureFormat, int(uwidth));
                if (result.isCompressed()) {
                    qsizetype size = pitch * std::max<quint32>(1, (uheight + 3) / 4);
                    if (size != result.bytesPerImage(level)) {
                        qCWarning(ddshandler) << "Image size != texture size:"
                                              << size << "!=" << result.bytesPerImage(level);
                        return false;
                    }
                    const auto data = result.imageData({Texture::Side(face), level, layer});
                    const auto read = device()->read(reinterpret_cast<char *>(data.data()), size);
                    if (read != size) {
                        qCWarning(ddshandler) << "Can't read from file:" << device()->errorString();
                        return false;
                    }
                } else {
                    if (pitch > result.bytesPerLine(level)) {
                        qCWarning(ddshandler) << "Pitch is bigger than texture's bytesPerLine:"
                                              << pitch << ">=" << result.bytesPerLine(level);
                        return false;
                    }

                    for (quint32 y = 0; y < uheight; ++y) {
                        const auto line = result.lineData(
                                    {0, int(y)}, {Texture::Side(face), level, layer});
                        auto read = device()->read(reinterpret_cast<char *>(line.data()), pitch);
                        if (read != pitch) {
                            qCWarning(ddshandler) << "Can't read from file:" << device()->errorString();
                            return false;
                        }
                    }
                }
            }
        }
    }

    qSwap(texture, result);

    return true;
}

bool DDSHandler::write(const Texture &texture)
{
    auto format = FormatUnknown;
    switch (texture.format()) {
    case Texture::Format::RGB_888:
        format = FormatR8G8B8;
        break;
    default:
        break;
    }

    if (format == FormatUnknown) {
        qCWarning(ddshandler) << "unsupported format" << int(texture.format());
        return false;
    }

    QDataStream s(device().get());
    s.setByteOrder(QDataStream::LittleEndian);

    DDSHeader dds;
    // Filling header
    dds.magic = ddsMagic;
    dds.size = 124;
    dds.flags = DDSHeader::FlagCaps | DDSHeader::FlagHeight |
                DDSHeader::FlagWidth | DDSHeader::FlagPixelFormat;
    dds.height = quint32(texture.height());
    dds.width = quint32(texture.width());
    dds.depth = 0;
    dds.mipMapCount = quint32(texture.levels() > 1 ? texture.levels() : 0);
    for (int i = 0; i < DDSHeader::ReservedCount; i++)
        dds.reserved1[i] = 0;
    dds.caps = DDSHeader::CapsTexture;
    dds.caps2 = 0;
    dds.caps3 = 0;
    dds.caps4 = 0;
    dds.reserved2 = 0;

    const auto &info = getFormatInfo(format);
    if (info.format == FormatUnknown) {
        qCWarning(ddshandler) << "Can't find info for format" << format;
        return false;
    }

    // Filling pixelformat
    dds.pixelFormat.size = pixelFormatSize;
    dds.pixelFormat.flags = info.flags;
    dds.pixelFormat.fourCC = 0;
    dds.pixelFormat.rgbBitCount = info.bitCount;
    dds.pixelFormat.aBitMask = info.aBitMask;
    dds.pixelFormat.rBitMask = info.rBitMask;
    dds.pixelFormat.gBitMask = info.gBitMask;
    dds.pixelFormat.bBitMask = info.bBitMask;

    dds.pitchOrLinearSize =
            quint32(Texture::calculateBytesPerLine(texture.format(), texture.width()));

    s << dds;

    for (int y = 0; y < texture.height(); ++y) {
        const auto line = texture.constLineData({0, y}, {});
        const auto written = s.device()->write(reinterpret_cast<const char *>(line.data()), line.size());
        if (written != line.size()) {
            qCWarning(ddshandler) << "Can't write to file:" << s.device()->errorString();
            return false;
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

bool DDSHandler::doScan() const
{
    DDSHandler *that = const_cast<DDSHandler *>(this);
    that->m_format = FormatUnknown;

    qint64 oldPos = device()->pos();
    device()->seek(0);

    QDataStream s(device().get());
    s.setByteOrder(QDataStream::LittleEndian);
    s >> that->m_header;
    if (m_header.pixelFormat.fourCC == dx10Magic)
        s >> that->m_header10;

    device()->seek(oldPos);

    if (s.status() != QDataStream::Ok)
        return false;

    if (!verifyHeader(m_header))
        return false;

    that->m_format = getFormat(m_header);

    return true;
}

bool DDSHandler::verifyHeader(const DDSHeader &dds) const
{
    quint32 flags = dds.flags;
    quint32 requiredFlags = DDSHeader::FlagCaps | DDSHeader::FlagHeight
            | DDSHeader::FlagWidth | DDSHeader::FlagPixelFormat;
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

std::unique_ptr<TextureIOHandler> DdsHandlerPlugin::create(const QMimeType &mimeType)
{
    if (mimeType.name() == u"image/x-dds")
        return std::make_unique<DDSHandler>();
    return nullptr;
}

DdsHandlerPlugin::Capabilities DdsHandlerPlugin::capabilities(const QMimeType &mimeType) const
{
    if (mimeType.name() == u"image/x-dds")
        return Capabilities(DdsHandlerPlugin::CanRead | DdsHandlerPlugin::CanWrite);
    return Capabilities();
}

Q_LOGGING_CATEGORY(ddshandler, "pluigns.textureformats.ddshandler")
