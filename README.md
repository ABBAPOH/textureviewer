# Texture Viewer

Currently, there is no viewer program itself, however, simple texture can be loaded using a
[bind_texture](tests/manual/bind_texture) test.

Also, there is also a [texturetool](src/apps/texturetool) that can convert between texture
formats. However, it's capabilities are too limited at the moment - only saving DDS is supported
and no other formats other than textures (i.e. png, jpg are not supported for now).

## License
This program is distributed under the [GNU LGPL](LICENSE)

## Formats
Below is the list of formats supported by the API.
* [DDS](src/plugins/dds/FORMAT.md) (Direct Draw Surface)
* [PKM](src/plugins/pkm/FORMAT.md) (PKM Texture Format)
* [KTX](src/plugins/ktx/FORMAT.md) (Khronos Texture)
* [VTF](src/plugins/vtf/FORMAT.md) (Valve Texture Format)

## Building from source

Windows
- Microsoft Visual Studio 17 or higher
- Qt 5.11.0 (maybe earlier versions will work too)
- Qbs 1.11.1 (packed with Qt Creator)

Mac OS
- XCode 9.4.1 or higher
- Qt 5.11.0 (maybe earlier versions will work too)
- Qbs 1.11.1 (packed with Qt Creator)

Linux
- opengl headers
- GCC-6/G++-6 or higher
- Qt 5.11.0 (maybe earlier versions will work too)
- Qbs 1.11.1

On debian-based systems below is the required packages
apt-get install gcc-6 g++6 libgl1-mesa-dev

Open the textureviewer.qbs file in a Qt Creator and press "Build"
