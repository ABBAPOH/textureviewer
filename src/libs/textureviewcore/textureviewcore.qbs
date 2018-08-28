import qbs.base 1.0

Lib {
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }
    Depends { name: "TextureLib" }
    Depends { name: "UtilsLib" }
    cpp.defines: base.concat(["TEXTUREVIEWCORE_LIBRARY"])
    name: "TextureViewCoreLib"
    files: [ "*.cpp", "*.h" ]
}
