#include <QGuiApplication>
#include <QTimer>
#include <QCommandLineParser>
#include <TextureLib/TextureIO>

#include "window.h"

struct Options
{
    bool coreProfile {false};
    QString inputFile;
};

Options parseOptions(const QStringList &arguments)
{
    QCommandLineParser parser;
    QCommandLineOption coreProfileOption(QStringLiteral("core-profile"),
                                         QStringLiteral("Use OpenGL 3.3 instead of OpenGL ES"));
    parser.addOption(coreProfileOption);
    parser.addPositionalArgument(QStringLiteral("input"),
                                 QStringLiteral("Input filename"),
                                 QStringLiteral("input"));

    parser.process(arguments);

    const auto positional = parser.positionalArguments();
    if (positional.size() != 1) {
        parser.showHelp(1);
    }

    Options options;
    options.coreProfile = parser.isSet(coreProfileOption);
    options.inputFile = positional.at(0);
    return options;
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(extramimetypes);
    Q_INIT_RESOURCE(images);

    QGuiApplication a(argc, argv);

    QCoreApplication::setApplicationName("bind_texture");

    const auto options = parseOptions(QCoreApplication::arguments());

    a.addLibraryPath(a.applicationDirPath() + TextureIO::pluginsDirPath());

    TextureIO io(options.inputFile);
    auto ok = io.read();
    if (!ok) {
        qCritical() << "Can't load texture" << toUserString(ok.error());
        return 2;
    }

    QSurfaceFormat fmt;
    if (options.coreProfile) {
        fmt.setVersion(3, 3);
        fmt.setProfile(QSurfaceFormat::CoreProfile);
    }
    QSurfaceFormat::setDefaultFormat(fmt);

    Window w(*ok, options.coreProfile);
    QTimer::singleShot(0, &w, &Window::show);

    return a.exec();
}
