#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "application.h"
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
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(m_view = new TextureView);

    ui->leftPaneDockWidget->setTitleBarWidget(new QWidget);

    initConnections();
}

ObserverPointer<TextureView> MainWindow::view() const
{
    return ObserverPointer<TextureView>(m_view);
}

MainWindow::~MainWindow() = default;

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
    connect(ui->actionOpen, &QAction::triggered, &Application::open);
    connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);

    // edit menu
    connect(ui->actionConvert, &QAction::triggered, this, &MainWindow::convert);

    // help menu
    connect(ui->actionAboutQt, &QAction::triggered, &QApplication::aboutQt);
    connect(ui->actionTextureFormats, &QAction::triggered,
            this, &MainWindow::showTextureFormatsDialog);

    auto onTextureChanged = [this]()
    {
        using size_type = Texture::size_type;
        ui->levelSpinBox->setValue(0);
        ui->levelSpinBox->setMaximum(std::max<size_type>(0, m_view->document()->levels() - 1));
        ui->levelSpinBox->setEnabled(ui->levelSpinBox->maximum() != 0);

        ui->layerSpinBox->setValue(0);
        ui->layerSpinBox->setMaximum(std::max<size_type>(0, m_view->document()->layers() - 1));
        ui->layerSpinBox->setEnabled(ui->layerSpinBox->maximum() != 0);

        ui->faceSpinBox->setValue(0);
        ui->faceSpinBox->setMaximum(std::max<size_type>(0, m_view->document()->faces() - 1));
        ui->faceSpinBox->setEnabled(ui->faceSpinBox->maximum() != 0);
    };
    connect(m_view->document().get(), &TextureDocument::textureChanged, this, onTextureChanged);

    auto onCurrentChanged = [this]()
    {
        m_view->control()->setLevel(ui->levelSpinBox->value());
        m_view->control()->setLayer(ui->layerSpinBox->value());
        m_view->control()->setFace(ui->faceSpinBox->value());
    };

    connect(ui->levelSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, onCurrentChanged);
    connect(ui->layerSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, onCurrentChanged);
    connect(ui->faceSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, onCurrentChanged);
}

} // namespace TextureViewer
