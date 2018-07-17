#include "ktxheader.h"

QDataStream&operator>>(QDataStream& s, KtxHeader& header)
{
    for (int i = 0; i < 12; ++i) {
        s >> header.identifier[i];
    }

    const auto bo = s.byteOrder();
    s.setByteOrder(QDataStream::BigEndian);
    s >> header.endianness;
    if (header.endianness == 0x01020304)
        s.setByteOrder(QDataStream::LittleEndian);

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

    s.setByteOrder(bo);
    return s;
}

QDebug&operator<<(QDebug& d, const KtxHeader& header)
{
    d << "KtxHeader {"
      << "identifier:" << QLatin1String(reinterpret_cast<const char*>(header.identifier), 12) << ","
      << "endianness:" << QString::number(header.endianness, 16) << ","
      << "glType:" << header.glType << ","
      << "glTypeSize:" << header.glTypeSize << ","
      << "glFormat:" << header.glFormat << ","
      << "glInternalFormat:" << header.glInternalFormat << ","
      << "glBaseInternalFormat:" << header.glBaseInternalFormat << ","
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
