import qbs.base 1.0

Product {
    Depends { name: "cpp" }

    cpp.cFlags: project.cFlags
    cpp.cxxFlags: project.cxxFlags
    cpp.linkerFlags: project.linkFlags
    cpp.includePaths: project.includePaths
    cpp.libraryPaths: project.libraryPaths
    cpp.cxxLanguageVersion: {
        if (qbs.targetOS.contains("macos") || qbs.targetOS.contains("windows"))
            return "c++14"; // XCode 9.4 does not support c++17 headers yet
        return "c++17";
    }
    cpp.cxxStandardLibrary: qbs.targetOS.contains("osx") ? "libc++" : base
    cpp.minimumOsxVersion: "10.7"

    Depends { name: "bundle" }
    Depends { name: "Qt.core" }
}
