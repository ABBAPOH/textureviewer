#ifndef VTFENUMS_H
#define VTFENUMS_H

#include <qglobal.h>

enum class VTFImageFormat : qint32
{
    None = -1,
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
    UVLX_8888
};

#endif // VTFENUMS_H
