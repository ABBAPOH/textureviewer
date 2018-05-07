import qbs.base 1.0

BaseProduct {
    type: "staticlibrary"
    destinationDirectory: project.install_library_path
    bundle.isBundle: false
}
