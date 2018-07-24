#ifndef ENUMS_H
#define ENUMS_H

#include <QtCore/qglobal.h>

enum class DDSFormat : quint32 {
    Unknown              = 0,

    R8G8B8               = 20,
    A8R8G8B8             = 21,
    X8R8G8B8             = 22,
    R5G6B5               = 23,
    X1R5G5B5             = 24,
    A1R5G5B5             = 25,
    A4R4G4B4             = 26,
    R3G3B2               = 27,
    A8                   = 28,
    A8R3G3B2             = 29,
    X4R4G4B4             = 30,
    A2B10G10R10          = 31,
    A8B8G8R8             = 32,
    X8B8G8R8             = 33,
    G16R16               = 34,
    A2R10G10B10          = 35,
    A16B16G16R16         = 36,

    A8P8                 = 40,
    P8                   = 41,

    L8                   = 50,
    A8L8                 = 51,
    A4L4                 = 52,

    V8U8                 = 60,
    L6V5U5               = 61,
    X8L8V8U8             = 62,
    Q8W8V8U8             = 63,
    V16U16               = 64,
    A2W10V10U10          = 67,

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

    D16Lockable         = 70,
    D32                  = 71,
    D15S1                = 73,
    D24S8                = 75,
    D24X8                = 77,
    D24X4S4              = 79,
    D16                  = 80,

    D32FLockable        = 82,
    D24FS8               = 83,

    D32Lockable         = 84,
    S8Lockable          = 85,

    L16                  = 81,

    VertexData           =100,
    Index16              =101,
    Index32              =102,

    Q16W16V16U16         = 110,

    Multi2ARGB8         = 0x3154454d, // "MET1"

    R16F                 = 111,
    G16R16F              = 112,
    A16B16G16R16F        = 113,

    R32F                 = 114,
    G32R32F              = 115,
    A32B32G32R32F        = 116,

    CxV8U8               = 117,

    A1                   = 118,
    A2B10G10R10_XR_BIAS  = 119,
    BinaryBuffer         = 199,

    P4,
    A4P4,

    Last                 = 0x7fffffff
};

#endif // ENUMS_H
