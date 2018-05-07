import qbs.base 1.0

// Special hack to show includes in Qt Creator (both in source tree
// and git commit editor (otherwise untracked includes are not present)
Product {
    name: "include"
    files: ["**"]
    excludeFiles: ["*.qbs"]
}

