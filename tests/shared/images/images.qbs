import qbs.base 1.0

BaseProduct {
    type: "staticlibrary"
    Depends { name: "Qt.core" }
    name: "TestImagesLib"
    files: [ "*.qrc" ]
}
