import qbs.base 1.0

Lib {
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }
    cpp.defines: base.concat(["TEXTUREVIEWCORE_LIBRARY"])
    name: "TextureViewCoreLib"
    files: [ "*.cpp", "*.h" ]
}
