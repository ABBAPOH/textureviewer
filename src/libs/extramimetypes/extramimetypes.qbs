import qbs.base 1.0

// We use name extramimetypes.qrc to prevent possible collision with Qt's own file mimetypes.qrc
BaseProduct {
    type: "staticlibrary"
    Depends { name: "Qt.core" }
    name: "TestImagesLib"
    files: [ "*.qrc" ]
}
