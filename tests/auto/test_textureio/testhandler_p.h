#pragma once

#include <TextureLib/Texture>

struct TestImageData
{
    quint32 magic {0};
    Texture::Type type {Texture::Type::None};
    Texture::Format format {Texture::Format::Invalid};
    int width;
    int height;
    int depth;
    int layers;
    int levels;
    QByteArray data;
};

QDataStream &operator <<(QDataStream &stream, const TestImageData &data);
QDataStream &operator >>(QDataStream &stream, TestImageData &data);
