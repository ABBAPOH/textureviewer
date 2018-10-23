#include "mainwindow.h"
#include "startupdialog.h"

#include <TextureLib/TextureIO>

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(extramimetypes);

    QApplication app(argc, argv);

    QCoreApplication::addLibraryPath(
            QCoreApplication::applicationDirPath() + TextureIO::pluginsDirPath());
    QCoreApplication::setApplicationName(QStringLiteral("textureviewer"));
    QCoreApplication::setOrganizationName(QStringLiteral("arch"));

    const auto arguments = QCoreApplication::arguments();
    if (arguments.size() == 2) {
        if (!TextureViewer::MainWindow::openPath(arguments.at(1)))
            return 1;
    } else {
        TextureViewer::StartupDialog dialog;
        if (dialog.exec() == QDialog::DialogCode::Rejected)
            return 0;
    }

    return QCoreApplication::exec();
}
