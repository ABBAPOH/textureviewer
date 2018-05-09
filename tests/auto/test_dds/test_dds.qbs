import qbs.base 1.0

AutoTest {
    Depends { name: "Qt.gui" }
    Depends { name: "TextureLib" }
    files: [ "*.cpp", "*.h", "*.qrc" ]
}
