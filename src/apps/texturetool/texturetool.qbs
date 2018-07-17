import qbs.base 1.0

ConsoleApp {
    Depends { name: "Qt.gui" }
    Depends { name: "TextureLib" }
//    Depends { name: "Utils" }

    files: [
        "*.cpp",
        "*.h"
    ]
}
