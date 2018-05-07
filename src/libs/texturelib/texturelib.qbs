import qbs.base 1.0

Lib {
    Depends { name: "Qt.gui" }
    cpp.defines: base.concat(["QT_STATICPLUGIN=1", "TEXTURELIB_LIBRARY"])
    name: "TextureLib"
    files: [ "*.cpp", "*.h" ]
}
