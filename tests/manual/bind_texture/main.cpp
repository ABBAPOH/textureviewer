#include <QGuiApplication>
#include <QTimer>
#include <TextureLib/TextureIO>

#include "window.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(extramimetypes);
    Q_INIT_RESOURCE(images);

    QGuiApplication a(argc, argv);

    if (argc != 2) {
        qInfo() << "Usage: ./bind_texture [texture]";
        return 1;
    }

    a.addLibraryPath(a.applicationDirPath() + TextureIO::pluginsDirPath());

    TextureIO io(QString::fromLocal8Bit(argv[1]));
    auto ok = io.read();
    if (!ok) {
        qCritical() << "Can't load texture" << toUserString(ok.error());
        return 2;
    }

    QSurfaceFormat fmt;
    fmt.setVersion(3, 2);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    Window w(*ok);
    QTimer::singleShot(0, &w, &Window::show);

    return a.exec();
}
