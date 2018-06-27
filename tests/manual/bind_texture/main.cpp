#include <QGuiApplication>
#include <QTimer>
#include <TextureLib/TextureIO>

#include "window.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(extramimetypes);

    QGuiApplication a(argc, argv);

    a.addLibraryPath(a.applicationDirPath() + TextureIO::pluginsDirPath());

    QSurfaceFormat fmt;
    fmt.setVersion(3, 2);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    Window w;
    QTimer::singleShot(0, &w, &Window::show);

    return a.exec();
}
