import qbs.base 1.0

Product {
    Depends { name: "cpp" }

    cpp.cFlags: project.cFlags
    cpp.cxxFlags: project.cxxFlags
    cpp.linkerFlags: project.linkFlags
    cpp.includePaths: project.includePaths
    cpp.libraryPaths: project.libraryPaths
    cpp.cxxLanguageVersion: "c++11"
    cpp.cxxStandardLibrary: qbs.targetOS.contains("osx") ? "libc++" : base
    cpp.minimumOsxVersion: "10.7"

    Depends { name: "bundle" }
    Depends { name: "Qt.core" }
}
