import qbs.base 1.0

App {
    Depends { name: "Qt.core" }
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }
    Depends { name: "TextureLib" }
    Depends { name: "TextureViewCoreLib" }
    Depends { name: "TextureViewWidgetsLib" }

    files: [
        "*.cpp",
        "*.h",
        "mainwindow.ui",
    ]

    Group {
        fileTagsFilter: ["application"]
        qbs.install: true
        qbs.installDir: project.install_binary_path
    }
}
