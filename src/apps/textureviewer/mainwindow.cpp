#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initConnections();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initConnections()
{
    connect(ui->actionQuit, &QAction::triggered, &QCoreApplication::quit);
}
