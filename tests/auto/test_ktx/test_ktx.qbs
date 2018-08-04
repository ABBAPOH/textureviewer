import qbs.base 1.0

AutoTest {
    Depends { name: "Qt.gui" }
    Depends { name: "TextureLib" }
    Depends { name: "TestImagesLib" }
    Depends { name: "ExtraMimeTypesLib" }
    files: [ "*.cpp", "*.h", "*.qrc" ]
}
