import qbs.base 1.0

BaseProduct {
    type: "dynamiclibrary"

    destinationDirectory: project.install_library_path
    bundle.isBundle: false

    cpp.sonamePrefix: qbs.targetOS.contains("macos") ? "@rpath/Frameworks" : ""
    cpp.rpaths: qbs.targetOS.contains("macos")
                ? [ "@loader_path/..", "@executable_path/.." ]
                : [ "$ORIGIN" ]

    Group {
        fileTagsFilter: {
            if (bundle.isBundle) {
                return ["bundle.content"];
            } else {
                var result = [
                            "dynamiclibrary",
                            "dynamiclibrary_symlink",
                            "dynamiclibrary_import",
                        ];
                if (qbs.debugInformation)
                    result.push("debuginfo_dll");
                return result;
            }
        }
        qbs.install: true
        qbs.installDir: project.install_library_path
        qbs.installSourceBase: project.buildDirectory + '/' + product.destinationDirectory
    }
}
