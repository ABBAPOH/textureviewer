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
