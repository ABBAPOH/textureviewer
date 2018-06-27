import qbs.base 1.0

// We use name extramimetypes.qrc to prevent possible collision with Qt's own file mimetypes.qrc
Lib {
    Depends { name: "Qt.core" }
    name: "ExtraMimeTypesLib"
    files: [ "*.qrc" ]
}
