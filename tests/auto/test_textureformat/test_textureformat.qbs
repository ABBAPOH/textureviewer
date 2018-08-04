import qbs.base 1.0

AutoTest {
    Depends { name: "Qt.gui" }
    Depends { name: "TextureLib" }

    cpp.defines: base.concat(["QT_STATICPLUGIN=1"])

    files: [ "*.cpp", "*.h" ]
}
