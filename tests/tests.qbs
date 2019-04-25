import qbs.base 1.0

Project {
    references: [
        "auto/auto.qbs",
        "manual/manual.qbs",
        "shared/shared.qbs",
    ]

    AutotestRunner {
        Depends { name: "Qt.core" }

        environment: {
            var env = base;
            if (qbs.hostOS.contains("darwin") && qbs.targetOS.contains("darwin")) {
                env.push("DYLD_FRAMEWORK_PATH=" + Qt.core.libPath);
                env.push("DYLD_LIBRARY_PATH=" + Qt.core.libPath);
            }
            return env;
        }
    }
}

