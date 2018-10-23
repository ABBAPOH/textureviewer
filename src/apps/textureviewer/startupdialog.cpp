#include "startupdialog.h"
#include "ui_startupdialog.h"

#include "mainwindow.h"

namespace TextureViewer {

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog)
{
    ui->setupUi(this);

    connect(ui->openButton, &QPushButton::clicked, this, &StartupDialog::openTexture);
}

StartupDialog::~StartupDialog() = default;

void StartupDialog::openTexture()
{
    if (MainWindow::open())
        accept();
}

} // namespace TextureViewer
