#include "application.h"
#include "mainwindow.h"

#include <TextureViewCoreLib/TextureDocument>

#include <TextureViewWidgetsLib/TextureView>

#include <TextureLib/TextureIO>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <QtCore/QSettings>

namespace TextureViewer {

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
}

QString Application::showOpenDialog()
{
    QSettings settings;
    const auto lastOpenedFile = settings.value(QStringLiteral("lastOpenedFile")).toString();
    return QFileDialog::getOpenFileName(nullptr, tr("Open"), lastOpenedFile);
}

bool Application::open()
{
    const auto path = showOpenDialog();
    if (path.isEmpty())
        return false;
    return openPath(path);
}

bool Application::openPath(const QString &path)
{
    if (path.isEmpty())
        return false;

    const auto window = new MainWindow;

    std::pair<bool, QString> result = {true, QString()};
    QEventLoop loop;
    auto onOpenFinished = [&loop, path, &result](bool ok, const QString &error)
    {
        loop.quit();
        result.first = ok;
        result.second = error;
    };
    auto document = window->view()->document();
    auto connection = connect(document.get(), &AbstractDocument::openFinished, onOpenFinished);
    window->view()->document()->open(QUrl::fromLocalFile(path));
    loop.exec();
    disconnect(connection);

    if (!result.first) {
        QMessageBox::warning(
                    nullptr,
                    tr("Open"),
                    tr("Can't open %1: %2").arg(path, result.second));
        delete window;
        return false;
    }

    window->show();

    QSettings settings;
    settings.setValue(QStringLiteral("lastOpenedFile"), path);
    return true;
}

} // namespace TextureViewer
