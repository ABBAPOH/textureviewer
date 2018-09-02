//! [0]

auto format = TextureFormat::ARGB8_Unorm;
auto width = 256, height = 256, depth = 256;

auto texture1D = Texture(format, {width}); // create a 1D texture
auto texture2D = Texture(format, {width, heigth}); // create a 2D texture
auto texture3D = Texture(format, {width, heigth, depth}); // create a 3D texture

//! [0]

//! [1]

auto size = Texture::Size();
size.setWidth(256);
size.setHeight(256);
size.setDepth(256);
auto texture = Texture(TextureFormat::ARGB8_Unorm, size);

//! [1]
