import qbs.base 1.0

ConsoleApp {
    Depends { name: "Qt.gui" }
    Depends { name: "TextureLib" }
    Depends { name: "ExtraMimeTypesLib" }
//    Depends { name: "Utils" }

    files: [
        "*.cpp",
        "*.h"
    ]

    Group {
        fileTagsFilter: ["application"]
        qbs.install: true
        qbs.installDir: project.install_binary_path
    }
}
