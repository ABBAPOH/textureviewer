## Valve Texture Format

[Format description](https://developer.valvesoftware.com/wiki/Valve_Texture_Format)

[Nem's Tools](http://nemesis.thewavelength.net/index.php?c=149)

[vtf.h](https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/vtf/vtf.h)

| Format             | Supported |
| ------------------ |:---------:|
| RGBA_8888          |    yes    |
| ABGR_8888          |    yes    |
| RGB_888            |    yes    |
| BGR_888            |    yes    |
| RGB_565            |    yes    |
| I8                 |    yes    |
| IA88               |    yes    |
| P8                 |    no     |
| A8                 |    yes    |
| RGB_888_BlueScreen |(see below)|
| BGR_888_BlueScreen |(see below)|
| ARGB_8888          |    yes    |
| BGRA_8888          |    yes    |
| DXT1               |    yes    |
| DXT3               |    yes    |
| DXT5               |    yes    |
| BGRX_8888          |    yes    |
| BGR_565            |    yes    |
| BGRX_5551          |    yes    |
| BGRA_4444          |    yes    |
| DXT1_ONEBITALPHA   |    yes    |
| BGRA_5551          |    yes    |
| UV88               |    no     |
| UVWQ_8888          |    no     |
| RGBA_16161616F     |    yes    |
| RGBA_16161616      |    yes    |
| UVLX_8888          |    no     |

* BlueScreen formats are converted to RGB_888 and BGR_888

### Writing

Writing VTF files is **not** supported

### TODO List

* Check volume textures
