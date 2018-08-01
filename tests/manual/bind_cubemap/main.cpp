#include "window.h"

#include <TextureLib/TextureIO>

#include <QtGui/QGuiApplication>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(extramimetypes);
    Q_INIT_RESOURCE(images);

    QGuiApplication a(argc, argv);

    QCoreApplication::setApplicationName("bind_cubemap");

    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + TextureIO::pluginsDirPath());

    QSurfaceFormat fmt;
    // NOTE: default depth buffer size is -1
    fmt.setDepthBufferSize(24);
    fmt.setVersion(3, 2);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    Window w;
    w.show();

    return QCoreApplication::exec();
}
