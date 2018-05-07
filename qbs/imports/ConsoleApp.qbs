import qbs.base 1.0

BaseProduct {
    type: ["application"]

    destinationDirectory: project.install_binary_path

    consoleApplication: true

    cpp.cFlags: project.cFlags
    cpp.cxxFlags: project.cxxFlags
    cpp.linkerFlags: project.linkFlags
    cpp.includePaths: project.includePaths
    cpp.libraryPaths: project.libraryPaths
    cpp.cxxLanguageVersion: "c++11"
    cpp.cxxStandardLibrary: qbs.targetOS.contains("osx") ? "libc++" : base
    cpp.minimumOsxVersion: "10.7"
    cpp.rpaths: qbs.targetOS.contains("osx")
                ? [ "@executable_path/.." ]
                : [ "$ORIGIN/../lib/" + project.app_target ]
}
