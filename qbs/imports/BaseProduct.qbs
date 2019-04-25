import qbs.base 1.0

Product {
    Depends { name: "cpp" }

    cpp.cFlags: project.cFlags
    cpp.cxxFlags: project.cxxFlags
    cpp.linkerFlags: project.linkFlags
    cpp.includePaths: project.includePaths
    cpp.libraryPaths: project.libraryPaths
    cpp.cxxLanguageVersion: "c++17"
    cpp.cxxStandardLibrary: qbs.targetOS.contains("macos") ? "libc++" : base
//    cpp.minimumOsxVersion: "10.14"
    cpp.minimumMacosVersion: "10.14"

    Depends { name: "bundle" }
    Depends { name: "Qt.core" }
}
