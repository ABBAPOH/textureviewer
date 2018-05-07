import qbs.base 1.0

BaseProduct {
    Depends { name: "Qt.gui" }

    type: "dynamiclibrary"
    destinationDirectory: project.install_plugin_path + "/imageformats2"
    bundle.isBundle: false

    cpp.sonamePrefix: "@rpath/Frameworks"
    cpp.rpaths: qbs.targetOS.contains("osx")
                ? [ "@loader_path/..", "@executable_path/.." ]
                : [ "$ORIGIN" ]

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: project.install_plugin_path + "/imageformats2"
    }
}
