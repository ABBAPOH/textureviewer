#include "ktxheader.h"

#include <gsl/span>

QDataStream&operator>>(QDataStream& s, KtxHeader& header)
{
    for (auto &byte: gsl::span<quint8>(header.identifier)) {
        s >> byte;
    }

    s.setByteOrder(QDataStream::BigEndian);
    s >> header.endianness;
    if (header.endianness == 0x01020304)
        s.setByteOrder(QDataStream::LittleEndian);
    else if (header.endianness == 0x04030201)
        s.setByteOrder(QDataStream::BigEndian);
    else
        return s;

    s >> header.glType;
    s >> header.glTypeSize;
    s >> header.glFormat;
    s >> header.glInternalFormat;
    s >> header.glBaseInternalFormat;
    s >> header.pixelWidth;
    s >> header.pixelHeight;
    s >> header.pixelDepth;
    s >> header.numberOfArrayElements;
    s >> header.numberOfFaces;
    s >> header.numberOfMipmapLevels;
    s >> header.bytesOfKeyValueData;

    return s;
}

QDebug&operator<<(QDebug& d, const KtxHeader& header)
{
    d << "KtxHeader {"
      << "identifier:" << QLatin1String(reinterpret_cast<const char*>(header.identifier), 12) << ","
      << "endianness:" << QString::number(header.endianness, 16) << ","
      << "glType:" << QStringLiteral("0x%1").arg(header.glType, 0, 16) << ","
      << "glTypeSize:" << header.glTypeSize << ","
      << "glFormat:" << QStringLiteral("0x%1").arg(header.glFormat, 0, 16) << ","
      << "glInternalFormat:" << QStringLiteral("0x%1").arg(header.glInternalFormat, 0, 16) << ","
      << "glBaseInternalFormat:"
      << QStringLiteral("0x%1").arg(header.glBaseInternalFormat, 0, 16) << ","
      << "pixelWidth:" << header.pixelWidth << ","
      << "pixelHeight:" << header.pixelHeight << ","
      << "pixelDepth:" << header.pixelDepth << ","
      << "numberOfArrayElements:" << header.numberOfArrayElements << ","
      << "numberOfFaces:" << header.numberOfFaces << ","
      << "numberOfMipmapLevels:" << header.numberOfMipmapLevels << ","
      << "bytesOfKeyValueData:" << header.bytesOfKeyValueData << ","
      << "}";

    return d;
}
