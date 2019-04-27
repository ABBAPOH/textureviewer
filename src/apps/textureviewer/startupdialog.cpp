#include "startupdialog.h"
#include "ui_startupdialog.h"

#include "application.h"

#include <QtWidgets/QMenu>

namespace TextureViewer {

StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog)
{
    ui->setupUi(this);

    const auto recentFiles = Application::recentFiles();
    const auto recentMenu = new QMenu(ui->openRecentButton);
    for (const auto &recent: recentFiles) {
        auto action = recentMenu->addAction(recent);
        connect(action, &QAction::triggered,
                this, [this, recent](){ Application::openPath(recent); accept(); });
    }
    ui->openRecentButton->setMenu(recentMenu);
    ui->openRecentButton->setEnabled(!recentFiles.isEmpty());

    connect(ui->openButton, &QPushButton::clicked, this, &StartupDialog::openTexture);
}

StartupDialog::~StartupDialog() = default;

void StartupDialog::openTexture()
{
    if (Application::open())
        accept();
}

} // namespace TextureViewer
