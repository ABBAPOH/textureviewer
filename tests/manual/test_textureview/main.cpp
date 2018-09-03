#include <TextureViewWidgetsLib/TextureView>

#include <TextureViewCoreLib/TextureDocument>

#include <TextureLib/TextureIO>

#include <QtWidgets/QApplication>

#include <QtCore/QTimer>
#include <QtCore/QCommandLineParser>

struct Options
{
    QString inputFile;
};

Options parseOptions(const QStringList &arguments)
{
    QCommandLineParser parser;
    parser.addPositionalArgument(QStringLiteral("input"),
                                 QStringLiteral("Input filename"),
                                 QStringLiteral("input"));

    parser.process(arguments);

    const auto positional = parser.positionalArguments();
    if (positional.size() != 1) {
        parser.showHelp(1);
    }

    Options options;
    options.inputFile = positional.at(0);
    return options;
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(extramimetypes);
    Q_INIT_RESOURCE(images);

    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("bind_texture");

    const auto options = parseOptions(QCoreApplication::arguments());

    a.addLibraryPath(a.applicationDirPath() + TextureIO::pluginsDirPath());

    TextureIO io(options.inputFile);
    auto ok = io.read();
    if (!ok) {
        qCritical() << "Can't load texture" << toUserString(ok.error());
        return 2;
    }

    TextureViewer::TextureView w;
    w.document()->setTexture(*ok);
    w.show();

    return a.exec();
}
