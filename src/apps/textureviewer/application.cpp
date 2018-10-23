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

    TextureIO io(path);
    const auto result = io.read();
    if (!result) {
        QMessageBox::warning(
                nullptr,
                tr("Open"),
                tr("Can't open %1: %2").arg(path, toUserString(result.error())));
        return false;
    }

    const auto window = new MainWindow;

    window->view()->document()->setTexture(*result);

    window->show();

    QSettings settings;
    settings.setValue(QStringLiteral("lastOpenedFile"), path);
    return true;
}

} // namespace TextureViewer
