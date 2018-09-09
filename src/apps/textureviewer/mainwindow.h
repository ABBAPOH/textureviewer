#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include <QtCore/QUrl>

#include <memory>

namespace Ui {
class MainWindow;
} // namespace Ui

namespace TextureViewer {
class TextureView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void open();
    void openDocument(const QUrl &url);

private:
    void initConnections();

private:
    std::unique_ptr<Ui::MainWindow> ui;

    TextureView *m_view {nullptr};
    QUrl m_url;
};

} // namespace TextureViewer

#endif // MAINWINDOW_H
