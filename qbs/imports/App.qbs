import qbs.base 1.0
import qbs.FileInfo

BaseProduct {
    Depends { name: "cpp" }
    Depends { name: "bundle" }

    type: "application"
    destinationDirectory: project.install_app_path

    cpp.rpaths: qbs.targetOS.contains("osx")
                ? [ "@executable_path/.." ]
                : [ "$ORIGIN/../lib/" + project.app_target ]

    Group {
        fileTagsFilter: ["application"]
        qbs.install: true
        qbs.installDir: bundle.isBundle
                        ? FileInfo.joinPaths(project.install_app_path, FileInfo.path(bundle.executablePath))
                        : project.install_app_path
    }
}
