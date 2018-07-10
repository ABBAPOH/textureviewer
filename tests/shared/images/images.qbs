import qbs.base 1.0

BaseProduct {
    type: "staticlibrary"
    Depends { name: "Qt.core" }
    name: "ExtraMimeTypesLib"
    files: [ "*.qrc" ]
}
