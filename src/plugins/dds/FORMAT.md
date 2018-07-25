## Direct Draw Surface Format

[Programming guide](https://docs.microsoft.com/en-us/windows/desktop/direct3ddds/dx-graphics-dds-pguide)

[Reference for DDS](https://docs.microsoft.com/en-us/windows/desktop/direct3ddds/dx-graphics-dds-reference)

DDS stores bytes in a reversed order, so the format A8R8G8B8 in a table belows is actually a
B8G8R8A8 format. The same is true for all uncompressed formats.

### Direct3D 9

| Format        | Supported |
| ------------- |:---------:|
| R8G8B8        |    yes    |
| A8R8G8B8      |    yes    |
| X8R8G8B8      |    yes    |
| R5G6B5        |    yes    |
| X1R5G5B5      |    yes    |
| A1R5G5B5      |    yes    |
| A4R4G4B4      |    yes    |
| R3G3B2        |    yes    |
| A8            |    yes    |
| A8R3G3B2      |    no     |
| X4R4G4B4      |    yes    |
| A2B10G10R10   |    yes    |
| A8B8G8R8      |    yes    |
| X8B8G8R8      |    yes    |
| G16R16        |    no     |
| A2R10G10B10   |    yes    |
| A16B16G16R16  |    yes    |
| A8P8          |    no     |
| P8            |    no     |
| L8            |    yes    |
| A8L8          |    yes    |
| A4L4          |    no     |
| V8U8          |    no     |
| L6V5U5        |    no     |
| X8L8V8U8      |    no     |
| Q8W8V8U8      |    no     |
| V16U16        |    no     |
| A2W10V10U10   |    no     |
| UYVY          |    no     |
| R8G8B8G8      |    no     |
| YUY2          |    no     |
| G8R8G8        |    no     |
| DXT1          |    yes    |
| DXT2          |    no     |
| DXT3          |    yes    |
| DXT4          |    no     |
| DXT5          |    yes    |
| RXGB          |    no     |
| ATI2          |    no     |
| D16Lockable   |    no     |
| D32           |    no     |
| D15S1         |    no     |
| D24S8         |    no     |
| D24X8         |    no     |
| D24X4S4       |    no     |
| D16           |    no     |
| D32FLockable  |    no     |
| D24FS8        |    no     |
| D32Lockable   |    no     |
| S8Lockable    |    no     |
| L16           |    no     |
| VertexData    |    no     |
| Index16       |    no     |
| Index32       |    no     |
| Q16W16V16U16  |    no     |
| Multi2ARGB8   |    no     |
| R16F          |    no     |
| G16R16F       |    no     |
| A16B16G16R16F |    yes    |
| R32F          |    no     |
| G32R32F       |    no     |
| A32B32G32R32F |    no     |
| CxV8U8        |    no     |
| A1            |    no     |
| A2B10G10R10_XR_BIAS |    no     |
| BinaryBuffer  |    no     |
| P4            |    no     |
| A4P4          |    no     |

### Direct3D 10/11 (DX10)

Not supported

### Writing

Writing DDS files **is supported**

### TODO List

* Cubemaps
* Volumemaps
* DX10 support
