#include "vtfheader.h"

QDataStream &operator>>(QDataStream &s, VTFHeader &header)
{
    quint32 flags {0};
    s.setByteOrder(QDataStream::LittleEndian);
    s.setFloatingPointPrecision(QDataStream::SinglePrecision);

    s >> header.signature;
    s >> header.version[0];
    s >> header.version[1];
    s >> header.headerSize;
    s >> header.width;
    s >> header.height;
    s >> flags;
    header.flags = VTFFlag(flags);
    s >> header.frames;
    s >> header.firstFrame;
    for (int i = 0; i < 4; ++i)
        s >> header.padding0[i]; // skip padding
    for (int i = 0; i < 3; ++i)
        s >> header.reflectivity[i];
    for (int i = 0; i < 4; ++i)
        s >> header.padding1[i];
    s >> header.bumpmapScale;
    s >> header.highResImageFormat;
    s >> header.mipmapCount;
    s >> header.lowResImageFormat;
    s >> header.lowResImageWidth;
    s >> header.lowResImageHeight;

    if (header.version[0] >= 7) {
        if (header.version[1] >= 2)
            s >> header.depth;
        else
            header.depth = 0;
        if (header.version[1] >= 3) {
            for (int i = 0; i < 3; ++i)
                s >> header.padding2[i];
            s >> header.numResources;
        } else {
            header.numResources = 0;
        }
    }

    return s;
}

QDataStream &operator<<(QDataStream &s, const VTFHeader &header)
{
    s.setFloatingPointPrecision(QDataStream::SinglePrecision);

    uchar padding = 0xff;

    s << header.signature;
    s << header.version[0];
    s << header.version[1];
    s << header.headerSize;
    s << header.width;
    s << header.height;
    s << quint32(header.flags);
    s << header.frames;
    s << header.firstFrame;
    for (int i = 0; i < 4; ++i)
        s << padding; // write padding
    for (int i = 0; i < 3; ++i)
        s << header.reflectivity[i];
    for (int i = 0; i < 4; ++i)
        s << padding;
    s << header.bumpmapScale;
    s << header.highResImageFormat;
    s << header.mipmapCount;
    s << header.lowResImageFormat;
    s << header.lowResImageWidth;
    s << header.lowResImageHeight;

    if (header.version[0] >= 7) {
        if (header.version[1] >= 2)
            s << header.depth;
        if (header.version[1] >= 3) {
            for (int i = 0; i < 3; ++i)
                s << padding;
            s << header.numResources;
        }
    }

    return s;
}

QDebug &operator<<(QDebug &d, const VTFHeader &header)
{
    d << "VtfHeader {"
      << "signature:" << QString("0x%1").arg(header.signature, 0, 16) << ","
      << "version:" << QString("%1.%2").arg(header.version[0]).arg(header.version[1]) << ","
      << "headerSize:" << header.headerSize << ","
      << "width:" << header.width << ","
      << "height:" << header.height << ","
      << "flags:" << header.flags << ","
      << "frames:" << header.frames << ","
      << "firstFrame:" << header.firstFrame << ","
      << "reflectivity: ["
      << header.reflectivity[0] << ","
      << header.reflectivity[1] << ","
      << header.reflectivity[2] << ","
      << "]"
      << "bumpmapScale:" << header.bumpmapScale << ","
      << "highResImageFormat:" << header.highResImageFormat << ","
      << "lowResImageWidth:" << header.lowResImageWidth << ","
      << "lowResImageHeight:" << header.lowResImageHeight << ","
      << "depth:" << header.depth << ","
      << "numResources:" << header.numResources
      << "}";

    return d;
}
