import qbs.base 1.0

AutoTest {
    Depends { name: "Qt.gui" }
    Depends { name: "ExtraMimeTypesLib" }
    Depends { name: "TextureLib" }
    Depends { name: "TestImagesLib" }
    files: [ "*.cpp", "*.h", "*.qrc" ]
}
