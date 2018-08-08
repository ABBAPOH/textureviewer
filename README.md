# Texture Viewer

Currently, there is no viewer program itself, however, simple texture can be loaded using a
[bind_texture](tests/manual/bind_texture) test.

Also, there is also a [texturetool](src/apps/texturetool) that can convert between texture
formats. However, it's capabilities are very limited at the moment - only saving DDS and PKM is
supported and no other formats other than textures (i.e. png, jpg are not supported for now).

## License
This program is distributed under the [GNU LGPL](LICENSE)

## Formats
Below is the list of formats supported by the API.
* [DDS](src/plugins/dds/FORMAT.md) (Direct Draw Surface)
* [PKM](src/plugins/pkm/FORMAT.md) (PKM Texture Format)
* [KTX](src/plugins/ktx/FORMAT.md) (Khronos Texture)
* [VTF](src/plugins/vtf/FORMAT.md) (Valve Texture Format)

## Building from source

### Build requirements

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

On debian-based systems the required packages can be installed using command

apt-get install gcc-6 g++6 libgl1-mesa-dev

### Building in Qt Creator

Open the [textureviewer.qbs](./textureviewer.qbs) file in a Qt Creator and press "Build"

### Building from command line

#### Configuring QBS
First, you need to configure QBS. Skip this step if you already did that for an other project or if
you are already using qbs and it is properly configured.

The first step is to setup the compilers.
```bash
$ qbs setup-toolchains --detect
Trying to detect gcc...
Profile 'gcc' created for '/usr/bin/gcc'.
Trying to detect clang...
Profile 'clang' created for '/usr/bin/clang'.
...
```

Second step is to configure a Qt version that you will use for building. Specify the correct path
to qmake if you have a Qt installed in a different location (i.e. on Mac and Windows)
```bash
$ qbs setup-qt /usr/bin/qmake myqt
Creating profile 'myqt'.
WARNING: You need to set up toolchain information before you can use this Qt version for building.
Consider setting one of these profiles as this profile's base profile: <list of your profiles>
```

If you get the warning above, you should manually setup the compiler that will be used for this Qt
version (the warning is shown if multiple toolchains found)
```bash
$ qbs config profiles.myqt.baseProfile <profile name>
```

The profile name can be chosen after calling the following command (choose one profile from the list
without "profiles." prefix and the profile properties after the name, i.e. simply "clang" or "gcc"
in the following example):
```bash
$ qbs config --list profiles
profiles.clang.cpp.toolchainInstallPath: "/usr/bin"
profiles.clang.qbs.toolchain: ["clang", "llvm", "gcc"]
profiles.gcc.cpp.toolchainInstallPath: "/usr/bin"
profiles.gcc.qbs.toolchain: ["gcc"]
...
```

Third step is to tell QBS which profile it should use by default.
```bash
$ qbs config defaultProfile myqt
```

Otherwise, you will get strange warning during the compilation saying "Dependency 'Qt.core' not
found for product \'<product name>\'". You also can manually specify profile when building.

For more details see Qt documentation how to
- [configure qbs](http://doc.qt.io/qbs/configuring.html)
- [configure a Qt version](http://doc.qt.io/qbs/qt-versions.html)

#### Building TextureViewer

```bash
$ cd textureviewer/
$ mkdir build
$ cd build/
$ qbs build -f ../textureviewer.qbs qbs.defaultBuildVariant:release
$ qbs install -f ../textureviewer.qbs qbs.defaultBuildVariant:release --install-root <install path>
```

### Installing mimetypes

On Linux OS, QMimeDatabase uses system information about mimetypes, so additional mimetypes should
be installed in the system. This is done using linux_mimetypes.sh script (run as root):

```bash
$ sudo ./linux_mimetypes.sh
Unknown media type in type 'all/all'
Unknown media type in type 'all/allfiles'
$

```
