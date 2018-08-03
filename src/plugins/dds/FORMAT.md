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
| A2R10G10B10   |    no     |
| A16B16G16R16  |    no     |
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

| Format                | Supported |
| --------------------- |:---------:|
| R32G32B32A32_TYPELESS |    no*    |
| R32G32B32A32_FLOAT    |    no     |
| R32G32B32A32_UINT     |    no     |
| R32G32B32A32_SINT     |    no     |
| R32G32B32_TYPELESS    |    no*    |
| R32G32B32_FLOAT       |    no     |
| R32G32B32_UINT        |    no     |
| R32G32B32_SINT        |    no     |
| R16G16B16A16_TYPELESS |    no*    |
| R16G16B16A16_FLOAT    |    no     |
| R16G16B16A16_UNORM    |    no     |
| R16G16B16A16_UINT     |    no     |
| R16G16B16A16_SNORM    |    no     |
| R16G16B16A16_SINT     |    no     |
| R32G32_TYPELESS       |    no*    |
| R32G32_FLOAT          |    no     |
| R32G32_UINT           |    no     |
| R32G32_SINT           |    no     |
| R32G8X24_TYPELESS     |    no*    |
| D32_FLOAT_S8X24_UINT  |    no     |
| R32_FLOAT_X8X24_TYPELESS |    no*    |
| X32_TYPELESS_G8X24_UINT |    no*    |
| R10G10B10A2_TYPELESS  |    no*    |
| R10G10B10A2_UNORM     |    no     |
| R10G10B10A2_UINT      |    no     |
| R11G11B10_FLOAT       |    no     |
| R8G8B8A8_TYPELESS     |    no*    |
| R8G8B8A8_UNORM        |    yes    |
| R8G8B8A8_UNORM_SRGB   |    no     |
| R8G8B8A8_UINT         |    no     |
| R8G8B8A8_SNORM        |    no     |
| R8G8B8A8_SINT         |    no     |
| R16G16_TYPELESS       |    no*    |
| R16G16_FLOAT          |    no     |
| R16G16_UNORM          |    no     |
| R16G16_UINT           |    no     |
| R16G16_SNORM          |    no     |
| R16G16_SINT           |    no     |
| R32_TYPELESS          |    no*    |
| D32_FLOAT             |    no     |
| R32_FLOAT             |    no     |
| R32_UINT              |    no     |
| R32_SINT              |    no     |
| R24G8_TYPELESS        |    no*    |
| D24_UNORM_S8_UINT     |    no     |
| R24_UNORM_X8_TYPELESS |    no*    |
| X24_TYPELESS_G8_UINT  |    no*    |
| R8G8_TYPELESS         |    no*    |
| R8G8_UNORM            |    no     |
| R8G8_UINT             |    no     |
| R8G8_SNORM            |    no     |
| R8G8_SINT             |    no     |
| R16_TYPELESS          |    no*    |
| R16_FLOAT             |    no     |
| D16_UNORM             |    no     |
| R16_UNORM             |    no     |
| R16_UINT              |    no     |
| R16_SNORM             |    no     |
| R16_SINT              |    no     |
| R8_TYPELESS           |    no*    |
| R8_UNORM              |    yes    |
| R8_UINT               |    yes    |
| R8_SNORM              |    yes    |
| R8_SINT               |    yes    |
| A8_UNORM              |    no     |
| R1_UNORM              |    no     |
| R9G9B9E5_SHAREDEXP    |    no     |
| R8G8_B8G8_UNORM       |    no     |
| G8R8_G8B8_UNORM       |    no     |
| BC1_TYPELESS          |    no*    |
| BC1_UNORM             |    yes    |
| BC1_UNORM_SRGB        |    yes    |
| BC2_TYPELESS          |    no*    |
| BC2_UNORM             |    yes    |
| BC2_UNORM_SRGB        |    yes    |
| BC3_TYPELESS          |    no*    |
| BC3_UNORM             |    yes    |
| BC3_UNORM_SRGB        |    yes    |
| BC4_TYPELESS          |    no*    |
| BC4_UNORM             |    yes    |
| BC4_SNORM             |    yes    |
| BC5_TYPELESS          |    no*    |
| BC5_UNORM             |    yes    |
| BC5_SNORM             |    yes    |
| B5G6R5_UNORM          |    no     |
| B5G5R5A1_UNORM        |    no     |
| B8G8R8A8_UNORM        |    no     |
| B8G8R8X8_UNORM        |    no     |
| R10G10B10_XR_BIAS_A2_UNORM |    no     |
| B8G8R8A8_TYPELESS     |    no*    |
| B8G8R8A8_UNORM_SRGB   |    no     |
| B8G8R8X8_TYPELESS     |    no*    |
| B8G8R8X8_UNORM_SRGB   |    no     |
| BC6H_TYPELESS         |    no*    |
| BC6H_UF16             |    yes**  |
| BC6H_SF16             |    yes**  |
| BC7_TYPELESS          |    no*    |
| BC7_UNORM             |    yes**  |
| BC7_UNORM_SRGB        |    yes**  |
| AYUV                  |    no     |
| Y410                  |    no     |
| Y416                  |    no     |
| NV12                  |    no     |
| P010                  |    no     |
| P016                  |    no     |
| _420_OPAQUE           |    no     |
| YUY2                  |    no     |
| Y210                  |    no     |
| Y216                  |    no     |
| NV11                  |    no     |
| AI44                  |    no     |
| IA44                  |    no     |
| P8                    |    no     |
| A8P8                  |    no     |
| B4G4R4A4_UNORM        |    no     |
| P208                  |    no     |
| V208                  |    no     |
| V408                  |    no     |

\* TYPELESS format support is not planned ever
\*\* Needs testing on a machine that supports Bc6/Bc7 compression

### Writing

Writing DDS files **is supported**

### TODO List

* Volumemaps
* DX10 support
