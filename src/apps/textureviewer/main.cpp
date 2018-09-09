#include "mainwindow.h"

#include <TextureLib/TextureIO>

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(extramimetypes);

    QApplication app(argc, argv);

    QCoreApplication::addLibraryPath(
            QCoreApplication::applicationDirPath() + TextureIO::pluginsDirPath());

    TextureViewer::MainWindow w;
    const auto arguments = QCoreApplication::arguments();
    if (arguments.size() == 2)
        w.openDocument(QUrl::fromLocalFile(arguments.at(1)));
    w.show();

    return app.exec();
}
