import qbs.base 1.0

Lib {
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }
    Depends { name: "TextureLib" }
    Depends { name: "TextureViewCoreLib" }
    Depends { name: "UtilsLib" }
    cpp.defines: base.concat(["TEXTUREVIEWWIDGETS_LIBRARY"])
    name: "TextureViewWidgetsLib"
    files: [ "*.cpp", "*.h" ]
}
