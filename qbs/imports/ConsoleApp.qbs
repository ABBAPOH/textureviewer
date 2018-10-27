import qbs.base 1.0

BaseProduct {
    type: ["application"]

    destinationDirectory: project.install_binary_path
    consoleApplication: true

    cpp.rpaths: qbs.targetOS.contains("osx")
                ? [ "@executable_path/.." ]
                : [ "$ORIGIN/../lib/" + project.app_target ]
}
