import qbs.base 1.0

BaseProduct {
    type: "dynamiclibrary"

    destinationDirectory: project.install_library_path
    bundle.isBundle: false

    cpp.sonamePrefix: qbs.targetOS.contains("osx") ? "@rpath/Frameworks" : ""
    cpp.rpaths: qbs.targetOS.contains("osx")
                ? [ "@loader_path/..", "@executable_path/.." ]
                : [ "$ORIGIN" ]

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_library_path
    }
}
