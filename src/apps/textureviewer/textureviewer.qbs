import qbs.base 1.0

App {
    name: project.app_target

    Depends { name: "Qt.core" }
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }
    Depends { name: "ExtraMimeTypesLib" }
    Depends { name: "TextureLib" }
    Depends { name: "TextureViewCoreLib" }
    Depends { name: "TextureViewWidgetsLib" }

    files: [
        "*.cpp",
        "*.h",
        "*.ui",
    ]

    Group {
        fileTagsFilter: ["application"]
        qbs.install: true
        qbs.installDir: project.install_app_path
    }
}
