import qbs.base 1.0

// We use name extramimetypes.qrc to prevent possible collision with Qt's own file mimetypes.qrc
BaseProduct {
    Depends { name: "Qt.core" }
    Export {
        Depends { name: "Qt.core" }
    }
    type: "staticlibrary"
    name: "ExtraMimeTypesLib"
    files: [ "*.qrc" ]
}
