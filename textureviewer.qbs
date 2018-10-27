import qbs.base 1.0

Project {
    property string app_target: qbs.targetOS.contains("macos") ? "Texture Viewer" : "textureviewer"

    property string install_app_path: {
        if (qbs.targetOS.contains("macos") || qbs.targetOS.contains("windows"))
            return ".";
        else
            return "bin";
    }

    property string install_binary_path: {
        if (qbs.targetOS.contains("macos"))
            return app_target + ".app/Contents/MacOS"
        else
            return install_app_path
    }

    property string lib_suffix: ""

    property string install_library_path: {
        if (qbs.targetOS.contains("macos"))
            return app_target + ".app/Contents/Frameworks"
        else if (qbs.targetOS.contains("windows"))
            return install_app_path
        else
            return "lib" + lib_suffix + "/" + app_target
    }

    property string install_plugin_path: {
        if (qbs.targetOS.contains("macos"))
            return app_target + ".app/Contents/PlugIns"
        else
            return install_library_path + "/plugins"
    }

    property string install_data_path: {
        if (qbs.targetOS.contains("macos"))
            return app_target + ".app/Contents/Resources"
        else
            return "share/" + app_target
    }

    property stringList includePaths: [ path + "/include/" ]
    property stringList libraryPaths: []

    property stringList cFlags: []
    property stringList cxxFlags: {
        if (qbs.targetOS.contains("macos"))
            return ["-Wno-unknown-pragmas"];
        if (qbs.targetOS.contains("windows"))
            return ["/Zc:__cplusplus"]
        return []
    }
    property stringList linkFlags: []

    qbsSearchPaths: "qbs"
    minimumQbsVersion: "1.6"

    name: "Texture Viewer"

    references: [
        "include/include.qbs",
        "src/src.qbs",
         "tests/tests.qbs",
    ]
}

