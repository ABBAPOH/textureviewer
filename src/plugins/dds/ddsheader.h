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

#pragma once

#include "enums.h"

#include <QtCore/QDataStream>

struct DDSPixelFormat
{
    enum DDSPixelFormatFlags {
        FlagAlphaPixels     = 0x00000001,
        FlagAlpha           = 0x00000002,
        FlagFourCC          = 0x00000004,
        FlagPaletteIndexed4 = 0x00000008,
        FlagPaletteIndexed8 = 0x00000020,
        FlagRGB             = 0x00000040,
        FlagYUV             = 0x00000200,
        FlagLuminance       = 0x00020000,
        FlagNormal          = 0x00080000,
        FlagRGBA = FlagAlphaPixels | FlagRGB,
        FlagLA = FlagAlphaPixels | FlagLuminance
    };

    quint32 size;
    quint32 flags;
    quint32 fourCC;
    quint32 rgbBitCount;
    quint32 rBitMask;
    quint32 gBitMask;
    quint32 bBitMask;
    quint32 aBitMask;
};

QDataStream &operator>>(QDataStream &s, DDSPixelFormat &pixelFormat);
QDataStream &operator<<(QDataStream &s, const DDSPixelFormat &pixelFormat);

struct DDSHeader
{
    enum DDSFlags {
        FlagCaps        = 0x000001,
        FlagHeight      = 0x000002,
        FlagWidth       = 0x000004,
        FlagPitch       = 0x000008,
        FlagPixelFormat = 0x001000,
        FlagMipmapCount = 0x020000,
        FlagLinearSize  = 0x080000,
        FlagDepth       = 0x800000
    };

    enum DDSCapsFlags {
        CapsComplex = 0x000008,
        CapsTexture = 0x001000,
        CapsMipmap  = 0x400000
    };

    enum DDSCaps2Flags {
        Caps2CubeMap          = 0x0200,
        Caps2CubeMapPositiveX = 0x0400,
        Caps2CubeMapNegativeX = 0x0800,
        Caps2CubeMapPositiveY = 0x1000,
        Caps2CubeMapNegativeY = 0x2000,
        Caps2CubeMapPositiveZ = 0x4000,
        Caps2CubeMapNegativeZ = 0x8000,
        Caps2Volume           = 0x200000
    };

    enum { ReservedCount = 11 };

    quint32 magic;
    quint32 size;
    quint32 flags;
    quint32 height;
    quint32 width;
    quint32 pitchOrLinearSize;
    quint32 depth;
    quint32 mipMapCount;
    quint32 reserved1[ReservedCount];
    DDSPixelFormat pixelFormat;
    quint32 caps;
    quint32 caps2;
    quint32 caps3;
    quint32 caps4;
    quint32 reserved2;
};

QDataStream &operator>>(QDataStream &s, DDSHeader &header);
QDataStream &operator<<(QDataStream &s, const DDSHeader &header);

struct DDSHeaderDX10
{
    quint32 dxgiFormat {0};
    quint32 resourceDimension {0};
    quint32 miscFlag {0};
    quint32 arraySize {0};
    quint32 reserved {0};
};

QDataStream &operator>>(QDataStream &s, DDSHeaderDX10 &header);
QDataStream &operator<<(QDataStream &s, const DDSHeaderDX10 &header);
