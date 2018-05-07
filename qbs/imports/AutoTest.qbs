import qbs.base 1.0

ConsoleApp {
    Depends { name: "Qt.test" }

    type: ["application", "autotest"]
}
