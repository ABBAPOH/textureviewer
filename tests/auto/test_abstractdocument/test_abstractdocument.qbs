import qbs.base 1.0

AutoTest {
    Depends { name: "Qt.gui" }
    Depends { name: "UtilsLib" }

    files: [ "*.cpp", "*.h" ]
}
