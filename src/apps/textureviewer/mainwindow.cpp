#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <TextureLib/TextureIO>

#include <TextureViewCoreLib/TextureDocument>
#include <TextureViewCoreLib/TextureItemModel>
#include <TextureViewWidgetsLib/TextureView>

#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

namespace TextureViewer {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_layersModel(new TextureItemModel(TextureItemModel::Dimension::Layer)),
    m_levelsModel(new TextureItemModel(TextureItemModel::Dimension::Level))
{
    ui->setupUi(this);
    setCentralWidget(m_view = new TextureView);

    m_layersModel->setDocument(m_view->document());
    ui->layersView->setModel(m_layersModel.get());
    ui->layersDockWidget->setTitleBarWidget(new QWidget);
    ui->layersDockWidget->hide();

    m_levelsModel->setDocument(m_view->document());
    ui->levelsView->setModel(m_levelsModel.get());
    ui->levelsDockWidget->setTitleBarWidget(new QWidget);
    ui->levelsDockWidget->hide();

    initConnections();
}

MainWindow::~MainWindow()
{
}

void MainWindow::open()
{
    const auto path = QFileDialog::getOpenFileName(this, tr("Open"), QString());
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
}

void MainWindow::initConnections()
{
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);

    connect(m_layersModel.get(), &QAbstractItemModel::modelReset,
            this, [this](){ ui->layersDockWidget->setVisible(m_layersModel->rowCount() > 1); });
    connect(m_levelsModel.get(), &QAbstractItemModel::modelReset,
            this, [this](){ ui->levelsDockWidget->setVisible(m_levelsModel->rowCount() > 1); });

    const auto onLayerChanged = [this](QModelIndex current, QModelIndex)
    {
        const auto layer = current.row();
        m_view->control()->setLayer(layer);
        m_levelsModel->setLayer(layer);
    };
    connect(ui->layersView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, onLayerChanged);

    const auto onLevelChanged = [this](QModelIndex current, QModelIndex)
    {
        const auto level = current.row();
        m_view->control()->setLevel(level);
        m_layersModel->setLevel(level);
    };
    connect(ui->levelsView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, onLevelChanged);
}

} // namespace TextureViewer
