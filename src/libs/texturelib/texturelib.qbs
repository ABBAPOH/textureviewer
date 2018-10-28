import qbs.base 1.0

Lib {
    Depends { name: "Qt.gui" }

    Export {
        Depends { name: "Qt.gui" }
    }

    name: "TextureLib"
    cpp.defines: base.concat(["QT_STATICPLUGIN=1", "TEXTURELIB_LIBRARY"])
    files: [ "*.cpp", "*.h" ]
}
