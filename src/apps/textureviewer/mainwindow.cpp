#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "convertdialog.h"
#include "textureformatsdialog.h"

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

bool MainWindow::open()
{
    QSettings settings;
    const auto lastOpenedFile = settings.value(QStringLiteral("lastOpenedFile")).toString();
    const auto path = QFileDialog::getOpenFileName(nullptr, tr("Open"), lastOpenedFile);
    if (path.isEmpty())
        return false;

    openPath(path);
    return true;
}

bool MainWindow::openPath(const QString &path)
{
    if (path.isEmpty())
        return false;

    const auto window = new MainWindow;
    window->show();

    TextureIO io(path);
    const auto result = io.read();
    if (!result) {
        QMessageBox::warning(
                window,
                tr("Open"),
                tr("Can't open %1: %2").arg(path, toUserString(result.error())));
        window->close();
        window->deleteLater();
        return false;
    }

    window->m_path = path;
    window->m_view->document()->setTexture(*result);

    window->ui->actionConvert->setEnabled(true);

    QSettings settings;
    settings.setValue(QStringLiteral("lastOpenedFile"), path);
    return true;
}

void MainWindow::convert()
{
    ConvertDialog dialog;
    if (dialog.exec() == QDialog::DialogCode::Rejected)
        return;
    bool ok = m_view->document()->convert(dialog.format(), dialog.alignment());
    if (!ok) {
        QMessageBox::warning(this, tr("Convert"), tr("Can't convert texture"));
    }
}

void MainWindow::showTextureFormatsDialog()
{
    TextureViewer::TextureFormatsDialog dialog;
    dialog.exec();
}

void MainWindow::initConnections()
{
    connect(ui->actionOpen, &QAction::triggered, &MainWindow::open);
    connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);

    // edit menu
    connect(ui->actionConvert, &QAction::triggered, this, &MainWindow::convert);

    // help menu
    connect(ui->actionAboutQt, &QAction::triggered, &QApplication::aboutQt);
    connect(ui->actionTextureFormats, &QAction::triggered,
            this, &MainWindow::showTextureFormatsDialog);

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
