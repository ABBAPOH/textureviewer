import qbs.base 1.0

Lib {
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }
    cpp.defines: base.concat(["UTILS_LIBRARY"])
    name: "UtilsLib"
    files: [ "*.cpp", "*.h" ]
}
