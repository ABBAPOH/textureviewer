#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <TextureLib/TextureIO>

#include <TextureViewCoreLib/TextureDocument>
#include <TextureViewCoreLib/TextureItemModel>
#include <TextureViewCoreLib/ThumbnailsModel>
#include <TextureViewWidgetsLib/TextureView>

#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <QtCore/QSettings>

namespace TextureViewer {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_thumbModel(std::make_unique<ThumbnailsModel>())
{
    ui->setupUi(this);
    setCentralWidget(m_view = new TextureView);

    m_thumbModel->setDocument(m_view->document());
    ui->leftPaneView->setModel(m_thumbModel.get());
    ui->leftPaneDockWidget->setTitleBarWidget(new QWidget);

    initConnections();
}

MainWindow::~MainWindow() = default;

void MainWindow::open()
{
    QSettings settings;
    const auto lastOpenedFile = settings.value(QStringLiteral("lastOpenedFile")).toString();
    const auto path = QFileDialog::getOpenFileName(this, tr("Open"), lastOpenedFile);
    if (path.isEmpty())
        return;

    openDocument(QUrl::fromLocalFile(path));
}

void MainWindow::openDocument(const QUrl &url)
{
    if (!m_url.isEmpty()) {
        const auto window = new MainWindow;
        window->openDocument(url);
        window->show();
        return;
    }

    if (url.isEmpty()) {
        m_url = url;
        return;
    }

    TextureIO io(url.toLocalFile());
    const auto result = io.read();
    if (!result) {
        QMessageBox::warning(
                this,
                tr("Open"),
                tr("Can't open %1: %2").arg(url.toLocalFile(), toUserString(result.error())));
        return;
    }

    m_url = url;
    m_view->document()->setTexture(*result);

    QSettings settings;
    settings.setValue(QStringLiteral("lastOpenedFile"), url.toLocalFile());
}

void MainWindow::initConnections()
{
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);

    auto onCurrentChanged = [this](QModelIndex current, QModelIndex)
    {
        const auto pos = m_thumbModel->position(current);
        if (pos.layer == -1 || pos.level == -1) // not valid
            return;
        m_view->control()->setLevel(pos.level);
        m_view->control()->setLayer(pos.layer);
        m_view->control()->setFace(pos.face);
    };

    connect(ui->leftPaneView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, onCurrentChanged);
}

} // namespace TextureViewer
