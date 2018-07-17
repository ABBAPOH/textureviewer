#ifndef KTXHEADER_H
#define KTXHEADER_H

#include <QtCore/QDataStream>
#include <QtCore/QDebug>

struct KtxHeader
{
    quint8 identifier[12];
    quint32 endianness;
    quint32 glType;
    quint32 glTypeSize;
    quint32 glFormat;
    quint32 glInternalFormat;
    quint32 glBaseInternalFormat;
    quint32 pixelWidth;
    quint32 pixelHeight;
    quint32 pixelDepth;
    quint32 numberOfArrayElements;
    quint32 numberOfFaces;
    quint32 numberOfMipmapLevels;
    quint32 bytesOfKeyValueData;
};

QDataStream &operator>>(QDataStream &s, KtxHeader &header);

QDebug &operator<<(QDebug &d, const KtxHeader &header);

#endif // KTXHEADER_H
