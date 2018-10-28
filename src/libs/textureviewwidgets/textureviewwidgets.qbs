import qbs.base 1.0

Lib {
    name: "TextureViewWidgetsLib"

    Depends { name: "TextureViewCoreLib" }
    Export {
        Depends { name: "TextureViewCoreLib" }
    }

    cpp.defines: base.concat(["TEXTUREVIEWWIDGETS_LIBRARY"])
    files: [ "*.cpp", "*.h" ]
}
