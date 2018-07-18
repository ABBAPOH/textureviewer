#ifndef VTFHEADER_H
#define VTFHEADER_H

#include <QtCore/QDataStream>
#include <QtCore/QDebug>

struct VTFHeader
{
    quint32 signature; // File signature ("VTF\0"). (or as little-endian integer, 0x00465456)
    quint32 version[2]; // version[0].version[1] (currently 7.2).
    quint32 headerSize;  // Size of the header struct  (16 byte aligned; currently 80 bytes) + size of the resources dictionary (7.3+).
    quint16 width; // Width of the largest mipmap in pixels. Must be a power of 2.
    quint16 height; // Height of the largest mipmap in pixels. Must be a power of 2.
    quint32 flags; // VTF flags.
    quint16 frames; // Number of frames, if animated (1 for no animation).
    quint16 firstFrame;  // First frame in animation (0 based).
    uchar padding0[4];  // reflectivity padding (16 byte alignment).
    float  reflectivity[3]; // reflectivity vector.
    uchar padding1[4];  // reflectivity padding (8 byte packing).
    float  bumpmapScale;  // Bumpmap scale.
    quint32 highResImageFormat; // High resolution image format.
    uchar mipmapCount;  // Number of mipmaps.
    quint32 lowResImageFormat; // Low resolution image format (always DXT1).
    uchar lowResImageWidth; // Low resolution image width.
    uchar lowResImageHeight; // Low resolution image height.

    // 7.2+
    quint16 depth {0};   // Depth of the largest mipmap in pixels.
                        // Must be a power of 2. Can be 0 or 1 for a 2D texture (v7.2 only).

    // 7.3+
    uchar padding2[3];  // depth padding (4 byte alignment).
    quint32 numResources {0};  // Number of resources this vtf has
};

QDataStream &operator>>(QDataStream &s, VTFHeader &header);
QDataStream &operator<<(QDataStream &s, const VTFHeader &header);

QDebug &operator<<(QDebug &d, const VTFHeader &header);

struct VTFResourceEntry
{
    quint32 type; // the type of the resource, see VTFResourceType enum
    quint32 data; // Resource data or offset from the beginning of the file
};

inline QDataStream &operator>>(QDataStream &s, VTFResourceEntry &entry)
{
    return s >> entry.type >> entry.data;
}

inline QDataStream &operator<<(QDataStream &s, const VTFResourceEntry &entry)
{
    return s << entry.type << entry.data;
}

#endif // VTFHEADER_H
