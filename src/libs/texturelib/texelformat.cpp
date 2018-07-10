#include "texelformat.h"

static const TexelFormat formats[] = {
    {},
    {Texture::Format::RGBA_8888, 32, 0},
    {Texture::Format::BGRA_8888, 32, 0},
    {Texture::Format::RGB_888, 24, 0},
    {Texture::Format::DXT1, 0, 8},
    {Texture::Format::DXT3, 0, 16},
    {Texture::Format::DXT5, 0, 16},
};

static_assert (sizeof (formats) == sizeof(TexelFormat) * size_t(Texture::Format::FormatsCount),
               "Some Texture::Format eniumerations are not handled in an array");

const TexelFormat &TexelFormat::texelFormat(Texture::Format format)
{
    return formats[int(format)];
}
