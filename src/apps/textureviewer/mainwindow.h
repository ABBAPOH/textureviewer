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
class ThumbnailsModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    static bool open();
    static bool openPath(const QString &path);
    void convert();
    void showTextureFormatsDialog();

private:
    void initConnections();

private:
    std::unique_ptr<Ui::MainWindow> ui;

    TextureView *m_view {nullptr};
    std::unique_ptr<ThumbnailsModel> m_thumbModel;

    QString m_path;
};

} // namespace TextureViewer

#endif // MAINWINDOW_H
